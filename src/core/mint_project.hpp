#pragma once

#include <core/mint_module.hpp>
#include <cstdint>
#include <vector>

namespace mint_synth {
  class time_signature {
    private:
      int upper;
      int lower;
    public:
      time_signature(int upper, int lower);
      time_signature(const time_signature& other);

      int get_upper() const;
      int get_lower() const;
  };
  
  class mint_project {
    private:
      int sample_rate;
      int channels;
      double volume;

      int tempo;
      time_signature meter;
      std::vector<mint_module*> modules;
    public:
      mint_project(int sample_rate, int channels, double volume, int tempo, time_signature meter, std::vector<mint_module*> modules);
      mint_project(const mint_project& other);

      const int& get_sample_rate() const;
      int& set_sample_rate();

      const int& get_channels() const;
      int& set_channels();

      const double& get_volume() const;
      double& set_volume();

      int get_tempo() const;
      time_signature get_time_signature() const;
      const std::vector<mint_module*>& get_modules() const;

      int16_t mix_modules(int current_frame, int n_channel) const;
  };
}
