#include "quill/LogMacros.h"

#include <core/mint_project.hpp>
#include <cstdint>
#include <vector>
#include <core/mint_module.hpp>

namespace mint_synth {
  time_signature::time_signature(int upper, int lower) 
    : upper(upper), lower(lower) {}

  time_signature::time_signature(const time_signature& other) 
    : upper(other.upper), lower(other.lower) {}

  int time_signature::get_lower() const {
    return this->lower;
  }

  int time_signature::get_upper() const {
    return this->upper;
  }

  mint_project::mint_project(int sample_rate, int channels, double volume, int tempo, time_signature meter, std::vector<mint_module*> modules) 
    : sample_rate(sample_rate), channels(channels), volume(volume), tempo(tempo), meter(meter), modules(modules) {}

  mint_project::mint_project(const mint_project& other) 
    : tempo(other.tempo), meter(other.meter), modules(other.modules) {}

  int mint_project::get_tempo() const {
    return this->tempo;
  }

  time_signature mint_project::get_time_signature() const {
    return this->meter;  
  }

  const std::vector<mint_module*>& mint_project::get_modules() const {
    return this->modules;
  }

  const int& mint_project::get_sample_rate() const {
    return this->sample_rate;
  }

  int& mint_project::set_sample_rate() {
    return this->sample_rate;
  }

  const int& mint_project::get_channels() const {
    return this->channels;
  }

  int& mint_project::set_channels() {
    return this->channels; 
  }

  const double& mint_project::get_volume() const {
    return this->volume;
  }

  double& mint_project::set_volume() {
    return this->volume;
  }


  int16_t mint_project::mix_modules(int current_frame, int n_channel) const {
    int mixed = 0;
    for (mint_module* module : this->modules) {
      mixed += (int) module->render(current_frame, n_channel);
    }
    if (mixed>32767) mixed=32767;
    if (mixed<-32768) mixed=-32768;
    //LOG_INFO(mint_synth::logger, "mixed: {}", mixed);
    return (int16_t) mixed;
  }
}
