#include "grapher/utils/json.hpp"

#include <fstream>
#include <iostream>
#include <optional>

#include <llvm/Support/raw_ostream.h>

#include <nlohmann/json.hpp>

#include "grapher/predicates.hpp"
#include "grapher/utils/config.hpp"

namespace grapher {

std::vector<double> get_values(benchmark_iteration_t const &iteration,
                               group_descriptor_t const &descriptor,
                               nlohmann::json::json_pointer value_jptr) {
  std::vector<double> res;
  res.reserve(iteration.repetition_paths.size());

  for (std::filesystem::path const &repetition_path :
       iteration.repetition_paths) {
    // Extract events
    nlohmann::json j;
    {
      std::ifstream repetition_ifstream(repetition_path);
      repetition_ifstream >> j;
    }

    std::vector<nlohmann::json> events =
        json_value<std::vector<nlohmann::json>>(j, "traceEvents");

    // Filter events
    std::vector<nlohmann::json> matching_events =
        extract_group(descriptor, events);

    // Check for data
    if (matching_events.empty()) {
      continue;
    }

    // Accumulate
    double val = 0;
    for (auto const &event : matching_events) {
      val += json_value<double>(event, value_jptr);
    }
    res.push_back(val);
  }

  return res;
}

nlohmann::json merge_into(nlohmann::json a, nlohmann::json const &b) {
  for (nlohmann::json const b_flat = b.flatten();
       auto const &[k_ptr, v] : b_flat.items()) {
    a[nlohmann::json::json_pointer(k_ptr)] = v;
  }
  return a;
}

} // namespace grapher
