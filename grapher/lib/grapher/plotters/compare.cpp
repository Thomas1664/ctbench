#include <filesystem>
#include <numeric>
#include <string>
#include <vector>

#include <llvm/Support/raw_ostream.h>

#include <nlohmann/json.hpp>

#include <sciplot/sciplot.hpp>

#include "grapher/core.hpp"
#include "grapher/plotters/compare.hpp"
#include "grapher/utils/config.hpp"
#include "grapher/utils/json.hpp"
#include "grapher/utils/plot.hpp"

namespace grapher::plotters {

std::string_view plotter_compare_t::get_help() const {
  return "For each matcher in the \'matchers\' JSON field, generates a graph "
         "comparing all benchmarks in the category. Each graph will be named "
         "after the feature they observe.";
}

nlohmann::json plotter_compare_t::get_default_config() const {
  // TODO
  nlohmann::json res = grapher::base_default_config();
  return grapher::base_default_config();
}

void plotter_compare_t::plot(benchmark_set_t const &cat,
                             std::filesystem::path const &dest,
                             nlohmann::json const &config) const {
  // Config

  std::vector<group_descriptor_t> group_descriptors = read_descriptors(
      json_value<std::vector<nlohmann::json>>(config, "group_descriptors"));

  nlohmann::json::json_pointer feature_value_jptr(
      json_value<std::string>(config, "value_json_pointer"));

  nlohmann::json::json_pointer feature_name_jptr(
      json_value<std::string>(config, "name_json_pointer"));

  bool draw_average = config.value("draw_average", true);
  bool draw_points = config.value("draw_points", true);

  std::string file_extension = config.value("file_extension", ".svg");

  // Draw

  for (group_descriptor_t const &descriptor : group_descriptors) {
    // Plot init
    sciplot::Plot plot;
    apply_config(plot, config);

    for (benchmark_t const &bench : cat) {
      std::vector<double> x_points;
      std::vector<double> y_points;

      std::vector<double> x_average;
      std::vector<double> y_average;

      for (entry_t const &entry : bench.entries) {
        if (entry.data.empty()) {
          llvm::errs() << "[WARNING] No event in benchmark " << bench.name
                       << " at size " << entry.size << "\n";
          continue;
        }

        std::vector<double> const values =
            get_values(entry, descriptor, feature_value_jptr);

        if (values.empty()) {
          llvm::errs() << "[WARNING] No event in benchmark " << bench.name
                       << " at size " << entry.size
                       << " matched by group descriptor " << descriptor.name
                       << ".\n";
          continue;
        }

        // Drawing points
        if (draw_points) {
          for (double value : values) {
            x_points.push_back(entry.size);
            y_points.push_back(value);
          }
        }

        // Drawing average
        if (draw_average) {
          x_average.push_back(entry.size);
          y_average.push_back(std::reduce(values.begin(), values.end()) /
                              values.size());
        }
      }

      plot.drawCurve(x_average, y_average).label(bench.name);
      plot.drawPoints(x_points, y_points).label(bench.name);
    }

    std::filesystem::create_directories(dest);
    plot.save(dest / (std::move(descriptor.name) + std::move(file_extension)));
  }
}

} // namespace grapher::plotters
