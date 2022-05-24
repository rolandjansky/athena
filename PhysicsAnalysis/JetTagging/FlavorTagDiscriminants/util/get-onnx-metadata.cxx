#include <core/session/onnxruntime_cxx_api.h>
#include <iostream>

int main(int narg, char* argv[]) {
  if (narg != 3 && narg != 2) {
    std::cout << "usage: " << argv[0] << " <onnx_file> [key]" << std::endl;
    return 1;
  }

  //load the onnx model to memory using the path
  auto env = std::make_unique< Ort::Env >(ORT_LOGGING_LEVEL_ERROR, "");

  // initialize session options if needed
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(
    GraphOptimizationLevel::ORT_DISABLE_ALL);

  // create session and load model into memory
  auto session = std::make_unique< Ort::Session >(*env, argv[1],
                                                session_options);

  // get metadata
  Ort::AllocatorWithDefaultOptions allocator;
  Ort::ModelMetadata metadata = session->GetModelMetadata();
  if (narg == 2) {
    std::cout << "keys: ";
    int64_t nkeys = 0;
    char** keys = metadata.GetCustomMetadataMapKeys(allocator, nkeys);
    for (int64_t i = 0; i < nkeys; i++) {
      std::cout << keys[i];
      if (i+1 < nkeys) std::cout << ", ";
    }
    std::cout << std::endl;
    return 2;
  }
  std::string val = metadata.LookupCustomMetadataMap(argv[2], allocator);
  std::cout << val << std::endl;

  return 0;
}
