// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHCUDACORE_MACROS_CUH
#define ATHCUDACORE_MACROS_CUH

// System include(s).
#include <iostream>
#include <stdexcept>
#include <string>

// CUDA include(s).
#ifdef __CUDACC__
#   include <cuda.h>
#else
typedef void* cudaStream_t;
#endif // __CUDACC__

/// Macro for declaring a device function
#ifdef __CUDACC__
#   define ATHCUDA_DEVICE __device__
#else
#   define ATHCUDA_DEVICE
#endif // __CUDACC__

/// Macro for declaring a host function
#ifdef __CUDACC__
#   define ATHCUDA_HOST __host__
#else
#   define ATHCUDA_HOST
#endif // __CUDACC__

/// Macro for declaring a host+device function
#ifdef __CUDACC__
#   define ATHCUDA_HOST_AND_DEVICE __host__ __device__
#else
#   define ATHCUDA_HOST_AND_DEVICE
#endif // __CUDACC__

/// Helper macro for checking the return types of CUDA function calls
#ifdef __CUDACC__
#   define CUDA_EXP_CHECK( EXP )                                     \
   do {                                                              \
      const cudaError_t ce = EXP;                                    \
      if( ce != cudaSuccess ) {                                      \
         std::cerr << __FILE__ << ":" << __LINE__                    \
                   << " Failed to execute: " << #EXP << " ("         \
                   << cudaGetErrorString( ce ) << ")"                \
                   << std::endl;                                     \
         throw std::runtime_error( std::string( __FILE__ ) + ":" +   \
                                   std::to_string( __LINE__ ) +      \
                                   " Failed to execute: " #EXP " ("  \
                                   + cudaGetErrorString( ce ) +      \
                                   ")" );                            \
      }                                                              \
   } while( false )
#else
#   define CUDA_EXP_CHECK( EXP ) do {} while( false )
#endif // __CUDACC__

/// Helper macro for checking the return types of CUDA function calls,
/// printing an error message when something goes wrong, but letting the
/// execution continue.
#ifdef __CUDACC__
#   define CUDA_PRINT_CHECK( EXP )                                   \
   do {                                                              \
      const cudaError_t ce = EXP;                                    \
      if( ce != cudaSuccess ) {                                      \
         std::cerr << __FILE__ << ":" << __LINE__                    \
                   << " Failed to execute: " << #EXP << " ("         \
                   << cudaGetErrorString( ce ) << ")"                \
                   << std::endl;                                     \
      }                                                              \
   } while( false )
#else
#   define CUDA_PRINT_CHECK( EXP ) do {} while( false )
#endif // __CUDACC__

/// Helper macro for running a CUDA function, but not caring about its output
#ifdef __CUDACC__
#   define CUDA_IGNORE( EXP )                                        \
   do {                                                              \
      EXP;                                                           \
   } while( false )
#else
#   define CUDA_IGNORE( EXP ) do {} while( false )
#endif // __CUDACC__

#endif // ATHCUDACORE_MACROS_CUH
