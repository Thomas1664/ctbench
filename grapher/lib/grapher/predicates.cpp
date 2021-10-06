#include "grapher/predicates.hpp"
#include "grapher/utils/json.hpp"

#include <regex>
#include <string>

#include <nlohmann/json.hpp>

#include <llvm/Support/raw_ostream.h>

namespace grapher::predicates {

/// \defgroup predicates
/// Predicates for group descriptors.

/// \ingroup predicates
/// Generates a regex predicate from constraint.
inline auto regex(nlohmann::json const &constraint) {
  // Validating pointer parameter
  return [pointer = nlohmann::json::json_pointer{json_value<std::string>(
              constraint, "pointer")},
          regex = json_value<std::string>(constraint, "regex")](
             nlohmann::json const &value) -> bool {
    if (!value.contains(pointer) || !value[pointer].is_string()) {
      return false;
    }

    return std::regex_match(json_value<std::string>(value, pointer),
                            std::regex(regex.data()));
  };
}

/// \ingroup predicates
/// Generates a match predicate from constraint.
inline auto match(nlohmann::json const &constraint) {
  return [pointer = nlohmann::json::json_pointer(
              json_value<std::string>(constraint, "pointer")),
          matcher_flat =
              json_value<nlohmann::json>(constraint, "matcher").flatten()](
             nlohmann::json const &value) -> bool {
    for (auto const &[k, v] : matcher_flat.items()) {
      if (value[nlohmann::json::json_pointer(k)] != v) {
        return false;
      }
    }
    return true;
  };
}

/// \ingroup predicates
/// Generates a streq predicate from constraint.
inline auto streq(nlohmann::json const &constraint) {
  return [pointer = nlohmann::json::json_pointer{json_value<std::string>(
              constraint, "pointer")},
          str = json_value<std::string>(constraint, "string")](
             nlohmann::json const &value) -> bool {
    if (!value.contains(pointer) || !value[pointer].is_string()) {
      return false;
    }

    return value[pointer].get_ref<nlohmann::json::string_t const &>() == str;
  };
}

} // namespace grapher::predicates

namespace grapher {

/// \ingroup predicates
/// Builds predicate and stores it in an std::function object.
predicate_t get_predicate(nlohmann::json const &constraint) {
  std::string constraint_type = json_value<std::string>(constraint, "type");

  if (constraint_type == "regex") {
    return predicates::regex(constraint);
  }
  if (constraint_type == "match") {
    return predicates::match(constraint);
  }
  if (constraint_type == "streq") {
    return predicates::streq(constraint);
  }

  llvm::errs() << "[ERROR] Invalid constraint type:\n"
               << constraint.dump(2) << '\n';
  std::exit(1);
}

} // namespace grapher
