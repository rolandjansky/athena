#include <core/session/onnxruntime_cxx_api.h>
#include <iostream>

int main(int narg, char* argv[]) {
  if (narg != 3) {
    std::cout << "usage: " << argv[0] << " <onnx_file> <key>" << std::endl;
    return 1;
  }

  //load the onnx model to memory using the path
  auto env = std::make_unique< Ort::Env >(ORT_LOGGING_LEVEL_ERROR, "");

  // initialize session options if needed
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(
    GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

  // create session and load model into memory
  auto session = std::make_unique< Ort::Session >(*env, argv[1],
                                                session_options);

  // get metadata
  Ort::AllocatorWithDefaultOptions allocator;
  Ort::ModelMetadata metadata = session->GetModelMetadata();
  std::string val = metadata.LookupCustomMetadataMap(argv[2], allocator);
  std::cout << val << std::endl;
  
  return 0;
}
