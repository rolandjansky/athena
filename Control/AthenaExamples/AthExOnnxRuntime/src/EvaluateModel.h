// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef ATHEXONNXRUNTIME_EVALUATEMODEL_H
#define ATHEXONNXRUNTIME_EVALUATEMODEL_H

// Local include(s).
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"

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
#include <vector>
#include <iterator>

namespace AthONNX {

   /// Algorithm demonstrating the usage of the ONNX Runtime C++ API
   ///
   /// In most cases this should be preferred over the C API...
   ///
   /// @author Debottam Bakshi Gupta <Debottam.Bakshi.Gupta@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EvaluateModel: public AthReentrantAlgorithm {

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

      /// Following properties needed to be consdered if the .onnx model is evaluated in batch mode
      Gaudi::Property<bool> m_doBatches {this, "DoBatches", false, "Processing events by batches"};
      Gaudi::Property<int> m_numberOfBatches {this, "NumberOfBatches", 1, "No. of batches to be passed"};
      Gaudi::Property<int> m_sizeOfBatch {this, "SizeOfBatch", 1, "No. of elements/example in a batch"};
      
      /// Handle to @c AthONNX::IONNXRuntimeSvc
      ServiceHandle< IONNXRuntimeSvc > m_svc{ this, "ONNXRuntimeSvc",
                                              "AthONNX::ONNXRuntimeSvc",
                                              "Name of the service to use" };

      /// @}

      /// The "session" of ONNX Runtime that we'll be using
      std::unique_ptr< Ort::Session > m_session;
      std::vector<std::vector<float>> m_input_tensor_values;
      std::vector<int> m_output_tensor_values;

   }; // class EvaluateModel

} // namespace AthONNX

#endif // ATHEXONNXRUNTIME_EVALUATEMODEL_H
