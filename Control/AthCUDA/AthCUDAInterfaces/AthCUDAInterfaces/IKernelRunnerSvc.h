// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAINTERFACES_IKERNELRUNNERSVC_H
#define ATHCUDAINTERFACES_IKERNELRUNNERSVC_H

// Local include(s).
#include "AthCUDAInterfaces/IKernelTask.h"

// Gaudi include(s).
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

// System include(s).
#include <memory>

namespace AthCUDA {

   /// Interface for executing @c AthCUDA::IKernelTask tasks
   ///
   /// The implementation of this service is supposed to be used for executing
   /// "GPU calculations" in a balanced way between the CPU and the GPU.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IKernelRunnerSvc : public virtual IService {

   public:
      /// Declare the interface ID
      DeclareInterfaceID( AthCUDA::IKernelRunnerSvc, 1, 0 );

      /// Execute a user specified kernel task
      ///
      /// If a GPU is available at runtime, and it is not doing other things
      /// at the moment, this function offloads the calculation to the GPU,
      /// and returns right away. The user code is expected to wait for the
      /// task to finish, in "some" way provided by the task object itself.
      ///
      /// If a GPU is not available for any reason, the function just executes
      /// the task on the CPU in the caller thread, and returns only once the
      /// task is finished.
      ///
      /// @param task The task to be executed on the CPU or GPU
      /// @return The usual @c StatusCode values
      ///
      virtual StatusCode execute( std::unique_ptr< IKernelTask > task ) = 0;

   }; // class IKernelRunnerSvc

} // namespace AthCUDA

#endif // ATHCUDAINTERFACES_IKERNELRUNNERSVC_H
