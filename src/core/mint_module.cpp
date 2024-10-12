#include <cmath>
#include <core/mint_synth.hpp>
#include <core/mint_module.hpp>
#include <cstdint>

namespace mint_synth {
  double get_random_double() {
    return ((double)rand()/(double)RAND_MAX);
  }

  int16_t mint_synth::mint_module::render(int current_frame, int channel) {
    return 0;
  }
}
