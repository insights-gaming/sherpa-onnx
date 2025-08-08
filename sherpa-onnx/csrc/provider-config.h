// sherpa-onnx/csrc/provider-config.h
//
// Copyright (c)  2024  Uniphore (Author: Manickavela)

#ifndef SHERPA_ONNX_CSRC_PROVIDER_CONFIG_H_
#define SHERPA_ONNX_CSRC_PROVIDER_CONFIG_H_

#include <string>

#include "onnxruntime_cxx_api.h"  // NOLINT
#include "sherpa-onnx/csrc/macros.h"
#include "sherpa-onnx/csrc/parse-options.h"

namespace sherpa_onnx {

struct CudaConfig {
  int32_t cudnn_conv_algo_search = OrtCudnnConvAlgoSearchHeuristic;

  CudaConfig() = default;
  explicit CudaConfig(int32_t cudnn_conv_algo_search)
      : cudnn_conv_algo_search(cudnn_conv_algo_search) {}

  void Register(ParseOptions *po);
  bool Validate() const;

  std::string ToString() const;
};

struct TensorrtConfig {
  int64_t trt_max_workspace_size = 2147483647;
  int32_t trt_max_partition_iterations = 10;
  int32_t trt_min_subgraph_size = 5;
  bool trt_fp16_enable = true;
  bool trt_detailed_build_log = false;
  bool trt_engine_cache_enable = true;
  bool trt_timing_cache_enable = true;
  std::string trt_engine_cache_path = ".";
  std::string trt_timing_cache_path = ".";
  bool trt_dump_subgraphs = false;

  TensorrtConfig() = default;
  TensorrtConfig(int64_t trt_max_workspace_size,
                 int32_t trt_max_partition_iterations,
                 int32_t trt_min_subgraph_size, bool trt_fp16_enable,
                 bool trt_detailed_build_log, bool trt_engine_cache_enable,
                 bool trt_timing_cache_enable,
                 const std::string &trt_engine_cache_path,
                 const std::string &trt_timing_cache_path,
                 bool trt_dump_subgraphs)
      : trt_max_workspace_size(trt_max_workspace_size),
        trt_max_partition_iterations(trt_max_partition_iterations),
        trt_min_subgraph_size(trt_min_subgraph_size),
        trt_fp16_enable(trt_fp16_enable),
        trt_detailed_build_log(trt_detailed_build_log),
        trt_engine_cache_enable(trt_engine_cache_enable),
        trt_timing_cache_enable(trt_timing_cache_enable),
        trt_engine_cache_path(trt_engine_cache_path),
        trt_timing_cache_path(trt_timing_cache_path),
        trt_dump_subgraphs(trt_dump_subgraphs) {}

  void Register(ParseOptions *po);
  bool Validate() const;

  std::string ToString() const;
};

struct OpenvinoConfig {
  std::string device_type = "CPU";
  std::string precision = "FP32";
  int32_t num_of_threads = 8;
  int32_t num_streams = 1;
  std::string cache_dir = "";
  void *context = nullptr;
  bool enable_opencl_throttling = false;
  bool enable_qdq_optimizer = false;
  std::string load_config = "";
  bool disable_dynamic_shapes = false;
  std::string model_priority = "DEFAULT";

  OpenvinoConfig() = default;
  OpenvinoConfig(const std::string &device_type, const std::string &precision,
                 int32_t num_of_threads, int32_t num_streams,
                 const std::string &cache_dir, void *context,
                 bool enable_opencl_throttling, bool enable_qdq_optimizer,
                 const std::string &load_config, bool disable_dynamic_shapes,
                 const std::string &model_priority)
      : device_type(device_type),
        precision(precision),
        num_of_threads(num_of_threads),
        num_streams(num_streams),
        cache_dir(cache_dir),
        context(context),
        enable_opencl_throttling(enable_opencl_throttling),
        enable_qdq_optimizer(enable_qdq_optimizer),
        load_config(load_config),
        disable_dynamic_shapes(disable_dynamic_shapes),
        model_priority(model_priority) {}

  void Register(ParseOptions *po);
  bool Validate() const;

  std::string ToString() const;
};

struct ProviderConfig {
  TensorrtConfig trt_config;
  CudaConfig cuda_config;
  OpenvinoConfig openvino_config;
  std::string provider = "cpu";
  int32_t device = 0;
  // device only used for cuda and trt

  ProviderConfig() = default;
  ProviderConfig(const std::string &provider, int32_t device)
      : provider(provider), device(device) {}
  ProviderConfig(const TensorrtConfig &trt_config,
                 const CudaConfig &cuda_config,
                 const OpenvinoConfig &openvino_config,
                 const std::string &provider, int32_t device)
      : trt_config(trt_config),
        cuda_config(cuda_config),
        openvino_config(openvino_config),
        provider(provider),
        device(device) {}

  void Register(ParseOptions *po);
  bool Validate() const;

  std::string ToString() const;
};

}  // namespace sherpa_onnx

#endif  // SHERPA_ONNX_CSRC_PROVIDER_CONFIG_H_
