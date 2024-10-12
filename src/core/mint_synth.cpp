#include "quill/LogMacros.h"

#include "core/mint_project.hpp"
#include <cmath>
#include <cstdint>
#include <pipewire/pipewire.h>
#include <math.h> 
#include <core/mint_synth.hpp>
#include <core/renderer.hpp>

using namespace std::chrono_literals;

namespace mint_synth { 
  mint_synth_core::mint_synth_core() : current_project(mint_project(44100, 2, 0.7, 120, time_signature(4, 4), {})) { 
  }

  mint_synth_core mint_synth_core::core;

  mint_synth_core& mint_synth_core::get() {
    return mint_synth_core::core;
  }

  const mint_project& mint_synth_core::get_current_project() const {
    return this->current_project;
  }

  mint_project& mint_synth_core::set_current_project() {
    return this->current_project;
  }

  void write_to_master(mint_synth_data* process_data, 
                       pw_buffer* pipewire_buf, 
                       spa_buffer* spa_buf, 
                       const mint_project& current_project) {
    int n_req_frames, stride;
    int16_t* dst;
  
    if ((dst = (int16_t*) spa_buf->datas[0].data) == NULL) 
      return;

    stride = sizeof(int16_t) * DEFAULT_CHANNELS;
    n_req_frames = spa_buf->datas[0].maxsize / stride;
    if (pipewire_buf->requested)
      n_req_frames = SPA_MIN(pipewire_buf->requested, n_req_frames);

    mint_synth::render_module(current_project, dst, n_req_frames, process_data->current_frame);

    spa_buf->datas[0].chunk->offset = 0;
    spa_buf->datas[0].chunk->stride = stride;
    spa_buf->datas[0].chunk->size = n_req_frames * stride;
  }

  void mint_synth_core::on_process(void* userdata) {
    mint_synth_data* process_data = (mint_synth_data*) userdata;

    struct pw_buffer* pipewire_buf;
    struct spa_buffer* spa_buf;
    
    if ((pipewire_buf = pw_stream_dequeue_buffer(process_data->stream)) == NULL) {
      pw_log_warn("out of buffers: %m");
      return;
    }

    spa_buf = pipewire_buf->buffer;

    const mint_project& current_project = mint_synth_core::get().get_current_project();
    write_to_master(process_data, pipewire_buf, spa_buf, current_project);
    
    pw_stream_queue_buffer(process_data->stream, pipewire_buf);
  }

}
