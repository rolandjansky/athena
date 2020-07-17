// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDACore/Memory.cuh"
#include "AthCUDACore/Info.h"
#include "AthCUDACore/Macros.cuh"
#include "AthCUDACore/TaskArena.h"

// TBB include(s).
#include <tbb/concurrent_queue.h>

// System include(s).
#include <cstdlib>
#include <stdexcept>
#include <thread>

namespace {

   /// Task used to delete pinned memory from the host
   class HostDeleterTask {

   public:
      /// Constructor with the memory pointer to delete
      HostDeleterTask( void* ptr ) : m_ptr( ptr ) {}

      /// Operator executing the functor
      void operator()() const {

         // Delete the memory.
         CUDA_EXP_CHECK( cudaFreeHost( m_ptr ) );
         return;
      }

   private:
      /// Pointer to delete (eventually)
      void* m_ptr;

   }; // class HostDeleterTask

   /// Task used to delete device memory
   class DeviceDeleterTask {

   public:
      /// Constructor with the memory pointer to delete
      DeviceDeleterTask( void* ptr ) : m_ptr( ptr ) {}

      /// Operator executing the functor
      void operator()() const {

         // Delete the memory.
         CUDA_EXP_CHECK( cudaFree( m_ptr ) );
         return;
      }

   private:
      /// Pointer to delete (eventually)
      void* m_ptr;

   }; // class DeviceDeleterTask

} // private namespace

namespace AthCUDA {

   namespace details {

      void ManagedArrayDeleter::operator()( void* ptr ) {

         // Don't do anything for a null pointer.
         if( ptr == nullptr ) {
            return;
         }

         // If a device is available, then free up the memory using CUDA.
         if( Info::instance().nDevices() != 0 ) {
            taskArena().enqueue( ::DeviceDeleterTask( ptr ) );
            return;
         }

         // If not, then the memory was simply allocated with malloc...
         ::free( ptr );
         return;
      }

      void DeviceArrayDeleter::operator()( void* ptr ) {

         // Don't do anything for a null pointer.
         if( ptr == nullptr ) {
            return;
         }

         // If a device is available, then free up the memory using CUDA.
         if( Info::instance().nDevices() != 0 ) {
            taskArena().enqueue( ::DeviceDeleterTask( ptr ) );
            return;
         }

         // If not, then the memory was simply allocated with malloc...
         ::free( ptr );
         return;
      }

      void HostArrayDeleter::operator()( void* ptr ) {

         // Don't do anything for a null pointer.
         if( ptr == nullptr ) {
            return;
         }

         // If a device is available, then free up the memory using CUDA.
         if( Info::instance().nDevices() != 0 ) {
            taskArena().enqueue( ::HostDeleterTask( ptr ) );
            return;
         }

         // If not, then the memory was simply allocated with malloc...
         ::free( ptr );
         return;
      }

      /// Use CUDA managed memory if CUDA is available during the build and
      /// a CUDA device is available during runtime. Otherwise do the deed
      /// simply with standard C memory allocation.
      ///
      /// @param size The size of the array to create
      /// @return A pointer to the allocated array
      ///
      template< typename T >
      T* managedMallocHelper( std::size_t size ) {

         // For a zero sized array return a null pointer.
         if( size == 0 ) {
            return nullptr;
         }

         // The result pointer.
         T* result = 0;

         // Try to allocate the array in CUDA managed memory first.
         if( Info::instance().nDevices() != 0 ) {
            CUDA_EXP_CHECK( cudaMallocManaged( &result, size * sizeof( T ) ) );
            return result;
         }

         // If that didn't work, fall back on simple malloc.
         result = static_cast< T* >( ::malloc( size * sizeof( T ) ) );
         return result;
      }

      /// Use CUDA device memory if CUDA is available during the build and
      /// a CUDA device is available during runtime. Otherwise do the deed
      /// simply with standard C memory allocation.
      ///
      /// @param size The size of the array to create
      /// @return A pointer to the allocated array
      ///
      template< typename T >
      T* deviceMallocHelper( std::size_t size ) {

         // For a zero sized array return a null pointer.
         if( size == 0 ) {
            return nullptr;
         }

         // The result pointer.
         T* result = 0;

         // Try to allocate the array in CUDA managed memory first.
         if( Info::instance().nDevices() != 0 ) {
            CUDA_EXP_CHECK( cudaMalloc( &result, size * sizeof( T ) ) );
            return result;
         }

         // If that didn't work, fall back on simple malloc.
         result = static_cast< T* >( ::malloc( size * sizeof( T ) ) );
         return result;
      }

      /// Use CUDA to allocate page-locked memory on the host if CUDA is
      /// available, otherwise just allocate plain old memory.
      ///
      /// @param size The size of the array to create
      /// @return A pointer to the allocated array
      ///
      template< typename T >
      T* hostMallocHelper( std::size_t size ) {

         // For a zero sized array return a null pointer.
         if( size == 0 ) {
            return nullptr;
         }

         // The result pointer.
         T* result = 0;

         // Try to allocate the array in CUDA managed memory first.
         if( Info::instance().nDevices() != 0 ) {
            CUDA_EXP_CHECK( cudaHostAlloc( &result, size * sizeof( T ),
                                           cudaHostAllocDefault |
                                           cudaHostAllocWriteCombined ) );
            return result;
         }

         // If that didn't work, fall back on simple malloc.
         result = static_cast< T* >( ::malloc( size * sizeof( T ) ) );
         return result;
      }

   } // namespace details

} // namespace AthCUDA

/// Helper macro for instantiating the allocator functions for different types
#define INST_MALLOC( TYPE )                                                    \
   template TYPE* AthCUDA::details::managedMallocHelper< TYPE >( std::size_t );\
   template TYPE* AthCUDA::details::hostMallocHelper< TYPE >( std::size_t );   \
   template TYPE* AthCUDA::details::deviceMallocHelper< TYPE >( std::size_t )

// Instantiate the array allocators for all "reasonable" primitive types.
INST_MALLOC( void* );
INST_MALLOC( char );
INST_MALLOC( unsigned char );
INST_MALLOC( short );
INST_MALLOC( unsigned short );
INST_MALLOC( int );
INST_MALLOC( unsigned int );
INST_MALLOC( long );
INST_MALLOC( unsigned long );
INST_MALLOC( long long );
INST_MALLOC( unsigned long long );
INST_MALLOC( float );
INST_MALLOC( double );

// Clean up.
#undef INST_MALLOC
