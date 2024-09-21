#pragma once

#include "mint_project.hpp"
#include <pipewire/pipewire.h>

#define M_PI_M2 ( M_PI + M_PI )

#define DEFAULT_RATE     192000
#define DEFAULT_CHANNELS 2
#define DEFAULT_VOLUME   0.7

namespace mint_synth {
  struct mint_synth_data {
    struct pw_main_loop* loop;
    struct pw_stream* stream;
    double accumulator; 
    double time;
    bool state;
  };
  
  class mint_synth_core {
    private:
      static mint_synth_core instance;
      mint_project current_project;
      mint_synth_core();
    public:
      static void initialize_mint_synth();
      static mint_synth_core get(); 

      static void on_process(void* userdata);
      mint_project get_current_project();
  };
}
