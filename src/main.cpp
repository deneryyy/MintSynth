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
#include <cmath>
#include <cstdint>
#include <math.h>
 
#include <spa/param/audio/format-utils.h>
 
#include <pipewire/pipewire.h>
#include <mint_synth.hpp>

#define MINT_SYNTH_VERSION "0.1.0"

namespace mint_synth {
  static const struct pw_stream_events stream_events = {
    .version = PW_VERSION_STREAM_EVENTS,
    .process = mint_synth::mint_synth_core::on_process,
  };
}

int main(int arg_count, char* args[]) {
  quill::Backend::start();

  quill::Logger* logger = quill::Frontend::create_or_get_logger(
    "root", quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"));

  mint_synth::mint_synth_core::initialize_mint_synth();

  mint_synth::mint_synth_data data = { 
    .accumulator = 0, 
    .time = 0,
    .state = false,
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
    "mint-synth-src", 
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

  LOG_INFO(logger, "Successfully initialized Mint Synth version {}!", std::string_view{MINT_SYNTH_VERSION});

  pw_main_loop_run(data.loop);
  pw_stream_destroy(data.stream);
  pw_core_disconnect(core);
  pw_context_destroy(context);
  pw_main_loop_destroy(data.loop);

  return 0;
}
