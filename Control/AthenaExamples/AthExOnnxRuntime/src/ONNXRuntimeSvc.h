// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef ATHEXONNXRUNTIME_ONNXRUNTIMESVC_H
#define ATHEXONNXRUNTIME_ONNXRUNTIMESVC_H

// Local include(s).
#include "AthExOnnxRuntime/IONNXRuntimeSvc.h"

// Framework include(s).
#include "AthenaBaseComps/AthService.h"

// ONNX include(s).
#include <core/session/onnxruntime_cxx_api.h>

// System include(s).
#include <memory>

namespace AthONNX {

   /// Service implementing @c AthONNX::IONNXRuntimeSvc
   ///
   /// This is a very simple implementation, just managing the lifetime
   /// of some ONNX Runtime C++ objects.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class ONNXRuntimeSvc : public extends< AthService, IONNXRuntimeSvc > {

   public:
      /// Inherit the base class's constructor
      using extends::extends;

      /// @name Function(s) inherited from @c Service
      /// @{

      /// Function initialising the service
      virtual StatusCode initialize() override;
      /// Function finalising the service
      virtual StatusCode finalize() override;

      /// @}

      /// @name Function(s) inherited from @c AthONNX::IONNXRuntimeSvc
      /// @{

      /// Return the ONNX Runtime environment object
      virtual Ort::Env& env() const override;

      /// @}

   private:
      /// Global runtime environment for ONNX Runtime
      std::unique_ptr< Ort::Env > m_env;

   }; // class ONNXRuntimeSvc

} // namespace AthONNX

#endif // ATHEXONNXRUNTIME_ONNXRUNTIMESVC_H
