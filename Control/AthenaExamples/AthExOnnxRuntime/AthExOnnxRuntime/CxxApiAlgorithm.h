// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef ATHEXONNXRUNTIME_CXXAPIALGORITHM_H
#define ATHEXONNXRUNTIME_CXXAPIALGORITHM_H

// Framework include(s).
#include "AnaAlgorithm/AnaAlgorithm.h"

// ONNX Runtime include(s).
#include <core/session/onnxruntime_cxx_api.h>

// System include(s).
#include <memory>
#include <string>
#include <iostream> 
#include <fstream>
#include <arpa/inet.h>

namespace AthONNX {

   /// Algorithm demonstrating the usage of the ONNX Runtime C++ API
   ///
   /// In most cases this should be preferred over the C API...
   ///
   /// @author Debottam Bakshi Gupta <Debottam.Bakshi.Gupta@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class CxxApiAlgorithm : public EL::AnaAlgorithm {

   public:
      CxxApiAlgorithm (const std::string& name, ISvcLocator* pSvcLocator);
      /// @name Function(s) inherited from @c AthAlgorithm
      /// @{

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm for a single event
      virtual StatusCode execute() override;
      /// Function finalising the algorithm
      virtual StatusCode finalize() override;

      /// @}
      std::unique_ptr< Ort::Session > m_session;

   private:
      /// @name Algorithm properties
      /// @{

      /// Name of the model file to load
       std::string m_modelFileName;
       std::string m_pixelFileName;
       std::string m_labelFileName;
       int m_testSample;

   }; // class CxxApiAlgorithm

} // namespace AthONNX

#endif // ATHEXONNXRUNTIME_CXXAPIALGORITHM_H
