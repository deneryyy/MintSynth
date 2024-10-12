#pragma once

#include <cstdint>

namespace mint_synth {
  class mint_module {
    public:
      virtual int16_t render(int current_frame, int n_channel);
  };
}

