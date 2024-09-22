#include <chrono>
#include <cstdint>

namespace mint_synth {
  class mint_module {
    virtual int16_t execute(std::chrono::milliseconds time_passed);
  };
}
