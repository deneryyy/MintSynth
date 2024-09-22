#pragma once

namespace mint_synth {
  class time_signature {
    private:
      const int upper;
      const int lower;
    public:
      time_signature(int upper, int lower);
      int get_upper();
      int get_lower();
  };
  
  class mint_project {
    private:
      const int tempo;
      const time_signature meter;
    public:
      mint_project(int tempo, time_signature meter);
      int get_tempo();
      time_signature get_time_signature();
  };
}
