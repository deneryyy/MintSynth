#pragma once

#include "core/mint_project.hpp"
#include "quill/Logger.h"
#include <pipewire/pipewire.h>

#define M_PI_M2 ( M_PI + M_PI )

#define DEFAULT_RATE     44100
#define DEFAULT_CHANNELS 2
#define DEFAULT_VOLUME   0.7

namespace mint_synth {
  struct mint_synth_data {
    struct pw_main_loop* loop;
    struct pw_stream* stream; 
    double accumulator;
    int current_frame;
  };
 
  static quill::Logger* logger;

  class mint_synth_core {
    private:
      static mint_synth_core core;
      mint_project current_project;
      mint_synth_core();
    public:
      mint_synth_core(const mint_synth_core&) = delete;

      static mint_synth_core& get(); 

      static void on_process(void* userdata);

      mint_project& set_current_project();
      const mint_project& get_current_project() const;
  };  
}
