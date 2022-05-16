#pragma once

#include "grapher/plotters/plotter_base.hpp"

namespace grapher::plotters {

/// \ingroup plotters
/// Debug plotter, outputs statistics on benchmark categories
struct plotter_debug_t : public plotter_base_t {
  void plot(benchmark_set_t const &bset, std::filesystem::path const &dest,
            grapher::json_t const &config) const override;

  grapher::json_t get_default_config() const override;
};

} // namespace grapher::plotters
