///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// METNetHandler.h
// Header file for class METNetHandler
// Author: M.Leigh<mleigh@cern.ch>
///////////////////////////////////////////////////////////////////

#ifndef METUTILITIES_MET_METNETHANDLER_H
#define METUTILITIES_MET_METNETHANDLER_H

// STL includes
#include <string>

// Asg tool includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/MessageCheck.h"

// ONNX Library
#include <core/session/onnxruntime_cxx_api.h>

namespace met {

  class METNetHandler {

  public:

    // Constructor with parameters
    METNetHandler(std::string model_name);

    // Destructor
    virtual ~METNetHandler();

    // Public methods
    StatusCode initialize();
    int getReqSize();
    std::vector<float> predict(std::vector<float> inputs);

  private:

    // Default constructor
    METNetHandler();

    // Class properties
    std::string m_modelName; // Path to the onnx file
    std::string m_modelPath; // Output of the path resolver

    // Features of the network structure
    size_t m_numInputs;
    size_t m_numOutputs;
    std::vector<int64_t> m_inputDims;
    std::vector<int64_t> m_outputDims;
    std::vector<const char *> m_graphInputNames;
    std::vector<const char *> m_graphOutputNames;

    // ONNX session objects
    Ort::Env m_onnxEnv;
    Ort::SessionOptions m_onnxSessionOptions;
    Ort::AllocatorWithDefaultOptions m_onnxAllocator;
    std::unique_ptr<Ort::Session> m_onnxSession;
  };

}

#endif
