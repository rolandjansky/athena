// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDASERVICES_KERNELRUNNERSVC_H
#define ATHCUDASERVICES_KERNELRUNNERSVC_H

// Local include(s).
#include "KernelRunnerSvcImpl.h"

// AthCUDA include(s).
#include "AthCUDAInterfaces/IKernelRunnerSvc.h"
#include "AthCUDAInterfaces/IStreamPoolSvc.h"

// Framework include(s).
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthService.h"

// System include(s).
#include <atomic>
#include <memory>

namespace AthCUDA {

   /// Service used for executing @c AthCUDA::IKernelTask tasks
   ///
   /// It allows the user to execute a configured number of tasks in parallel
   /// on a GPU, while "overflow" tasks are executed on the CPU instead.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class KernelRunnerSvc : public extends< AthService, IKernelRunnerSvc > {

   public:
      /// Inherit the base class's constructor(s)
      using extends::extends;

      /// @name Interface inherited from @c IService
      /// @{

      /// Initialise the service
      virtual StatusCode initialize() override;

      /// Finalise the service
      virtual StatusCode finalize() override;

      /// @}

      /// @name Interface inherited from @c AthCUDA::IKernelRunnerSvc
      /// @{

      /// Execute a user specified kernel task
      ///
      /// If a GPU is available at runtime, and it is not doing other things
      /// at the moment, this function offloads the calculation to the GPU,
      /// and returns right away. The user is expected to return control in the
      /// calling thread to the framework, as the kernel task will notify the
      /// framework when the task gets finished.
      ///
      /// If a GPU is not available for any reason, the function just executes
      /// the task on the CPU in the caller thread, and returns only once the
      /// task is finished.
      ///
      /// @param task The task to be executed on the CPU or GPU
      /// @return A code describing what happened to the task
      ///
      virtual StatusCode
      execute( std::unique_ptr< IKernelTask > task ) override;

      /// @}

      // Function telling the service that a task has finished its execution
      void setTaskFinished();

   private:
      /// @name Service properties
      /// @{

      /// The number of streams to use
      Gaudi::Property< int > m_nKernels{ this, "NParallelKernels", 2,
         "The number of CUDA kernels to execute in parallel" };

      /// Service managing CUDA the streams
      ServiceHandle< IStreamPoolSvc > m_streamPoolSvc{ this, "StreamPoolSvc",
         "AthCUDA::StreamPoolSvc",
         "The AthCUDA::StreamPoolSvc instance to use" };

      /// @}

      /// The current number of kernels being executed
      std::atomic_int m_kernelsInFlight;

      /// The number of tasks executed during the job in total
      std::atomic_uint m_totalTasks;
      /// The number of tasks sent to the GPU during the job
      std::atomic_uint m_gpuTasks;

      /// Implementation helper object
      std::unique_ptr< KernelRunnerSvcImpl > m_impl;

   }; // class KernelRunnerSvc

} // namespace AthCUDA

#endif // ATHCUDASERVICES_KERNELRUNNERSVC_H
