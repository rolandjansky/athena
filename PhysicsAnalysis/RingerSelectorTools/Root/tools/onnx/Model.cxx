/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "RingerSelectorTools/tools/onnx/Model.h"
#include <iostream>
using namespace std;

namespace Ringer{

  namespace onnx{

    Model::Model( std::string modelPath, AthONNX::IONNXRuntimeSvc *svc,
                  float etmin, float etmax, float etamin, float etamax,
                  unsigned barcode):
      m_etmin(etmin),
      m_etmax(etmax),
      m_etamin(etamin),
      m_etamax(etamax),
      m_barcode(barcode)

    {
      // Some ORT related initialization
      Ort::SessionOptions sessionOptions;
      sessionOptions.SetIntraOpNumThreads( 1 );
      sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );  
      m_session = std::make_shared< Ort::Session >( svc->env(), modelPath.c_str(), sessionOptions );
    }   


    void Model::compile()
    {
      Ort::AllocatorWithDefaultOptions allocator;  
      size_t num_input_nodes = m_session->GetInputCount();
      
      for( std::size_t i = 0; i < num_input_nodes; i++ ) {
        char* input_name = m_session->GetInputName(i, allocator);
        m_input_node_names.push_back(input_name);
        Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        auto input_node_dims = tensor_info.GetShape();
        for (std::size_t j = 0; j < input_node_dims.size(); j++){
          if(input_node_dims[j]<0)
            input_node_dims[j] =1;
        }  
        m_input_node_dims.push_back(input_node_dims);
      }
      
     
      // Always have only one output
      size_t num_output_nodes = m_session->GetOutputCount();      
      for( std::size_t i = 0; i < num_output_nodes; i++ ) {
        char* output_name = m_session->GetOutputName(i, allocator);
        m_output_node_names.push_back(output_name);
      }

      
    }





    float Model::predict( std::vector< std::vector<float> > &input_vecs ) const 
    {
      auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
      std::vector<Ort::Value> ort_inputs;

      size_t num_inputs = input_vecs.size();
      for( size_t i=0; i < num_inputs; ++i ){
        ort_inputs.emplace_back( Ort::Value::CreateTensor<float>(memory_info, input_vecs[i].data(), 
                                                                  input_vecs[i].size(), m_input_node_dims[i].data(), 
                                                                  m_input_node_dims[i].size()) );
      }

      // score model & input tensor, get back output tensor
      auto output_tensors = m_session->Run(Ort::RunOptions{nullptr}, m_input_node_names.data(), ort_inputs.data(), 
                                           ort_inputs.size(), m_output_node_names.data(), 1);
      
      assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
    
      // Get pointer to output tensor float values
      float* output_arr = output_tensors.front().GetTensorMutableData<float>();
      
      return output_arr[0];
    }

  }// onnx
}// ringer
