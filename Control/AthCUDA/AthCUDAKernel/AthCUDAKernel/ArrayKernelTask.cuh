// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAKERNEL_ARRAYKERNELTASK_CUH
#define ATHCUDAKERNEL_ARRAYKERNELTASK_CUH

// Local include(s).
#include "AthCUDAKernel/KernelStatus.h"
#include "AthCUDAKernel/ArrayKernelTaskImpl.cuh"

// AthCUDA include(s).
#include "AthCUDAInterfaces/IKernelTask.h"
#include "AthCUDACore/Info.h"
#include "AthCUDACore/Memory.cuh"
#include "AthCUDACore/StreamHolderHelpers.cuh"

// System include(s).
#include <cstring>
#include <memory>
#include <tuple>
#include <type_traits>

namespace AthCUDA {

   /// Kernel running on arrays of the same size
   ///
   /// Such a kernel can be used to run some calculation either on the CPU or
   /// on a GPU on one or more primitive arrays that all have the same size.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   template< class FUNCTOR, typename... ARGS >
   class ArrayKernelTask : public IKernelTask {

      // At least one argument has to be provided.
      static_assert( sizeof...( ARGS ) > 0,
                     "At least one functor argument must be provided" );

   public:
      /// Constructor to use in a blocking execution
      ArrayKernelTask( KernelStatus& status,
                       std::size_t arraySizes, ARGS... args );

      /// @name Function(s) inherited from @c AthCUDA::IKernelTask
      /// @{

      /// Execute the kernel using a specific stream
      virtual int execute( StreamHolder& stream ) override;

      /// Function called when an asynchronous execution finishes
      virtual int finished( int code, KernelExecMode mode ) override;

      /// @}

   private:
      /// The status object to notify about the task finishing
      KernelStatus* m_status;
      /// The size of the arrays being processed
      std::size_t m_arraySizes;
      /// The arguments received by the constructor
      std::tuple< ARGS... > m_args;
      /// The received variables, copied into pinned host memory
      typename ::ArrayKernelTaskHostVariables< ARGS... >::type m_hostObjs;
      /// The received variables, in device memory
      typename ::ArrayKernelTaskDeviceVariables< ARGS... >::type m_deviceObjs;
      /// The arguments received by the constructor, in device memory
      std::tuple< ARGS... > m_deviceArgs;
      /// Status flag showing that the kernel was run on a device
      bool m_ranOnDevice;

   }; // class ArrayKernelTask

   /// Helper function for creating an instance of @c AthCUDA::ArrayKernelTask
   template< class FUNCTOR, typename... ARGS >
   std::unique_ptr< ArrayKernelTask< FUNCTOR, ARGS... > >
   make_ArrayKernelTask( KernelStatus& status,
                         std::size_t arraySizes, ARGS... args );

   /// @name Implementation of the functions defined in this header
   /// @{

   template< class FUNCTOR, typename... ARGS >
   ArrayKernelTask< FUNCTOR, ARGS... >::
   ArrayKernelTask( KernelStatus& status, std::size_t arraySizes,
                    ARGS... args )
   : m_status( &status ), m_arraySizes( arraySizes ),
     m_args( args... ), m_hostObjs(), m_deviceObjs(), m_deviceArgs(),
     m_ranOnDevice( false ) {

   }

   template< class FUNCTOR, typename... ARGS >
   int ArrayKernelTask< FUNCTOR, ARGS... >::
   execute( StreamHolder& stream ) {

      // Extract the stream to use.
      cudaStream_t cudaStream = getStream( stream );

      // Only bother with host and device variables if the calculation is
      // actually being sent to a device...
      if( cudaStream ) {
         // Set up the host smart variable(s).
         m_hostObjs = ::MakeHostTuple< sizeof...( ARGS ) - 1,
                                       ARGS... >()( m_arraySizes, m_args );
         // Set up the device smart variable(s).
         m_deviceObjs = ::MakeDeviceTuple< sizeof...( ARGS ) - 1,
                                           ARGS... >()( m_arraySizes );

         // Copy data from the host to the device.
         const int result =
            ::CopyHostToDevice< sizeof...( ARGS ) - 1,
                                ARGS... >()( cudaStream, m_arraySizes,
                                             m_hostObjs, m_deviceObjs );
         AKT_CHECK( result );

         // Set up the raw device variable(s).
         m_deviceArgs = ::MakeDeviceArgs< sizeof...( ARGS ) - 1,
                                          ARGS... >()( m_deviceObjs );
      }

      // Schedule the execution of the code.
      if( cudaStream ) {
         AKT_CHECK( ::ExecuteImpl< FUNCTOR >::execute( cudaStream,
                       m_arraySizes, ::tupleTail( m_deviceArgs ),
                       std::get< 0 >( m_deviceArgs ) ) );
         m_ranOnDevice = true;
      } else {
         AKT_CHECK( ::ExecuteImpl< FUNCTOR >::execute(
                       m_arraySizes, ::tupleTail( m_args ),
                       std::get< 0 >( m_args ) ) );
      }

      // If the calculation was offloaded, set up the copy of the output
      // back to the host.
      if( cudaStream ) {
         const int result =
            ::CopyDeviceToHost< sizeof...( ARGS ) - 1,
                                ARGS... >()( cudaStream, m_arraySizes,
                                             m_deviceObjs, m_hostObjs );
         AKT_CHECK( result );
      }

      // Return gracefully.
      return 0;
   }

   template< class FUNCTOR, typename... ARGS >
   int ArrayKernelTask< FUNCTOR, ARGS... >::
   finished( int code, KernelExecMode ) {

      // Copy the array(s) back to non-pinned host memory if needed.
      if( m_ranOnDevice ) {
         const int result =
            ::CopyHostToHost< sizeof...( ARGS ) - 1,
                              ARGS... >()( m_arraySizes, m_hostObjs, m_args );
         AKT_CHECK( result );
         m_ranOnDevice = false;
      }
      // Notify the status object that the task is finished.
      if( m_status ) {
         m_status->finished( code );
      }
      // Return gracefully.
      return 0;
   }

   template< class FUNCTOR, typename... ARGS >
   std::unique_ptr< ArrayKernelTask< FUNCTOR, ARGS... > >
   make_ArrayKernelTask( KernelStatus& status,
                         std::size_t arraySizes, ARGS... args ) {
      return
         std::make_unique< ArrayKernelTask< FUNCTOR, ARGS... > >(
            status, arraySizes, args... );
   }

   /// @}

} // namespace AthCUDA

#endif // ATHCUDAKERNEL_ARRAYKERNELTASK_CUH
