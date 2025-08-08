// sherpa-onnx/csrc/provider-config.cc
//
// Copyright (c)  2024  Uniphore (Author: Manickavela)

#include "sherpa-onnx/csrc/provider-config.h"

#include <set>
#include <sstream>

#include "sherpa-onnx/csrc/file-utils.h"
#include "sherpa-onnx/csrc/macros.h"

namespace sherpa_onnx {

void CudaConfig::Register(ParseOptions *po) {
  po->Register("cuda-cudnn-conv-algo-search", &cudnn_conv_algo_search,
               "CuDNN convolution algrorithm search");
}

bool CudaConfig::Validate() const {
  if (cudnn_conv_algo_search < 1 || cudnn_conv_algo_search > 3) {
    SHERPA_ONNX_LOGE(
        "cudnn_conv_algo_search: '%d' is not a valid option."
        "Options : [1,3]. Check OnnxRT docs",
        cudnn_conv_algo_search);
    return false;
  }
  return true;
}

std::string CudaConfig::ToString() const {
  std::ostringstream os;

  os << "CudaConfig(";
  os << "cudnn_conv_algo_search=" << cudnn_conv_algo_search << ")";

  return os.str();
}

void TensorrtConfig::Register(ParseOptions *po) {
  po->Register("trt-max-workspace-size", &trt_max_workspace_size,
               "Set TensorRT EP GPU memory usage limit.");
  po->Register("trt-max-partition-iterations", &trt_max_partition_iterations,
               "Limit partitioning iterations for model conversion.");
  po->Register("trt-min-subgraph-size", &trt_min_subgraph_size,
               "Set minimum size for subgraphs in partitioning.");
  po->Register("trt-fp16-enable", &trt_fp16_enable,
               "Enable FP16 precision for faster performance.");
  po->Register("trt-detailed-build-log", &trt_detailed_build_log,
               "Enable detailed logging of build steps.");
  po->Register("trt-engine-cache-enable", &trt_engine_cache_enable,
               "Enable caching of TensorRT engines.");
  po->Register("trt-timing-cache-enable", &trt_timing_cache_enable,
               "Enable use of timing cache to speed up builds.");
  po->Register("trt-engine-cache-path", &trt_engine_cache_path,
               "Set path to store cached TensorRT engines.");
  po->Register("trt-timing-cache-path", &trt_timing_cache_path,
               "Set path for storing timing cache.");
  po->Register("trt-dump-subgraphs", &trt_dump_subgraphs,
               "Dump optimized subgraphs for debugging.");
}

bool TensorrtConfig::Validate() const {
  if (trt_max_workspace_size < 0) {
    std::ostringstream os;
    os << "trt_max_workspace_size: " << trt_max_workspace_size
       << " is not valid.";
    SHERPA_ONNX_LOGE("%s", os.str().c_str());
    return false;
  }
  if (trt_max_partition_iterations < 0) {
    SHERPA_ONNX_LOGE("trt_max_partition_iterations: %d is not valid.",
                     trt_max_partition_iterations);
    return false;
  }
  if (trt_min_subgraph_size < 0) {
    SHERPA_ONNX_LOGE("trt_min_subgraph_size: %d is not valid.",
                     trt_min_subgraph_size);
    return false;
  }

  return true;
}

std::string TensorrtConfig::ToString() const {
  std::ostringstream os;

  os << "TensorrtConfig(";
  os << "trt_max_workspace_size=" << trt_max_workspace_size << ", ";
  os << "trt_max_partition_iterations=" << trt_max_partition_iterations << ", ";
  os << "trt_min_subgraph_size=" << trt_min_subgraph_size << ", ";
  os << "trt_fp16_enable=\"" << (trt_fp16_enable ? "True" : "False") << "\", ";
  os << "trt_detailed_build_log=\""
     << (trt_detailed_build_log ? "True" : "False") << "\", ";
  os << "trt_engine_cache_enable=\""
     << (trt_engine_cache_enable ? "True" : "False") << "\", ";
  os << "trt_engine_cache_path=\"" << trt_engine_cache_path.c_str() << "\", ";
  os << "trt_timing_cache_enable=\""
     << (trt_timing_cache_enable ? "True" : "False") << "\", ";
  os << "trt_timing_cache_path=\"" << trt_timing_cache_path.c_str() << "\",";
  os << "trt_dump_subgraphs=\"" << (trt_dump_subgraphs ? "True" : "False")
     << "\" )";
  return os.str();
}

void OpenvinoConfig::Register(ParseOptions *po) {
  po->Register("openvino-device-type", &device_type,
               "OpenVINO device type (CPU, NPU, GPU, etc.)");
  po->Register("openvino-precision", &precision,
               "OpenVINO precision (FP32, FP16, ACCURACY)");
  po->Register("openvino-num-of-threads", &num_of_threads,
               "Number of threads for OpenVINO execution");
  po->Register("openvino-num-streams", &num_streams,
               "Number of inference streams for OpenVINO");
  po->Register("openvino-cache-dir", &cache_dir,
               "Path to store/load OpenVINO model blobs for caching");
  po->Register("openvino-enable-opencl-throttling", &enable_opencl_throttling,
               "Enable OpenCL queue throttling to reduce CPU usage");
  po->Register("openvino-enable-qdq-optimizer", &enable_qdq_optimizer,
               "Enable QDQ optimization for improved NPU performance");
  po->Register("openvino-load-config", &load_config,
               "Path to custom JSON config file for OpenVINO parameters");
  po->Register("openvino-disable-dynamic-shapes", &disable_dynamic_shapes,
               "Rewrite dynamic-shaped models to static shapes");
  po->Register(
      "openvino-model-priority", &model_priority,
      "Model priority for resource allocation (LOW, MEDIUM, HIGH, DEFAULT)");
}

bool OpenvinoConfig::Validate() const {
  std::set<std::string> valid_devices = {"CPU", "NPU", "GPU", "GPU.0", "GPU.1"};
  if (valid_devices.find(device_type) == valid_devices.end() &&
      device_type.find("HETERO:") != 0 && device_type.find("MULTI:") != 0 &&
      device_type.find("AUTO:") != 0) {
    SHERPA_ONNX_LOGE(
        "device_type: '%s' is not valid. Valid options: CPU, NPU, GPU, "
        "GPU.0, GPU.1, HETERO:<devices>, MULTI:<devices>, AUTO:<devices>",
        device_type.c_str());
    return false;
  }

  if (num_of_threads <= 0) {
    SHERPA_ONNX_LOGE("num_of_threads: %d is not valid. Must be > 0.",
                     num_of_threads);
    return false;
  }
  
  if (num_streams <= 0) {
    SHERPA_ONNX_LOGE("num_streams: %d is not valid. Must be > 0.",
                     num_streams);
    return false;
  }

  if (device_type == "CPU" && precision != "FP32" && precision != "ACCURACY") {
    SHERPA_ONNX_LOGE("OpenVINO CPU only supports FP32 and ACCURACY precision");
    return false;
  }

  if (device_type == "GPU" && precision != "FP32" && precision != "FP16" &&
      precision != "ACCURACY") {
    SHERPA_ONNX_LOGE(
        "OpenVINO GPU only supports FP32, FP16 and ACCURACY precision");
    return false;
  }

  if (device_type == "NPU" && precision != "FP16" && precision != "ACCURACY") {
    SHERPA_ONNX_LOGE("OpenVINO NPU only supports FP16 and ACCURACY precision");
    return false;
  }

  if (model_priority != "LOW" && model_priority != "MEDIUM" &&
      model_priority != "HIGH" && model_priority != "DEFAULT") {
    SHERPA_ONNX_LOGE(
        "model_priority: '%s' is not valid. Valid options: LOW, MEDIUM, HIGH, "
        "DEFAULT",
        model_priority.c_str());
    return false;
  }

  return true;
}

std::string OpenvinoConfig::ToString() const {
  std::ostringstream os;

  os << "OpenvinoConfig(";
  os << "device_type=\"" << device_type << "\", ";
  os << "precision=\"" << precision << "\", ";
  os << "num_of_threads=" << num_of_threads << ", ";
  os << "num_streams=" << num_streams << ", ";
  os << "cache_dir=\"" << cache_dir << "\", ";
  os << "context=" << context << ", ";
  os << "enable_opencl_throttling=\""
     << (enable_opencl_throttling ? "True" : "False") << "\", ";
  os << "enable_qdq_optimizer=\"" << (enable_qdq_optimizer ? "True" : "False")
     << "\", ";
  os << "load_config=\"" << load_config << "\", ";
  os << "disable_dynamic_shapes=\""
     << (disable_dynamic_shapes ? "True" : "False") << "\", ";
  os << "model_priority=\"" << model_priority << "\")";

  return os.str();
}

void ProviderConfig::Register(ParseOptions *po) {
  cuda_config.Register(po);
  trt_config.Register(po);
  openvino_config.Register(po);

  po->Register("device", &device, "GPU device index for CUDA and Trt EP");
  po->Register("provider", &provider,
               "Specify a provider to use: cpu, cuda, coreml, openvino");
}

bool ProviderConfig::Validate() const {
  if (device < 0) {
    SHERPA_ONNX_LOGE("device: '%d' is invalid.", device);
    return false;
  }

  if (provider == "cuda" && !cuda_config.Validate()) {
    return false;
  }

  if (provider == "trt" && !trt_config.Validate()) {
    return false;
  }

  if (provider == "openvino" && !openvino_config.Validate()) {
    return false;
  }

  return true;
}

std::string ProviderConfig::ToString() const {
  std::ostringstream os;

  os << "ProviderConfig(";
  os << "device=" << device << ", ";
  os << "provider=\"" << provider << "\", ";
  os << "cuda_config=" << cuda_config.ToString() << ", ";
  os << "trt_config=" << trt_config.ToString() << ", ";
  os << "openvino_config=" << openvino_config.ToString() << ")";
  return os.str();
}

}  // namespace sherpa_onnx
