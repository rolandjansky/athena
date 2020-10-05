// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDACORE_MEMORY_CUH
#define ATHCUDACORE_MEMORY_CUH

// Local include(s).
#include "AthCUDACore/Macros.cuh"

// System include(s).
#include <memory>

namespace AthCUDA {

   /// Namespace for code the user should not be relying on directly
   namespace details {

      /// Class performing the deletion of a CUDA managed memory array
      class ManagedArrayDeleter {

      public:
         /// Operator performing the deletion of the memory
         void operator()( void* ptr );

      }; // class ManagedArrayDeleter

      /// Class performing the deletion of a CUDA device memory array
      class DeviceArrayDeleter {

      public:
         /// Operator performing the deletion of the memory
         void operator()( void* ptr );

      }; // class DeviceArrayDeleter

      /// Class performing the deletion of pinned host memory
      class HostArrayDeleter {

      public:
         /// Operator performing the deletion of the memory
         void operator()( void* ptr );

      }; // class HostArrayDeleter

   } // namespace details

   /// Convenience type for handling primitive variable arrays with CUDA
   template< typename T >
   using managed_array = std::unique_ptr< T, details::ManagedArrayDeleter >;

   /// Function creating a primitive array in CUDA managed memory
   template< typename T >
   managed_array< T > make_managed_array( std::size_t size );

   /// Convenience type for using primitive variable arrays on a CUDA device
   template< typename T >
   using device_array = std::unique_ptr< T, details::DeviceArrayDeleter >;

   /// Function creating a primitive array in CUDA device memory
   template< typename T >
   device_array< T > make_device_array( std::size_t size );

   /// Convenience type for using primitive pinned variable arrays on the host
   template< typename T >
   using host_array = std::unique_ptr< T, details::HostArrayDeleter >;

   /// Function creating a primitive array in the host's pinned memory
   template< typename T >
   host_array< T > make_host_array( std::size_t size );

   /// Replacement for @c std::vector in device code
   ///
   /// Since STL classes can't be used in CUDA device code, some helper
   /// code is needed to manage variable sized arrays in some convenient
   /// way.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   template< typename T >
   class array {

   public:
      /// Default constructor
      ATHCUDA_HOST_AND_DEVICE
      array();
      /// Constructor allocating some memory right away
      ATHCUDA_HOST_AND_DEVICE
      array( std::size_t size );
      /// Move constructor
      ATHCUDA_HOST_AND_DEVICE
      array( array< T >&& parent );
      /// Destructor
      ATHCUDA_HOST_AND_DEVICE
      ~array();

      /// Move operator
      ATHCUDA_HOST_AND_DEVICE
      array< T >& operator=( array< T >&& rhs );

      /// Get the size of the array
      ATHCUDA_HOST_AND_DEVICE
      std::size_t size() const;

      /// Get the pointer to the array held by the object (non-const)
      ATHCUDA_HOST_AND_DEVICE
      T* get();
      /// Get the pointer to the array held by the object (const)
      ATHCUDA_HOST_AND_DEVICE
      const T* get() const;

      /// Conversion operator to a bare pointer (non-const)
      ATHCUDA_HOST_AND_DEVICE
      operator T*() { return get(); }
      /// Conversion operator to a bare pointer (const)
      ATHCUDA_HOST_AND_DEVICE
      operator const T*() const { return get(); }

      /// Operator for accessing one element of the array (non-const)
      ATHCUDA_HOST_AND_DEVICE
      T& operator[]( std::size_t index );
      /// Operator for accessing one element of the array (const)
      ATHCUDA_HOST_AND_DEVICE
      const T& operator[]( std::size_t index ) const;

      /// Resize the managed array
      ATHCUDA_HOST_AND_DEVICE
      bool resize( std::size_t size );

   private:
      /// Pointer to the managed array
      T* m_ptr;
      /// Size of the managed array
      std::size_t m_size;

   }; // class array

} // namespace AthCUDA

// Include the implementation.
#include "AthCUDACore/Memory.icc"

#endif // ATHCUDACORE_MEMORY_CUH
