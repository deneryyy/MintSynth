#include "mint_project.hpp"
#include <cmath>
#include <pipewire/pipewire.h>
#include <math.h> 
#include <mint_synth.hpp>

namespace mint_synth {
  double get_random_double() {
    return ((double)rand()/(double)RAND_MAX);
  }

  void mint_synth_core::initialize_mint_synth() {
    mint_synth_core::instance = mint_synth_core();
    mint_synth_core::instance.current_project = mint_project(120, time_signature(4, 4));
  }

  mint_project mint_synth_core::get_current_project() {
    return this->current_project;
  }

  void mint_synth_core::on_process(void* userdata) {
    mint_synth_data* process_data = (mint_synth_data*) userdata;
    struct pw_buffer* pipewire_buf;
    struct spa_buffer* spa_buf;
    int i, c, n_frames, stride;
    int16_t* dst, val;

    if ((pipewire_buf = pw_stream_dequeue_buffer(process_data->stream)) == NULL) {
      pw_log_warn("out of buffers: %m");
      return;
    }

    spa_buf = pipewire_buf->buffer;
    if ((dst = (int16_t*) spa_buf->datas[0].data) == NULL) 
      return;

    stride = sizeof(int16_t) * DEFAULT_CHANNELS;
    n_frames = spa_buf->datas[0].maxsize / stride;
    if (pipewire_buf->requested)
      n_frames = SPA_MIN(pipewire_buf->requested, n_frames);

    if (process_data->state) {
      process_data->time -= 0.005;
      if (process_data->time <= 0.0)
        process_data->state = false;
    } else {
      process_data->time += 0.005; 
      if (process_data->time >= 1.0)
        process_data->state = true;
    }

    double rand = mint_synth::get_random_double();

    for (i = 0; i < n_frames; i++) {
      process_data->accumulator += M_PI_M2 * 15000 * (1 - process_data->time) * rand / DEFAULT_RATE;
      if (process_data->accumulator >= M_PI_M2) {
        process_data->accumulator -= M_PI_M2;
      }

      val = sin(process_data->accumulator) * DEFAULT_VOLUME * 32676.0;
      for (c = 0; c < DEFAULT_CHANNELS; c++) {
        double r = mint_synth::get_random_double();

        if (c == 0) {
          *dst++ = val * (process_data->time - 0.5) * r;
        } else if (c == 1) {
          *dst++ = val * (process_data->time + 0.5) * r;
        }
      }
    }

    spa_buf->datas[0].chunk->offset = 0;
    spa_buf->datas[0].chunk->stride = stride;
    spa_buf->datas[0].chunk->size = n_frames * stride;

    pw_stream_queue_buffer(process_data->stream, pipewire_buf);
  }
}
