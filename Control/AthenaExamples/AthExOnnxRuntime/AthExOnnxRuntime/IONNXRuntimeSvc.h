// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef ATHEXONNXRUNTIME_IONNXRUNTIMESVC_H
#define ATHEXONNXRUNTIME_IONNXRUNTIMESVC_H

// Gaudi include(s).
#include "GaudiKernel/IService.h"

// ONNX include(s).
#include <core/session/onnxruntime_cxx_api.h>

/// Namespace holding all of the ONNX Runtime example code
namespace AthONNX {

   /// Service used for managing global objects used by ONNX Runtime
   ///
   /// In order to allow multiple clients to use ONNX Runtime at the same
   /// time, this service is used to manage the objects that must only
   /// be created once in the Athena process.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IONNXRuntimeSvc : public virtual IService {

   public:
      /// Virtual destructor, to make vtable happy
      virtual ~IONNXRuntimeSvc() = default;

      /// Declare an ID for this interface
      DeclareInterfaceID( AthONNX::IONNXRuntimeSvc, 1, 0 );

      /// Return the ONNX Runtime environment object
      virtual Ort::Env& env() const = 0;

   }; // class IONNXRuntimeSvc

} // namespace AthONNX

#endif // ATHEXONNXRUNTIME_IONNXRUNTIMESVC_H
