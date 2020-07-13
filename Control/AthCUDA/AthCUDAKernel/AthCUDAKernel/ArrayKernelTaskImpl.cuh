// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDAKERNEL_ARRAYKERNELTASKIMPL_CUH
#define ATHCUDAKERNEL_ARRAYKERNELTASKIMPL_CUH

// AthCUDA include(s).
#include "AthCUDACore/Info.h"
#include "AthCUDACore/Macros.cuh"
#include "AthCUDACore/Memory.cuh"

// System include(s).
#include <cassert>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

/// Helper macro for status code checks inside of these functions
#define AKT_CHECK( EXP )                                                 \
   do {                                                                  \
      const int exp_result = EXP;                                        \
      if( exp_result != 0 ) {                                            \
         std::cerr << __FILE__ << ":" << __LINE__                        \
                   << " Failed to execute: " << #EXP << std::endl;       \
         return exp_result;                                              \
      }                                                                  \
   } while( false )

namespace {

   /// @name Code implementing @c ArrayKernelTaskHostVariables
   /// @{

   /// Type to use for primtive variables
   template< typename T >
   struct HostVariableTypeImpl {
      // Sanity check(s).
      static_assert( std::is_trivial< T >::value == true,
                     "Non-array variables must be trivial" );
      /// The type to use
      typedef T type;
   }; // struct HostVariableTypeImpl

   /// Type to use for arrays / pointers
   template< typename T >
   struct HostVariableTypeImpl< T* > {
      /// Sanity check(s).
      static_assert( std::is_trivial< T >::value == true,
                     "Arrays must be made of trivial types" );
      /// The type to use
      typedef typename AthCUDA::host_array< T > type;
   }; // struct HostVariableTypeImpl< T* >

   /// Type of the tuple holding the pinned host variables in
   /// @c AthCUDA::ArrayKernelTask
   template< typename... ARGS >
   struct ArrayKernelTaskHostVariables {
      /// The type to use
      typedef typename
      std::tuple< typename ::HostVariableTypeImpl< ARGS >::type... > type;
   }; // struct ArrayKernelTaskHostVariables

   /// @}

   /// @name Code implementing @c ArrayKernelTaskDeviceVariables
   /// @{

   /// Type to use for primtive variables
   template< typename T >
   struct DeviceVariableTypeImpl {
      // Sanity check(s).
      static_assert( std::is_trivial< T >::value == true,
                     "Non-array variables must be trivial" );
      /// The type to use
      typedef T type;
   }; // struct DeviceVariableTypeImpl

   /// Type to use for arrays / pointers
   template< typename T >
   struct DeviceVariableTypeImpl< T* > {
      /// Sanity check(s).
      static_assert( std::is_trivial< T >::value == true,
                     "Arrays must be made of trivial types" );
      /// The type to use
      typedef typename AthCUDA::device_array< T > type;
   }; // struct DeviceVariableTypeImpl< T* >

   /// Type of the tuple holding the device variables in
   /// @c AthCUDA::ArrayKernelTask
   template< typename... ARGS >
   struct ArrayKernelTaskDeviceVariables {
      /// The type to use
      typedef typename
      std::tuple< typename ::DeviceVariableTypeImpl< ARGS >::type... > type;
   }; // struct ArrayKernelTaskDeviceVariables

   /// @}

   /// @name Code used to create the pinned variables on the host
   /// @{

   /// Model for the functors creating the host variables
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class MakeHostObjectImpl;

   /// Functor creating a host array for an array received by the task
   template< std::size_t Index, typename... ARGS >
   class MakeHostObjectImpl< true, Index, ARGS... > {
   public:
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         ArrayVariable_t;
      /// Variable type, without the pointer notation
      typedef typename std::remove_pointer< ArrayVariable_t >::type Variable_t;
      /// Operator creating a host array object.
      AthCUDA::host_array< Variable_t >
      operator()( std::size_t arraySizes, std::tuple< ARGS... >& args ) const {
         // Create the host array object.
         auto result = AthCUDA::make_host_array< Variable_t >( arraySizes );
         // Copy the contents of the received array to the host array.
         std::memcpy( result.get(), std::get< Index >( args ),
                      arraySizes * sizeof( Variable_t ) );
         // Return the smart pointer.
         return result;
      }
   }; // class MakeHostObjectImpl

   /// Functor making a trivial copy of a simple variable received by the task
   template< std::size_t Index, typename... ARGS >
   class MakeHostObjectImpl< false, Index, ARGS... > {
   public:
      /// Type of the variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      // Sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Received a non-trivial type" );
      /// Operator returning the same primitive variable as-is
      Variable_t operator()( std::size_t /*arraySizes*/,
                             std::tuple< ARGS... >& args ) const {
         // Get the primitive variable out of the tuple.
         return std::get< Index >( args );
      }
   }; // class MakeHostObjectImpl

   /// Functor preparing a tuple with the "host variables"
   template< std::size_t Index, typename... ARGS >
   class MakeHostTuple {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      /// Operator creating the host objects recursively
      auto operator()( std::size_t arraySizes,
                       std::tuple< ARGS... >& args ) const {
         // Create all the previous elements of the tuple.
         auto prev = MakeHostTuple< Index - 1, ARGS... >()( arraySizes, args );
         // Create the current element of the tuple.
         auto current =
            std::make_tuple(
               MakeHostObjectImpl< std::is_pointer< Variable_t >::value, Index,
                                   ARGS... >()( arraySizes, args ) );
         // Return their concatenation.
         return std::tuple_cat( std::move( prev ), std::move( current ) );
      }
   }; // class MakeHostTuple

   /// Functor creating the first element of the "host variable" tuple
   template< typename... ARGS >
   class MakeHostTuple< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
      /// Operator creating the first host object
      auto operator()( std::size_t arraySizes,
                       std::tuple< ARGS... >& args ) const {
         // Return the first element inside of a tuple.
         return
            std::make_tuple(
               MakeHostObjectImpl< std::is_pointer< Variable_t >::value, 0,
                                   ARGS... >()( arraySizes, args ) );
      }
   }; // class MakeHostTuple

   /// @}

   /// @name Code used to create the device variables
   /// @{

   /// Model for the functors creating the host variables
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class MakeDeviceObjectImpl;

   /// Functor creating a device array for an array received by the task
   template< std::size_t Index, typename... ARGS >
   class MakeDeviceObjectImpl< true, Index, ARGS... > {
   public:
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         ArrayVariable_t;
      /// Variable type, without the pointer notation
      typedef typename std::remove_pointer< ArrayVariable_t >::type Variable_t;
      /// Operator creating a device array object.
      AthCUDA::device_array< Variable_t >
      operator()( std::size_t arraySizes ) const {
         // Create the device array object.
         return AthCUDA::make_device_array< Variable_t >( arraySizes );
      }
   }; // class MakeDeviceObjectImpl

   /// Functor making a trivial variable to pass to the device function
   template< std::size_t Index, typename... ARGS >
   class MakeDeviceObjectImpl< false, Index, ARGS... > {
   public:
      /// Type of the variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      // Sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Received a non-trivial type" );
      /// Operator returning the same primitive variable as-is
      Variable_t operator()( std::size_t /*arraySizes*/ ) const {
         // Return the default value of the trivial variable.
         return Variable_t();
      }
   }; // class MakeDeviceObjectImpl

   /// Functor preparing a tuple with the "device variables"
   template< std::size_t Index, typename... ARGS >
   class MakeDeviceTuple {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      /// Operator creating the device objects recursively
      auto operator()( std::size_t arraySizes ) const {
         // Create all the previous elements of the tuple.
         auto prev = MakeDeviceTuple< Index - 1, ARGS... >()( arraySizes );
         // Create the current element of the tuple.
         auto current =
            std::make_tuple(
               MakeDeviceObjectImpl< std::is_pointer< Variable_t >::value,
                                     Index, ARGS... >()( arraySizes ) );
         // Return their concatenation.
         return std::tuple_cat( std::move( prev ), std::move( current ) );
      }
   }; // class MakeDeviceTuple

   /// Functor creating the first element of the "device variable" tuple
   template< typename... ARGS >
   class MakeDeviceTuple< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
      /// Operator creating the first host object
      auto operator()( std::size_t arraySizes ) const {
         // Return the first element inside of a tuple.
         return
            std::make_tuple(
               MakeDeviceObjectImpl< std::is_pointer< Variable_t >::value, 0,
                                     ARGS... >()( arraySizes ) );
      }
   }; // class MakeDeviceTuple

   /// @}

   /// @name Code used to create the "raw" device arguments
   /// @{

   /// Model for the functors creating the device raw arguments
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class MakeDeviceArgsImpl;

   /// Functor creating a device raw argument
   template< std::size_t Index, typename... ARGS >
   class MakeDeviceArgsImpl< true, Index, ARGS... > {
   public:
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Argument_t;
      /// Type of the device smart pointer tuple.
      typedef typename ::ArrayKernelTaskDeviceVariables< ARGS... >::type
         DeviceTuple_t;
      /// Operator exctracting the device array variable.
      Argument_t operator()( DeviceTuple_t& tuple ) const {
         return std::get< Index >( tuple ).get();
      }
   }; // class MakeDeviceArgsImpl

   /// Functor creating a device raw argument
   template< std::size_t Index, typename... ARGS >
   class MakeDeviceArgsImpl< false, Index, ARGS... > {
   public:
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Argument_t;
      /// Type of the device smart pointer tuple.
      typedef typename ::ArrayKernelTaskDeviceVariables< ARGS... >::type
         DeviceTuple_t;
      /// Operator exctracting the device array variable.
      Argument_t operator()( DeviceTuple_t& tuple ) const {
         return std::get< Index >( tuple );
      }
   }; // class MakeDeviceArgsImpl

   template< std::size_t Index, typename... ARGS >
   class MakeDeviceArgs {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      /// Type of the device smart pointer tuple.
      typedef typename ::ArrayKernelTaskDeviceVariables< ARGS... >::type
         DeviceTuple_t;
      /// Operator creating the device raw variables recursively
      auto operator()( DeviceTuple_t& tuple ) const {
         // Create all the previous elements of the tuple.
         auto prev = MakeDeviceArgs< Index - 1, ARGS... >()( tuple );
         // Create the current element of the tuple.
         auto current =
            std::make_tuple(
               MakeDeviceArgsImpl< std::is_pointer< Variable_t >::value, Index,
                                   ARGS... >()( tuple ) );
         // Return their concatenation.
         return std::tuple_cat( std::move( prev ), std::move( current ) );
      }
   }; // class MakeDeviceArgs

   template< typename... ARGS >
   class MakeDeviceArgs< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
   public:
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
      /// Type of the device smart pointer tuple.
      typedef typename ::ArrayKernelTaskDeviceVariables< ARGS... >::type
         DeviceTuple_t;
      /// Operator creating the device raw variables recursively
      auto operator()( DeviceTuple_t& tuple ) const {
         // Return the first element of the tuple.
         return
            std::make_tuple(
               MakeDeviceArgsImpl< std::is_pointer< Variable_t >::value, 0,
                                   ARGS... >()( tuple ) );
      }
   }; // class MakeDeviceArgs

   /// @}

   /// @name Code used to set up copying data between the host to the device
   /// @{

   /// Model for the functors performing the H->D copies
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class CopyHostToDeviceImpl;

   /// Functor setting up the asynchronous H->D copy of one array
   template< std::size_t Index, typename... ARGS >
   class CopyHostToDeviceImpl< true, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         ArrayVariable_t;
      /// Variable type, without the pointer notation
      typedef typename std::remove_pointer< ArrayVariable_t >::type Variable_t;
      // Some more sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Only trivial arrays are supported" );
   public:
      /// Operator scheduling the host->device copy of one array
      int operator()( cudaStream_t
#ifdef __CUDACC__
                         stream
#endif // __CUDACC__
                      , std::size_t
#ifdef __CUDACC__
                         arraySizes
#endif // __CUDACC__
                      , typename ArrayKernelTaskHostVariables< ARGS... >::type&
#ifdef __CUDACC__
                         hostArgs
#endif // __CUDACC__
                      , typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
#ifdef __CUDACC__
                         deviceArgs
#endif // __CUDACC__
                      ) {
         // Schedule the H->D copy.
         CUDA_EXP_CHECK( cudaMemcpyAsync( std::get< Index >( deviceArgs ).get(),
                                          std::get< Index >( hostArgs ).get(),
                                          arraySizes * sizeof( Variable_t ),
                                          cudaMemcpyHostToDevice,
                                          stream ) );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToDeviceImpl

   /// Functor performing the H->D copy of a trivial variable
   template< std::size_t Index, typename... ARGS >
   class CopyHostToDeviceImpl< false, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
      // Some more sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Only trivial variables are supported" );
   public:
      /// Operator scheduling the host->device copy of one array
      int operator()( cudaStream_t /*stream*/, std::size_t /*arraySizes*/,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostArgs,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         deviceArgs ) {
         // Perform a trivial copy between the variables.
         std::get< Index >( deviceArgs ) = std::get< Index >( hostArgs );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToDeviceImpl

   /// Functor used to set up the H->D copy of every user function parameter
   template< std::size_t Index, typename... ARGS >
   class CopyHostToDevice {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function scheduling the copy from the host to the device
      int operator()( cudaStream_t stream, std::size_t arraySizes,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostArgs,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         deviceArgs ) {
         // Perform the copy for all the rest of the tuples.
         int result =
            CopyHostToDevice< Index - 1, ARGS... >()( stream, arraySizes,
                                                      hostArgs, deviceArgs );
         AKT_CHECK( result );
         // Set up the copy for this specific index.
         result = CopyHostToDeviceImpl< std::is_pointer< Variable_t >::value,
                                        Index, ARGS... >()( stream, arraySizes,
                                                            hostArgs,
                                                            deviceArgs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToDevice

   /// Functor used to set up the H->D copy of the first function parameter
   template< typename... ARGS >
   class CopyHostToDevice< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
      /// Type of the first variable in the tuple.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function scheduling the copy from the host to the device
      int operator()( cudaStream_t stream, std::size_t arraySizes,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostArgs,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         deviceArgs ) {
         // Set up the copy for this first index.
         const int result =
            CopyHostToDeviceImpl< std::is_pointer< Variable_t >::value, 0,
                                  ARGS... >()( stream, arraySizes, hostArgs,
                                               deviceArgs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToDevice

   /// Model for the functors performing the D->H copies
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class CopyDeviceToHostImpl;

   /// Functor setting up the asynchronous D->H copy of one array
   template< std::size_t Index, typename... ARGS >
   class CopyDeviceToHostImpl< true, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         ArrayVariable_t;
      /// Variable type, without the pointer notation
      typedef typename std::remove_pointer< ArrayVariable_t >::type Variable_t;
      // Some more sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Only trivial arrays are supported" );
   public:
      /// Operator scheduling the device->host copy of one array
      int operator()( cudaStream_t
#ifdef __CUDACC__
                         stream
#endif // __CUDACC__
                      , std::size_t
#ifdef __CUDACC__
                         arraySizes
#endif // __CUDACC__
                      , typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
#ifdef __CUDACC__
                         deviceObjs
#endif // __CUDACC__
                      , typename ArrayKernelTaskHostVariables< ARGS... >::type&
#ifdef __CUDACC__
                         hostObjs
#endif // __CUDACC__
                      ) {
         // Schedule the D->H copy.
         CUDA_EXP_CHECK( cudaMemcpyAsync( std::get< Index >( hostObjs ).get(),
                                          std::get< Index >( deviceObjs ).get(),
                                          arraySizes * sizeof( Variable_t ),
                                          cudaMemcpyDeviceToHost,
                                          stream ) );
         // Return gracefully.
         return 0;
      }
   }; // class CopyDeviceToHostImpl

   /// Functor performing the D->H copy of a trivial variable
   template< std::size_t Index, typename... ARGS >
   class CopyDeviceToHostImpl< false, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
   public:
      /// A no-op operator
      int operator()( cudaStream_t /*stream*/, std::size_t /*arraySizes*/,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         /*deviceObjs*/,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         /*hostObjs*/ ) {
         // Return gracefully.
         return 0;
      }
   }; // class CopyDeviceToHostImpl

   /// Functor used to set up the D->H copy of every user function parameter
   template< std::size_t Index, typename... ARGS >
   class CopyDeviceToHost {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function scheduling the copy from the device to the host
      int operator()( cudaStream_t stream, std::size_t arraySizes,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         deviceObjs,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostObjs ) {
         // Perform the copy for all the rest of the tuples.
         int result =
            CopyDeviceToHost< Index - 1, ARGS... >()( stream, arraySizes,
                                                      deviceObjs, hostObjs );
         AKT_CHECK( result );
         // Set up the copy for this specific index.
         result = CopyDeviceToHostImpl< std::is_pointer< Variable_t >::value,
                                        Index, ARGS... >()( stream, arraySizes,
                                                            deviceObjs,
                                                            hostObjs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyDeviceToHost

   /// Functor used to set up the D->H copy of the first function parameter
   template< typename... ARGS >
   class CopyDeviceToHost< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
      /// Type of the first variable in the tuple.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function scheduling the copy from the device to the host
      int operator()( cudaStream_t stream, std::size_t arraySizes,
                      typename ArrayKernelTaskDeviceVariables< ARGS... >::type&
                         deviceObjs,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostObjs ) {
         // Set up the copy for this first index.
         const int result =
            CopyDeviceToHostImpl< std::is_pointer< Variable_t >::value, 0,
                                  ARGS... >()( stream, arraySizes, deviceObjs,
                                               hostObjs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyDeviceToHost

   /// Model for the functors performing the H->H copies
   template< bool IsArray, std::size_t Index, typename... ARGS >
   class CopyHostToHostImpl;

   /// Functor setting up the asynchronous H->H copy of one array
   template< std::size_t Index, typename... ARGS >
   class CopyHostToHostImpl< true, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the array variable at this index
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         ArrayVariable_t;
      /// Variable type, without the pointer notation
      typedef typename std::remove_pointer< ArrayVariable_t >::type Variable_t;
      // Some more sanity check(s).
      static_assert( std::is_trivial< Variable_t >::value == true,
                     "Only trivial arrays are supported" );
   public:
      /// Operator performing the host->host copy of one array
      int operator()( std::size_t arraySizes,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostObjs,
                      std::tuple< ARGS... >& hostArgs ) {
         // Perform the H->H copy.
         std::memcpy( std::get< Index >( hostArgs ),
                      std::get< Index >( hostObjs ).get(),
                      arraySizes * sizeof( Variable_t ) );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToHostImpl

   /// Functor performing the H->H copy of a trivial variable
   template< std::size_t Index, typename... ARGS >
   class CopyHostToHostImpl< false, Index, ARGS... > {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
   public:
      /// A no-op operator
      int operator()( std::size_t /*arraySizes*/,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         /*hostObjs*/,
                      std::tuple< ARGS... >& /*hostArgs*/ ) {
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToHostImpl

   /// Functor used to perform the H->H copy of every user function parameter
   template< std::size_t Index, typename... ARGS >
   class CopyHostToHost {
      // Some sanity check(s).
      static_assert( Index < sizeof...( ARGS ), "Invalid index received" );
      /// Type of the variable under the current index.
      typedef typename std::tuple_element< Index, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function performing the host->host copy
      int operator()( std::size_t arraySizes,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostObjs,
                      std::tuple< ARGS... >& hostArgs ) {
         // Perform the copy for all the rest of the tuple.
         int result =
            CopyHostToHost< Index - 1, ARGS... >()( arraySizes, hostObjs,
                                                    hostArgs );
         AKT_CHECK( result );
         // Set up the copy for this specific index.
         result = CopyHostToHostImpl< std::is_pointer< Variable_t >::value,
                                      Index, ARGS... >()( arraySizes, hostObjs,
                                                          hostArgs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToHost

   /// Functor used to perform the H->H copy of the first function parameter
   template< typename... ARGS >
   class CopyHostToHost< 0, ARGS... > {
      // Some sanity check(s).
      static_assert( 0 < sizeof...( ARGS ), "No arguments received" );
      /// Type of the first variable in the tuple.
      typedef typename std::tuple_element< 0, std::tuple< ARGS... > >::type
         Variable_t;
   public:
      /// Function performing the host->host copy
      int operator()( std::size_t arraySizes,
                      typename ArrayKernelTaskHostVariables< ARGS... >::type&
                         hostObjs,
                      std::tuple< ARGS... >& hostArgs ) {
         // Set up the copy for this first index.
         const int result =
            CopyHostToHostImpl< std::is_pointer< Variable_t >::value, 0,
                                ARGS... >()( arraySizes, hostObjs, hostArgs );
         AKT_CHECK( result );
         // Return gracefully.
         return 0;
      }
   }; // class CopyHostToHost

   /// @}

   /// @name @c std::tuple handling helper functions
   /// @{

   /// Implementation detail for the @c tupleTail function
   template< class TUPLE, std::size_t... Ns >
   auto tupleTailImpl( std::index_sequence< Ns... >, const TUPLE& t ) {
      return std::make_tuple( std::get< Ns + 1u >( t )... );
   }

   /// Function shaving off the first element from a tuple
   template< class TUPLE >
   auto tupleTail( const TUPLE& t ) {
      return tupleTailImpl(
         std::make_index_sequence< std::tuple_size< TUPLE >() - 1u >(), t );
   }

   /// @}

   /// @name Functions executing a user functor on the host or a device
   /// @{

   /// Function executing the user functor on the host
   ///
   /// @param arraySizes Sizes of the arrays appearing in the argument list
   /// @param args Mixture of array and primitive arguments to the functor
   ///
   template< class FUNCTOR, class... ARGS >
   int hostExecute( std::size_t arraySizes, ARGS... args ) {

      // Instantiate the functor.
      auto functor = FUNCTOR();

      // Execute the functor on all elements of the array(s).
      for( std::size_t i = 0; i < arraySizes; ++i ) {
         functor( i, args... );
      }
      return 0;
   }

#ifdef __CUDACC__
   /// Kernel executing the user functor on a device/GPU
   ///
   /// @param arraySizes The sizes of the arrays processed by the user functor
   /// @param args Mixture of arrays and primitive variables
   ///
   template< class FUNCTOR, typename... ARGS >
   __global__
   void deviceKernel( std::size_t arraySizes, ARGS... args ) {

      // Find the current index that we need to process.
      const std::size_t i = blockIdx.x * blockDim.x + threadIdx.x;
      if( i >= arraySizes ) {
         return;
      }

      // Execute the user's function for this index.
      FUNCTOR()( i, args... );
      return;
   }

   /// Function launching the user functor on a device/GPU
   ///
   /// @param stream The CUDA stream to schedule the execution to/with
   /// @param arraySizes The sizes of the arrays processed by the user functor
   /// @param args Mixture of arrays and primitive variables
   ///
   template< class FUNCTOR, class... ARGS >
   int deviceExecute( cudaStream_t stream, std::size_t arraySizes,
                      ARGS... args ) {

      // Decide how many threads to use per execution block. Default
      // to 1024, but scale back from it if necessary.
      int nThreadsPerBlock = 1024;
      for( int i = 0; i < AthCUDA::Info::instance().nDevices(); ++i ) {
         while( nThreadsPerBlock >
                AthCUDA::Info::instance().maxThreadsPerBlock()[ i ] ) {
            nThreadsPerBlock /= 2;
         }
      }

      // If the arrays are not even this large, then reduce the value to the
      // size of the arrays.
      if( arraySizes < nThreadsPerBlock ) {
         nThreadsPerBlock = arraySizes;
      }

      // Schedule the data movement and the calculation.
      const int nBlocks = ( ( arraySizes + nThreadsPerBlock - 1 ) /
                            nThreadsPerBlock );
      deviceKernel< FUNCTOR ><<< nBlocks, nThreadsPerBlock, 0, stream >>>(
         arraySizes, args... );

      // Return gracefully.
      return 0;
   }
#endif // __CUDACC__

   /// @}

   /// Helper class for implementing @c AthCUDA::ArrayKernelTask::execute.
   template< class FUNCTOR >
   class ExecuteImpl {
   public:
      /// Function called at the end of the recursive function calls. This
      /// is the function that actually does something.
      template< typename... ARGS1 >
      static int execute( cudaStream_t
#ifdef __CUDACC__
                             stream
#endif // __CUDACC__
                          , std::size_t arraySizes, const std::tuple<>&,
                          ARGS1...
#ifdef __CUDACC__
                             args
#endif // __CUDACC__
                          ) {

         // If the arrays are empty, return right away.
         if( arraySizes == 0 ) {
            return 0;
         }

#ifdef __CUDACC__
         return deviceExecute< FUNCTOR >( stream, arraySizes, args... );
#else
         return 1;
#endif // __CUDACC__
      }

      /// Function used to recursively pull out all function arguments from
      /// @c AthCUDA::AuxKernelTask::m_args.
      template< typename... ARGS1, typename... ARGS2 >
      static int execute( cudaStream_t stream, std::size_t arraySizes,
                          const std::tuple< ARGS1... >& t, ARGS2... args ) {

         return execute( stream, arraySizes, ::tupleTail( t ), args...,
                         std::get< 0 >( t ) );
      }

      /// Function called at the end of the recursive function calls. This
      /// is the function that actually does something.
      template< typename... ARGS1 >
      static int execute( std::size_t arraySizes,
                          const std::tuple<>&, ARGS1... args ) {

         // If the arrays are empty, return right away.
         if( arraySizes == 0 ) {
            return 0;
         }

         // If not, the let's run it in the current thread on the CPU.
         return hostExecute< FUNCTOR >( arraySizes, args... );
      }

      /// Function used to recursively pull out all function arguments from
      /// @c AthCUDA::AuxKernelTask::m_args.
      template< typename... ARGS1, typename... ARGS2 >
      static int execute( std::size_t arraySizes,
                          const std::tuple< ARGS1... >& t, ARGS2... args ) {

         return execute( arraySizes, ::tupleTail( t ), args...,
                         std::get< 0 >( t ) );
      }
   }; // class ExecuteImpl

} // private namespace

#endif // ATHCUDAKERNEL_ARRAYKERNELTASKIMPL_CUH
