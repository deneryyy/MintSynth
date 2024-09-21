#pragma once

namespace mint_synth {
  class time_signature {
    private:
      int upper;
      int lower;
    public:
      time_signature(int upper, int lower);
      int* get_upper();
      int* get_lower();
  };
  
  class mint_project {
    private:
      int tempo;
      time_signature meter;
    public:
      mint_project(int tempo, time_signature meter);
  };
}
