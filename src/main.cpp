#include "core/mint_module.hpp"
#include "core/mint_project.hpp"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"

#include "pipewire/core.h"
#include "pipewire/keys.h"
#include "pipewire/main-loop.h"
#include "pipewire/stream.h"
#include "spa/param/audio/raw-utils.h"
#include "spa/param/audio/raw.h"
#include "spa/param/param.h"
#include "spa/pod/builder.h"
#include "spa/utils/defs.h"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <math.h>
 
#include <spa/param/audio/format-utils.h>
 
#include <pipewire/pipewire.h>
#include <core/mint_synth.hpp>
#include <vector>

#define MINT_SYNTH_VERSION "0.1.0"

using namespace std::chrono_literals;

namespace mint_synth {
  static const struct pw_stream_events stream_events = {
    .version = PW_VERSION_STREAM_EVENTS,
    .process = mint_synth::mint_synth_core::on_process,
  };

  double get_random_double() {
    return ((double)rand()/(double)RAND_MAX);
  }
}

int main(int arg_count, char* args[]) {
  quill::Backend::start();

  mint_synth::logger = quill::Frontend::create_or_get_logger(
    "mint_synth", 
    quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1")
  );

  mint_synth::mint_synth_data data = { 
    .accumulator = 0,
  };

  struct pw_context* context;
  struct pw_core* core;
  struct spa_hook stream_listener;
  
  const struct spa_pod* params[1];
  uint8_t buffer[1024];
  struct spa_pod_builder pod_builder =  SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

  pw_init(&arg_count, &args);

  data.loop = pw_main_loop_new(NULL);
  context = pw_context_new(pw_main_loop_get_loop(data.loop), NULL, 0);

  core = pw_context_connect(context, NULL, 0);
  spa_zero(stream_listener);
  
  data.stream = pw_stream_new(
    core,
    "mint-synth", 
    pw_properties_new(
      PW_KEY_MEDIA_TYPE, "Audio",
      PW_KEY_MEDIA_CATEGORY, "Playback",
      PW_KEY_MEDIA_ROLE, "Music",
      NULL
    )
  );

  pw_stream_add_listener(data.stream, &stream_listener, &mint_synth::stream_events, &data);

  spa_audio_info_raw spa_audio_info = SPA_AUDIO_INFO_RAW_INIT(
    .format = SPA_AUDIO_FORMAT_S16,
    .rate = DEFAULT_RATE,
    .channels = DEFAULT_CHANNELS
  );

  params[0] = spa_format_audio_raw_build(
    &pod_builder, 
    SPA_PARAM_EnumFormat,
    &spa_audio_info
  );

  pw_stream_flags stream_flags = (pw_stream_flags)
    (PW_STREAM_FLAG_AUTOCONNECT |
    PW_STREAM_FLAG_MAP_BUFFERS |
    PW_STREAM_FLAG_RT_PROCESS);
   
  pw_stream_connect(
    data.stream,
    PW_DIRECTION_OUTPUT,
    PW_ID_ANY,
    stream_flags,
    params, 
    1
  );

  LOG_INFO(mint_synth::logger, "Successfully initialized Mint Synth version {}!", std::string_view{MINT_SYNTH_VERSION});

  std::thread console_thread([](pw_main_loop* loop) {
    class sine_wave_module : public mint_synth::mint_module {
    private:
      double accumulator;
    public:
      sine_wave_module() : accumulator(0.0) {}

      int16_t render(int current_frame, int n_channel) override {
        const mint_synth::mint_project& project = mint_synth::mint_synth_core::get().get_current_project();
        double frame_interval_ms = 1.0 / (double) project.get_sample_rate() * 1000.0;
        int fourth_ms = (60000 / project.get_tempo());
        int frame_per_fourth = fourth_ms / frame_interval_ms;
        double fourth_k = 1.0 - ((double) (current_frame % frame_per_fourth)  / (double) frame_per_fourth);

        this->accumulator += M_PI_M2 * 150 * fourth_k / DEFAULT_RATE;
        if (this->accumulator >= M_PI_M2) {
          this->accumulator -= M_PI_M2;
        }
        return sin(this->accumulator) * DEFAULT_VOLUME * 32676.0;
      }
    };
    /* 
    class kick_module : public mint_synth::mint_module {
      double accumulator;
      double fourth_rhythm;
      int16_t val;

      int16_t execute(std::chrono::milliseconds time_passed, int n_frame, int n_channel) {
        return this->val;
      }

      void compute_frame(std::chrono::milliseconds time_passed, int n_frame) {
        this->accumulator += M_PI_M2 * 150 * fourth_rhythm / DEFAULT_RATE;
        if (this->accumulator >= M_PI_M2) {
          this->accumulator -= M_PI_M2;
        }
        this->val = sin(this->accumulator) * DEFAULT_VOLUME * 32676.0;
      }

      void compute_execution(std::chrono::milliseconds time_passed) {
        int time_passed_ms = time_passed.count();
        int fourth_ms = (60000 / mint_synth::mint_synth_core::get().get_current_project().get_tempo());
        this->fourth_rhythm = (1 - (double)(time_passed_ms % fourth_ms) / (double)fourth_ms);
      }
    };

    class fx_module : public mint_synth::mint_module {
      double accumulator;
      int16_t val;
      double rand;
      double sixteenth_rhythm;

      int16_t execute(std::chrono::milliseconds time_passed, int n_frame, int n_channel) override {
        if (n_channel == 0) {    
          return this->val * sixteenth_rhythm;
        } else if (n_channel == 1) { 
          return this->val;
        }
        return 0;
      }

      void compute_frame(std::chrono::milliseconds time_passed, int n_frame) override {
        this->accumulator += M_PI_M2 * 15000 / DEFAULT_RATE * sixteenth_rhythm;
        if (this->accumulator >= M_PI_M2) {
          this->accumulator -= M_PI_M2;
        }
        this->val = sin(this->accumulator) * 0.2 * 32676.0; 
      }

      void compute_execution(std::chrono::milliseconds time_passed) override {
        rand = mint_synth::get_random_double(); 
        int time_passed_ms = time_passed.count();
        int fourth_ms = (60000 / mint_synth::mint_synth_core::get().get_current_project().get_tempo()) / 4;
        this->sixteenth_rhythm = (1 - (double) (time_passed_ms % fourth_ms) / (double)fourth_ms);
      }
    
    };

    static kick_module kick_mod = kick_module();
    static fx_module bass_mod = fx_module();
*/
    static auto sine_wave = sine_wave_module();

    for (;;) {
      std::string input;
      std::cin >> input;
      std::istringstream ss(input);
      std::string arg;
      std::vector<std::string> args = {};

      while (getline(ss, arg, ' ')) {
        args.push_back(arg);  
      }

      std::string arg_0 = args[0];

      // New Project
      if (arg_0 == "np") {
        //std::string arg_1 = args[1];  
        mint_synth::mint_synth_core::get().set_current_project() = mint_synth::mint_project(44100, 2, 0.7, 125, mint_synth::time_signature(4, 4), {
          &sine_wave
        });
        LOG_INFO(mint_synth::logger, "Set new project!\n");
      }

      // Quit
      if (arg_0 == "q") {
        pw_main_loop_quit(loop);
        break;
      }
    }
  }, data.loop);

  pw_main_loop_run(data.loop);

  console_thread.join();

  pw_stream_destroy(data.stream);
  pw_core_disconnect(core);
  pw_context_destroy(context);
  pw_main_loop_destroy(data.loop);

  return 0;
}
