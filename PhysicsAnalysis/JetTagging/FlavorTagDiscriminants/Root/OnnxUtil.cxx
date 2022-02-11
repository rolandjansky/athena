/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <iostream>

#include "FlavorTagDiscriminants/OnnxUtil.h"
#include "PathResolver/PathResolver.h"

namespace FlavorTagDiscriminants {

  // Constructor
  OnnxUtil::OnnxUtil(const std::string& path_to_onnx){
    m_path_to_onnx = path_to_onnx;
  }

  // Destructor
  OnnxUtil::~OnnxUtil(){
  }

  void OnnxUtil::initialize(){

    //load the onnx model to memory using the path m_path_to_onnx
    m_env = std::make_unique< Ort::Env >(ORT_LOGGING_LEVEL_WARNING, "");

    // initialize session options if needed
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(
      GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // create session and load model into memory
    m_session = std::make_unique< Ort::Session >(*m_env, m_path_to_onnx.c_str(),
                                                 session_options);
    Ort::AllocatorWithDefaultOptions allocator;

    // get the input nodes
    size_t num_input_nodes = m_session->GetInputCount();
    
    // iterate over all input nodes
    for (std::size_t i = 0; i < num_input_nodes; i++) {
      char* input_name = m_session->GetInputName(i, allocator);
      m_input_node_names.push_back(std::string(input_name));
    }

    // get the output nodes
    size_t num_output_nodes = m_session->GetOutputCount();
    std::vector<int64_t> output_node_dims;

    // iterate over all output nodes
    for(std::size_t i = 0; i < num_output_nodes; i++ ) {
      char* output_name = m_session->GetOutputName(i, allocator);
      m_output_node_names.push_back(std::string(output_name));
    }
  }


  std::string OnnxUtil::getMetaData(const std::string& key){

    Ort::AllocatorWithDefaultOptions allocator;
    Ort::ModelMetadata metadata = m_session->GetModelMetadata();
    std::string val = metadata.LookupCustomMetadataMap(key.c_str(), allocator);
    return val;
  }


  void OnnxUtil::runInference(
    std::map<std::string, input_pair>& gnn_inputs,
    std::map<std::string, float>& output) const {

    // Args:
    //    gnn_inputs : {string: input_pair} 
    //    outputs    : {string: float} 

    std::vector<float> input_tensor_values;

    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(
      OrtArenaAllocator, OrtMemTypeDefault
    );

    std::vector<Ort::Value> input_tensors;
    for (auto const &node_name : m_input_node_names){
      input_tensors.push_back(Ort::Value::CreateTensor<float>(
        memory_info, gnn_inputs.at(node_name).first.data(), gnn_inputs.at(node_name).first.size(),
        gnn_inputs.at(node_name).second.data(), gnn_inputs.at(node_name).second.size())
      );
    }

    // casting vector<string> to vector<const char*>. this is what ORT expects
    std::vector<const char*> input_node_names(m_input_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_input_node_names.size()); i++) {
      input_node_names[i]= m_input_node_names.at(i).c_str();
    }
    std::vector<const char*> output_node_names(m_output_node_names.size(),nullptr);
    for (int i=0; i<static_cast<int>(m_output_node_names.size()); i++) {
      output_node_names[i]= m_output_node_names.at(i).c_str();
    }

    // score model & input tensor, get back output tensor
    auto output_tensors = m_session->Run(Ort::RunOptions{nullptr},
      input_node_names.data(), input_tensors.data(), input_node_names.size(),
      output_node_names.data(), output_node_names.size()
    );

    for (unsigned int node_idx=0; node_idx<m_output_node_names.size(); node_idx++){
      output.insert({output_node_names[node_idx],
        *output_tensors[node_idx].GetTensorMutableData<float>()});
    }
  }

} // end of FlavorTagDiscriminants namespace
