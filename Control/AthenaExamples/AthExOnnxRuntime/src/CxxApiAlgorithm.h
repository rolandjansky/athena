// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef ATHEXONNXRUNTIME_CXXAPIALGORITHM_H
#define ATHEXONNXRUNTIME_CXXAPIALGORITHM_H

// Local include(s).
#include "AthExOnnxRuntime/IONNXRuntimeSvc.h"

// Framework include(s).
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

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
   class CxxApiAlgorithm : public AthReentrantAlgorithm {

   public:
      /// Inherit the base class's constructor
      using AthReentrantAlgorithm::AthReentrantAlgorithm;

      /// @name Function(s) inherited from @c AthAlgorithm
      /// @{

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;
      /// Function executing the algorithm for a single event
      virtual StatusCode execute( const EventContext& ctx ) const override;
      /// Function finalising the algorithm
      virtual StatusCode finalize() override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// Name of the model file to load
      Gaudi::Property< std::string > m_modelFileName{ this, "ModelFileName",
         "dev/MLTest/2020-03-02/MNIST_testModel.onnx",
         "Name of the model file to load" };
      Gaudi::Property< std::string > m_pixelFileName{ this, "InputDataPixel",
         "dev/MLTest/2020-03-31/t10k-images-idx3-ubyte",
         "Name of the input pixel file to load" };
      Gaudi::Property< std::string > m_labelFileName{ this, "InputDataLabel",
         "dev/MLTest/2020-03-31/t10k-labels-idx1-ubyte",
         "Name of the label file to load" };
      Gaudi::Property<int> m_testSample {this, "TestSample", 0, "A Random Test Sample"};
      /// Handle to @c AthONNX::IONNXRuntimeSvc
      ServiceHandle< IONNXRuntimeSvc > m_svc{ this, "ONNXRuntimeSvc",
                                              "AthONNX::ONNXRuntimeSvc",
                                              "Name of the service to use" };

      /// @}

      /// The "session" of ONNX Runtime that we'll be using
      std::unique_ptr< Ort::Session > m_session;

   }; // class CxxApiAlgorithm

} // namespace AthONNX

#endif // ATHEXONNXRUNTIME_CXXAPIALGORITHM_H
