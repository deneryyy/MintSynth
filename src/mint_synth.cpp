#include "quill/LogMacros.h"

#include "mint_project.hpp"
#include <cmath>
#include <cstdint>
#include <pipewire/pipewire.h>
#include <math.h> 
#include <mint_synth.hpp>
#include <chrono>

using namespace std::chrono_literals;

namespace mint_synth {
  double get_random_double() {
    return ((double)rand()/(double)RAND_MAX);
  }

  mint_synth_core::mint_synth_core() : current_project(mint_project(120, time_signature(4, 4))) { 
  }

  mint_synth_core mint_synth_core::core;

  mint_synth_core& mint_synth_core::get() {
    return mint_synth_core::core;
  }

  mint_project& mint_synth_core::get_current_project() {
    return this->current_project;
  }

  void write_spa_buffer(mint_synth_data* process_data, pw_buffer* pipewire_buf, spa_buffer* spa_buf, double k) {
    int i, c, n_frames, stride;
    int16_t* dst, val;
  
    if ((dst = (int16_t*) spa_buf->datas[0].data) == NULL) 
      return;

    stride = sizeof(int16_t) * DEFAULT_CHANNELS;
    n_frames = spa_buf->datas[0].maxsize / stride;
    if (pipewire_buf->requested)
      n_frames = SPA_MIN(pipewire_buf->requested, n_frames);
 
   
    for (i = 0; i < n_frames; i++) {
      process_data->accumulator += M_PI_M2 * 150 * k / DEFAULT_RATE;
      if (process_data->accumulator >= M_PI_M2) {
        process_data->accumulator -= M_PI_M2;
      }

      val = sin(process_data->accumulator) * DEFAULT_VOLUME * 32676.0;
      for (c = 0; c < DEFAULT_CHANNELS; c++) {
        double r = get_random_double();
        int mixed = (int) *(dst + 1) + val;
        if (mixed>32767) mixed=32767;
        if (mixed<-32768) mixed=-32768;
        *dst++ += val; // mixed;

        //LOG_INFO(process_data->logger, "dst: {}", *dst);
      }
    }

    spa_buf->datas[0].chunk->offset = 0;
    spa_buf->datas[0].chunk->stride = stride;
    spa_buf->datas[0].chunk->size = n_frames * stride;
  }

  void mint_synth_core::on_process(void* userdata) {
    mint_synth_data* process_data = (mint_synth_data*) userdata;

    using clock = std::chrono::high_resolution_clock;

    auto delta_time = clock::now() - process_data->time_start;
    process_data->time_start = clock::now();
    process_data->time_passed += std::chrono::duration_cast<std::chrono::milliseconds>(delta_time);

    struct pw_buffer* pipewire_buf;
    struct spa_buffer* spa_buf;
    
    if ((pipewire_buf = pw_stream_dequeue_buffer(process_data->stream)) == NULL) {
      pw_log_warn("out of buffers: %m");
      return;
    }

    spa_buf = pipewire_buf->buffer;

    mint_project& current_project = mint_synth_core::get().get_current_project();
    int time_passed_ms = process_data->time_passed.count();
    int fourth_ms = (60000 / current_project.get_tempo());
    double fourth_rhythm = (1 - (double)(time_passed_ms % fourth_ms) / (double)fourth_ms);

    write_spa_buffer(process_data, pipewire_buf, spa_buf, fourth_rhythm);
    write_spa_buffer(process_data, pipewire_buf, spa_buf, 1.0);
    
    pw_stream_queue_buffer(process_data->stream, pipewire_buf);
  }

}
