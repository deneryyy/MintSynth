#pragma once

#include <core/mint_project.hpp>
#include <cstdint>

namespace mint_synth {
  void render_module(const mint_project& current_project, int16_t* dst, const int& n_req_frames, int& current_frame);
}
