/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "RingerSelectorTools/tools/onnx/Model.h"

namespace Ringer{

  namespace onnx{

    Model::Model( std::string modelPath, AthONNX::IONNXRuntimeSvc *svc,
                  float etmin, float etmax, float etamin, float etamax):
      m_etmin(etmin),
      m_etmax(etmax),
      m_etamin(etamin),
      m_etamax(etamax)

    {
      // Some ORT related initialization
      Ort::SessionOptions sessionOptions;
      sessionOptions.SetIntraOpNumThreads( 1 );
      sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );  
      m_session = std::make_shared< Ort::Session >( svc->env(), modelPath.c_str(), sessionOptions );
    }   


    void Model::compile()
    {
      // Set up the ONNX Runtime session.
      Ort::AllocatorWithDefaultOptions allocator;  
      std::vector<int64_t> input_node_dims;
      size_t num_input_nodes = m_session->GetInputCount();
      std::vector<const char*> input_node_names(num_input_nodes);
      
      for( std::size_t i = 0; i < num_input_nodes; i++ ) {
        char* input_name = m_session->GetInputName(i, allocator);
        input_node_names[i] = input_name;
        Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        //ONNXTensorElementDataType type = tensor_info.GetElementType();

        input_node_dims = tensor_info.GetShape();
        for (std::size_t j = 0; j < input_node_dims.size(); j++){
          if(input_node_dims[j]<0)
            input_node_dims[j] =1;
        }  
      }
      

      m_input_node_dims = input_node_dims;
      m_input_node_names = input_node_names;
     
      
      size_t num_output_nodes = m_session->GetOutputCount();
      std::vector<const char*> output_node_names(num_output_nodes);
      
      for( std::size_t i = 0; i < num_output_nodes; i++ ) {
        char* output_name = m_session->GetOutputName(i, allocator);
        output_node_names[i]= output_name;
      }

      m_output_node_names = output_node_names;
    }





    float Model::predict( std::vector< std::vector<float> > &input_vecs ) const 
    {
      auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
      std::vector<Ort::Value> ort_inputs;
      size_t num_inputs = input_vecs.size();

      for( size_t i=0; i < num_inputs; ++i ){
        
        ort_inputs.emplace_back( Ort::Value::CreateTensor<float>(memory_info, input_vecs[i].data(), 
                                                                  input_vecs[i].size(), m_input_node_dims.data(), 
                                                                  m_input_node_dims.size()) );

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
