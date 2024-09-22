#pragma once

#include "mint_project.hpp"
#include "quill/Logger.h"
#include <chrono>
#include <pipewire/pipewire.h>
#include <ratio>

#define M_PI_M2 ( M_PI + M_PI )

#define DEFAULT_RATE     192000
#define DEFAULT_CHANNELS 2
#define DEFAULT_VOLUME   0.7

namespace mint_synth {
  struct mint_synth_data {
    struct pw_main_loop* loop;
    struct pw_stream* stream; 
    quill::Logger* logger;
    double accumulator; 
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> time_start;
    std::chrono::milliseconds time_passed;
  };
  
  class mint_synth_core {
    private:
      static mint_synth_core core;
      mint_project current_project;
      mint_synth_core();
    public:
      mint_synth_core(const mint_synth_core&) = delete;

      static void initialize_mint_synth();
      static mint_synth_core& get(); 

      static void on_process(void* userdata);
      mint_project& get_current_project();
  };  
}
