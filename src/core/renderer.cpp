#include <core/renderer.hpp>
#include "core/mint_project.hpp"
#include <cstdint>

namespace mint_synth { 
  void render_module(const mint_project& current_project, int16_t* dst, const int& n_req_frames, int& current_frame) {
    for (int i = 0; i < n_req_frames; i++) {
      current_frame++;
      for (int channel = 0; channel < current_project.get_channels(); channel++) {
        *dst++ = current_project.mix_modules(current_frame, channel);
      }
    }
  }
}
