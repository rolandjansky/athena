///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// METNetHandler.cxx
// Implementation file for class METNetHandler
// Author: M.Leigh<mleigh@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METNetHandler.h"
#include "PathResolver/PathResolver.h"
#include "AsgTools/MessageCheck.h"

namespace met {

  METNetHandler::METNetHandler(std::string m_modelName) : m_modelName(m_modelName) { }
  METNetHandler::~METNetHandler() { }

  StatusCode METNetHandler::initialize() {

    // Use the path resolver to find the location of the network .onnx file
    m_modelPath = PathResolverFindCalibFile(m_modelName);
    if (m_modelPath == "") return StatusCode::FAILURE;

    // Use the default ONNX session settings for 1 CPU thread
    m_onnxSessionOptions.SetIntraOpNumThreads(1);
    m_onnxSessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC);

    // Initialise the ONNX environment and session using the above options and the model name
    m_onnxSession = std::make_unique<Ort::Session>(m_onnxEnv, m_modelPath.c_str(), m_onnxSessionOptions);

    // Values required for processing (usually automatic in a loop, but we know the model!)
    // So these can not be changed without altering METNet, so lets keep them fixed at their
    // specific values
    m_numInputs = 1;
    m_numOutputs = 1;
    m_inputDims = {1, 77};
    m_outputDims = {1, 2};
    m_graphInputNames = {"input"};
    m_graphOutputNames = {"output"};

    return StatusCode::SUCCESS;
  }

  int METNetHandler::getReqSize(){
    // Returns the required size of the inputs for the network
    return (int)m_inputDims[1];
  }

  std::vector<float> METNetHandler::predict(std::vector<float> inputs) {
    // This method passes a input vector through the neural network and returns its estimate
    // It requires conversions to onnx type tensors and back

    // Create a CPU tensor to be used as input
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>( memory_info,
                                                               inputs.data(),
                                                               inputs.size(),
                                                               m_inputDims.data(),
                                                               m_inputDims.size() );

    // Create a CPU tensor to be used as the output
    std::vector<float> outputs = {0, 0};
    Ort::Value output_tensor = Ort::Value::CreateTensor<float>( memory_info,
                                                                outputs.data(),
                                                                outputs.size(),
                                                                m_outputDims.data(),
                                                                m_outputDims.size() );

    // Pass the input through the network, getting a vector of outputs
    m_onnxSession->Run( Ort::RunOptions{nullptr},
                        m_graphInputNames.data(),  &input_tensor,  m_numInputs,
                        m_graphOutputNames.data(), &output_tensor, m_numOutputs );

    return outputs;

  }

}
