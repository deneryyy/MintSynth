#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

namespace mint_synth {
  void decode() {
    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
      
    }
  }
}
