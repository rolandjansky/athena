// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#ifndef ONNX_UTILS_H
#define ONNX_UTILS_H

#include <string>
#include <iostream> 
#include <fstream>
#include <arpa/inet.h>
#include <vector>
#include <iterator>
#include <tuple>

// ONNX Runtime include(s).
#include <core/session/onnxruntime_cxx_api.h>
// Local include(s).
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace AthONNX {

/************************Flattening of Input Data***************************/
/***************************************************************************/ 

 template<typename T>

 inline std::vector<T> FlattenInput_multiD_1D( std::vector<std::vector<T>> features){
    // 1. Compute the total size required.
    int total_size = 0;
    for (const auto& feature : features) total_size += feature.size();
    
    // 2. Create a vector to hold the data.
    std::vector<T> Flatten1D;
    Flatten1D.reserve(total_size);

    // 3. Fill it
    for (const auto& feature : features)
      for (const auto& elem : feature)
        Flatten1D.push_back(elem);
 
   return Flatten1D;
  }

/*********************************Creation of ORT tensor*********************************/
/****************************************************************************************/

 template<typename T>
 inline Ort::Value TensorCreator(std::vector<T> flattenData, std::vector<int64_t> input_node_dims ){ 
    /************** Create input tensor object from input data values to feed into your model *********************/
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault); 
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, 
                                                                  flattenData.data(), 
                                                                  flattenData.size(),  /*** 1x28x28 = 784 ***/ 
                                                                  input_node_dims.data(), 
                                                                  input_node_dims.size());     /*** [1, 28, 28] = 3 ***/
    return input_tensor;
   }


/*********************************Creation of ORT Session*********************************/
/*****************************************************************************************/

 //template<typename T>
 inline std::unique_ptr< Ort::Session > CreateORTSession(const std::string& modelFile){
   
    // Set up the ONNX Runtime session.
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads( 1 );
    sessionOptions.SetGraphOptimizationLevel( ORT_ENABLE_BASIC );

    ServiceHandle< IONNXRuntimeSvc > svc("AthONNX::ONNXRuntimeSvc",
                                              "AthONNX::ONNXRuntimeSvc");

    return std::make_unique<Ort::Session>( svc->env(),
                                     modelFile.c_str(),
                                     sessionOptions );
   }

/*********************************Input Node Structure of Model*********************************/
/***********************************************************************************************/

  inline  std::tuple<std::vector<int64_t>, std::vector<const char*> > GetInputNodeInfo(const std::unique_ptr< Ort::Session >& session){
    
    std::vector<int64_t> input_node_dims;
    size_t num_input_nodes = session->GetInputCount();
    std::vector<const char*> input_node_names(num_input_nodes);
    Ort::AllocatorWithDefaultOptions allocator;
    for( std::size_t i = 0; i < num_input_nodes; i++ ) {
     
        char* input_name = session->GetInputName(i, allocator);
        input_node_names[i] = input_name;
        Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
   
        input_node_dims = tensor_info.GetShape();  
     }
     return std::make_tuple(input_node_dims, input_node_names); 
  }

/*********************************Output Node Structure of Model*********************************/
/***********************************************************************************************/

  inline  std::tuple<std::vector<int64_t>, std::vector<const char*> > GetOutputNodeInfo(const std::unique_ptr< Ort::Session >& session){
     
     //output nodes
     std::vector<int64_t> output_node_dims;
     size_t num_output_nodes = session->GetOutputCount();
     std::vector<const char*> output_node_names(num_output_nodes);
     Ort::AllocatorWithDefaultOptions allocator;

      for( std::size_t i = 0; i < num_output_nodes; i++ ) {
        char* output_name = session->GetOutputName(i, allocator);
        output_node_names[i] = output_name;

        Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        output_node_dims = tensor_info.GetShape();
      }
    return std::make_tuple(output_node_dims, output_node_names);
   }


/*********************************Running Inference through ORT*********************************/
/***********************************************************************************************/
  inline float* Inference(const std::unique_ptr< Ort::Session >& session,std::vector<const char*>& input_node_names, Ort::Value& input_tensor, std::vector<const char*>& output_node_names){
     auto output_tensor =  session->Run(Ort::RunOptions{nullptr},
                                             input_node_names.data(),
                                             &input_tensor,
                                             input_node_names.size(),      /** 1, flatten_input:0 **/
                                             output_node_names.data(),
                                             output_node_names.size());    /** 1, dense_1/Softmax:0 **/ 
 
     //assert(output_tensor.size() == output_node_names.size() && output_tensor.front().IsTensor());
     // Get pointer to output tensor float values
     float* floatarr = output_tensor.front().GetTensorMutableData<float>();
     return floatarr;
  }

}
#endif
