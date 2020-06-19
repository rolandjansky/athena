// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDASERVICES_KERNELRUNNERSVCIMPL_H
#define ATHCUDASERVICES_KERNELRUNNERSVCIMPL_H

// AthCUDA include(s).
#include "AthCUDAInterfaces/IKernelTask.h"
#include "AthCUDAInterfaces/StreamHolder.h"

// System include(s).
#include <memory>

namespace AthCUDA {

   // Forward declaration(s).
   class IStreamPoolSvc;
   class KernelRunnerSvc;

   /// Class used for calling @c AthCUDA::KernelRunnerSvc::setTaskFinished
   class KernelRunnerSvcImplCallback {

   public:
      /// Constructor with the service object to operate on
      KernelRunnerSvcImplCallback( KernelRunnerSvc& svc );

      /// Call @c setTaskFinished() on the service object
      KernelRunnerSvcImplCallback& operator()();

   private:
      /// Pointer to the main service
      KernelRunnerSvc* m_svc;

   }; // class KernelRunnerSvcImplCallback

   /// CUDA implementation of @c AthCUDA::KernelRunnerSvc
   class KernelRunnerSvcImpl {

   public:
      /// Constructor with all necessary parameters
      KernelRunnerSvcImpl( IStreamPoolSvc& streamPool,
                           KernelRunnerSvc& svc );

      /// Implementation of @c AthCUDA::IKernelRunnerSvc::execute
      void execute( std::unique_ptr< IKernelTask > task );

      /// Get an available CUDA stream from the pool
      StreamHolder getAvailableStream();

   private:
      /// The stream pool service in use
      IStreamPoolSvc* m_streamPool;

      /// Helper object to call @c setTaskFinished() with
      KernelRunnerSvcImplCallback m_callback;

   }; // class KernelRunnerSvcImpl

} // namespace AthCUDA

#endif // ATHCUDASERVICES_KERNELRUNNERSVCIMPL_H
