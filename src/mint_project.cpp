#include <mint_project.hpp>

namespace mint_synth {
  time_signature::time_signature(int upper, int lower) 
    : upper(upper), lower(lower) {}

  int time_signature::get_lower() {
    return this->lower;
  }

  int time_signature::get_upper() {
    return this->upper;
  }

  mint_project::mint_project(int tempo, time_signature meter) 
    : tempo(tempo), meter(meter) {}

  int mint_project::get_tempo() {
    return this->tempo;
  }

  time_signature mint_project::get_time_signature() {
    return this->meter;  
  }
}
