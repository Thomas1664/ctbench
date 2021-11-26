## =============================================================================
#! # `benchmarking.cmake`
#!
#! Functions for automated benchmark declarations.
#!
## =============================================================================

## =============================================================================
#! ## `ctbench-graph-all` target
#!
#! `ctbench-graph-all` is a ctbench-provided target to generate all graphs
#! at once.
#!

add_custom_target(ctbench-graph-all)

include(cmake/internal.cmake)

# Granularity option
if(${PROJECT_IS_TOP_LEVEL})
  set(CTBENCH_TIME_TRACE_GRANULARITY 1 PARENT_SCOPE)
endif()

## =============================================================================
#! ## `ctbench_add_benchmark`
#!
#! Add a benchmark for a given source, with a given size range.
#!
#! - `name`: Name of benchmark
#! - `source`: Source file
#! - `begin, end, step`: Iteration parameters
#! - `samples`: Number of samples per iteration
#!

function(
  ctbench_add_benchmark
  name
  source
  begin
  end
  step
  samples)
  # Setting names
  add_custom_target(${name})

  foreach(iteration RANGE ${samples})
    foreach(size RANGE ${begin} ${end} ${step})
      # Subtargets aren't meant to be compiled by end-users
      set(subtarget_name "_${name}-size_${size}-it_${iteration}")

      _ctbench_internal_add_compile_benchmark(
        ${subtarget_name} "${name}/${size}/${iteration}.json" "${source}"
        "-DBENCHMARK_SIZE=${size}")

      target_compile_options(
        ${subtarget_name}
        PRIVATE -ftime-trace-granularity=${CTBENCH_TIME_TRACE_GRANULARITY})

      add_dependencies(${name} ${subtarget_name})
    endforeach()
  endforeach()

endfunction(ctbench_add_benchmark)

## =============================================================================
#! ## `ctbench_add_custom_benchmark`
#!
#! Add a benchmark for a given source with a given size range
#! using a custom compile options generator.
#!
#! - `name`: Name of benchmark
#! - `source`: Source file
#! - `begin, end, step`: Iteration parameters
#! - `iterations`: Number of benchmark iterations for a given size
#! - `generator`: Compile option generator. Takes a size and an output
#!                variable name as parameters.
#!

function(
  ctbench_add_custom_benchmark
  name
  source
  begin
  end
  step
  iterations
  generator)

  # Setting names
  add_custom_target(${name})

  foreach(iteration RANGE 1 ${iterations})
    foreach(size RANGE ${begin} ${end} ${step})
      # Subtargets aren't meant to be compiled by end-users
      set(subtarget_name "_${name}-size_${size}-it_${iteration}")

      cmake_language(CALL ${generator} ${size} ctbench_options_output)

      _ctbench_internal_add_compile_benchmark(
        ${subtarget_name} "${name}/${size}/${iteration}.json" "${source}"
        "${ctbench_options_output}")

      add_dependencies(${name} ${subtarget_name})
    endforeach()
  endforeach()

endfunction(ctbench_add_custom_benchmark)

## =============================================================================
#! ## `ctbench_add_graph`
#!
#! Adds a graph target for a set of benchmarks,
#! and adds the target to ctbench-graph-all.
#!
#! - `category`: Name of the category. This is also the name of the graph
#!               target, and the folder where the graphs will be saved.
#! - `config`: Config file for plotting
#! - `benchmarks`: List of benchmark names
#!

function(ctbench_add_graph category config)
  set(config_path ${CMAKE_CURRENT_SOURCE_DIR}/${config})
  add_custom_target(
    ${category}
    COMMAND grapher-plot --output=${category} --config=${config_path} ${ARGN}
    DEPENDS ${config_path} ${ARGN})
  add_dependencies(ctbench-graph-all ${category})
endfunction(ctbench_add_graph)
