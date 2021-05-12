#pragma once

#include <filesystem>
#include <optional>
#include <tuple>
#include <vector>

#include <nlohmann/json.hpp>

#include <grapher/core.hpp>

namespace grapher {

/// Raw entry (before proper extraction in an entry_t type)
using raw_entry_t = std::tuple<std::filesystem::path, nlohmann::json>;

/// Raw category
using raw_category_t = std::vector<entry_t>;

/// Extracts a benchmark given a path.
/// Internally, a benchmark is basically a matrix of M * N benchmark entries,
/// where M is a number of different sizes, and N is a number of iterations.
/// This implies that all benchmark sizes must have the same number of
/// iterations. Otherwise the benchmark isn't valid.
std::optional<benchmark_t> extract_benchmark(std::filesystem::path const &cat);

} // namespace grapher
