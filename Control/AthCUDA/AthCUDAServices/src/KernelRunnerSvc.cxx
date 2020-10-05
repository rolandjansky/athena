//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "KernelRunnerSvc.h"

// Project include(s).
#include "AthCUDACore/Info.h"

// System include(s).
#include <cassert>
#include <sstream>
#include <thread>

namespace AthCUDA {

   StatusCode KernelRunnerSvc::initialize() {

      // Reset the internal counter(s).
      m_kernelsInFlight = 0;
      m_totalTasks = 0;
      m_gpuTasks = 0;

      // If no devices are available or no kernels are allowed to run on the
      // GPU, then don't even set up the implementation object.
      if( ( Info::instance().nDevices() == 0 ) ||
          ( m_nKernels.value() == 0 ) ) {
         ATH_MSG_INFO( "Will run everything on the CPU." );
         return StatusCode::SUCCESS;
      }

      // Access the stream pool service.
      ATH_CHECK( m_streamPoolSvc.retrieve() );

      // Create the implementation object.
      m_impl = std::make_unique< KernelRunnerSvcImpl >( *m_streamPoolSvc,
                                                        *this );

      // Tell the user what happened.
      std::ostringstream str;
      str << Info::instance();
      ATH_MSG_INFO( "Started service for running " << m_nKernels.value()
                    << " GPU kernel(s) in parallel on device(s):\n"
                    << str.str() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode KernelRunnerSvc::finalize() {

      // Destroy the implementation object.
      m_impl.reset();

      // Tell the user what happened.
      ATH_MSG_INFO( " o All task(s) executed: " << m_totalTasks.load() );
      const double percentage =
         ( m_totalTasks != 0 ?
           ( static_cast< double >( m_gpuTasks.load() ) /
             static_cast< double >( m_totalTasks.load() ) * 100.0 ) : 0.0 );
      ATH_MSG_INFO( " o GPU task(s) executed: " << m_gpuTasks.load() << " ("
                    << percentage << "%)" );

      // Finalise the base class.
      ATH_CHECK( Service::finalize() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode KernelRunnerSvc::execute( std::unique_ptr< IKernelTask > task ) {

      // Make sure that we received a valid task.
      if( task.get() == nullptr ) {
         ATH_MSG_ERROR( "Invalid task object received" );
         return StatusCode::FAILURE;
      }

      // One way or another, we will execute this task.
      ++m_totalTasks;

      // Check if a GPU is available, and no other thread is launching a GPU
      // calculation right now.
      if( ( ! m_impl ) || m_streamPoolSvc->isEmpty() ||
          ( ( m_nKernels.value() > 0 ) &&
            ( m_kernelsInFlight.load() >= m_nKernels.value() ) ) ) {

         // If so, let's just execute the task in the current thread.
         ATH_MSG_VERBOSE( "Executing a task on the CPU" );
         StreamHolder dummy;
         if( task->finished( task->execute( dummy ),
                             IKernelTask::Synchronous ) != 0 ) {
            ATH_MSG_ERROR( "Failed to execute task in the caller thread!" );
            return StatusCode::FAILURE;
         }

         // Return gracefully.
         return StatusCode::SUCCESS;
      }

      // If we got here, we need to schedule the task for execution on the/a
      // GPU.

      // Give the task to the implementation object to launch it.
      ATH_MSG_VERBOSE( "Executing an offloaded task" );
      ++m_kernelsInFlight;
      ++m_gpuTasks;
      m_impl->execute( std::move( task ) );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   void KernelRunnerSvc::setTaskFinished() {

      // Update the internal counter.
      --m_kernelsInFlight;
      return;
   }

} // namespace AthCUDA
