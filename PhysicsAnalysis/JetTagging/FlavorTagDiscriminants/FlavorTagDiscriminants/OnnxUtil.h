/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ONNXUTIL_H
#define ONNXUTIL_H

#include <core/session/onnxruntime_cxx_api.h>
#include <map>

namespace FlavorTagDiscriminants {

  typedef std::pair<std::vector<float>, std::vector<int64_t>> input_pair;
  
  //
  // Utility class that loads the onnx model from the given path
  // and runs inference based on the user given inputs

  class OnnxUtil final{

    public:

      OnnxUtil(const std::string& path_to_onnx);
      ~OnnxUtil();

      void initialize();

      void runInference(
        std::map<std::string, input_pair> & gnn_inputs,
        std::map<std::string, float>& output
      ) const;

      std::string getMetaData(const std::string& key);

    private:

      std::string m_path_to_onnx;

      std::unique_ptr< Ort::Session > m_session;
      std::unique_ptr< Ort::Env > m_env;

      std::vector<std::string> m_input_node_names;
      std::vector<std::string> m_output_node_names;

  }; // Class OnnxUtil
} // end of FlavorTagDiscriminants namespace

#endif //ONNXUTIL_H
