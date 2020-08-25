// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "KernelRunnerSvcImpl.h"

// AthCUDA include(s).
#include "AthCUDACore/Macros.cuh"
#include "AthCUDACore/StreamHolderHelpers.cuh"
#include "AthCUDACore/TaskArena.h"

// System include(s).
#include <cassert>
#include <iostream>
#include <memory>

namespace {

#ifdef __CUDACC__
   /// Type used internally to signal the finish of kernel tasks
   class KernelFinishData {
   public:
      /// Constructor
      KernelFinishData( const AthCUDA::KernelRunnerSvcImplCallback& callback,
                        std::unique_ptr< AthCUDA::IKernelTask > task )
      : m_callback( callback ), m_task( std::move( task ) ) {}

      /// Callback to the kernel runner service
      AthCUDA::KernelRunnerSvcImplCallback m_callback;
      /// The task that's being executed
      std::unique_ptr< AthCUDA::IKernelTask > m_task;
   }; // struct KernelFinishData

   /// Function set up to be called by CUDA when a task finishes
   void setKernelFinished( void* userData ) {

      // Cast the user data to the right type.
      KernelFinishData* data =
         reinterpret_cast< KernelFinishData* >( userData );
      assert( data != nullptr );

      // Check if there was an error in the execution, and finish
      // the task accordingly.
      const int result =
         data->m_task->finished( cudaGetLastError() == cudaSuccess ? 0 : 1,
                                 AthCUDA::IKernelTask::Asynchronous );
      if( result != 0 ) {
         std::cerr << "Failed to finish a CUDA kernel task!" << std::endl;
         std::abort();
      }

      // Tell the service that a task has finished executing.
      data->m_callback();

      // Delete the data object.
      delete data;
      return;
   }
#endif // __CUDACC__

   /// Functor scheduling an @c AthCUDA::IKernelTask for execution
   class KernelSchedulerTask {

   public:
      /// Constructor with all necessary parameters
      KernelSchedulerTask( const AthCUDA::KernelRunnerSvcImplCallback& callback,
                           std::unique_ptr< AthCUDA::IKernelTask > task,
                           AthCUDA::KernelRunnerSvcImpl& svcImpl )
      : m_callback( callback ), m_task( std::move( task ) ),
        m_svcImpl( &svcImpl ) {}

      /// Operator executing the functor
      void operator()() const {

         // Get a stream for the job.
         auto stream = m_svcImpl->getAvailableStream();
         assert( stream );

         // First off, let the task schedule all of its own operations.
         if( m_task->execute( stream ) != 0 ) {
            std::cerr << "Failed to schedule the launch of a GPU task"
                      << std::endl;
            // At this point there's not much that the code can do. So let's
            // just sit back, and see how the job will eventually crash...
         }

         // Now add a step after those to the stream, one that signals to
         // us that the task is done.
         CUDA_EXP_CHECK( cudaLaunchHostFunc( getStream( stream ),
                                             ::setKernelFinished,
                            new ::KernelFinishData( m_callback,
                                                    std::move( m_task ) ) ) );
      }

   private:
      /// Callback to the kernel runner service
      AthCUDA::KernelRunnerSvcImplCallback m_callback;
      /// The task that's being executed
      mutable std::unique_ptr< AthCUDA::IKernelTask > m_task;
      /// Pointer to the service implementation object
      AthCUDA::KernelRunnerSvcImpl* m_svcImpl;

   }; // class KernelSchedulerTask

} // private namespace

namespace AthCUDA {

   void KernelRunnerSvcImpl::execute( std::unique_ptr< IKernelTask > task ) {

      // Schedule a task that will take care of scheduling/launching the CUDA
      // kernel.
      taskArena().enqueue( ::KernelSchedulerTask( m_callback,
                                                  std::move( task ),
                                                  *this ) );

      // Return gracefully.
      return;
   }

} // namespace AthCUDA
