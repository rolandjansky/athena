// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDACore/Info.h"
#include "AthCUDACore/Macros.cuh"

// System include(s).
#include <cmath>

namespace AthCUDA {

   Info& Info::instance() {

      static Info info;
      return info;
   }

   int Info::nDevices() const {

      return m_nDevices;
   }

   const std::vector< std::string >& Info::names() const {

      return m_names;
   }

   const std::vector< int >& Info::maxThreadsPerBlock() const {

      return m_maxThreadsPerBlock;
   }

   const std::vector< bool >& Info::concurrentKernels() const {

      return m_concurrentKernels;
   }

   const std::vector< std::size_t >& Info::totalMemory() const {

      return m_totalMemory;
   }

   void Info::print() const {

      std::cout << *this << std::endl;
      return;
   }

   Info::Info() {

      // Collect all information about all the available devices on
      // construction.
      CUDA_IGNORE( cudaGetDeviceCount( &m_nDevices ) );
#ifdef __CUDACC__
      for( int i = 0; i < m_nDevices; ++i ) {

         // Retrieve all properties of this device.
         cudaDeviceProp properties;
         CUDA_EXP_CHECK( cudaGetDeviceProperties( &properties, i ) );

         // Set the values in our variables.
         m_names.push_back( properties.name );
         m_maxThreadsPerBlock.push_back( properties.maxThreadsPerBlock );
         m_concurrentKernels.push_back( properties.concurrentKernels );
         m_totalMemory.push_back( properties.totalGlobalMem );
      }
#endif // __CUDACC__
   }

   std::ostream& operator<< ( std::ostream& out, const AthCUDA::Info& info ) {

      if( info.nDevices() > 0 ) {
         for( int i = 0; i < info.nDevices(); ++i ) {
            out << " /-- Device ID " << i << " " << std::string( 31, '-' )
                << "\\" << std::endl;
            out << " | Name: " << info.names()[ i ]
                << std::string( ( 39 > info.names()[ i ].length() ?
                                  39 - info.names()[ i ].length() : 0 ), ' ' )
                << "|" << std::endl;
            const std::size_t threadDigits =
               ( static_cast< std::size_t >( std::log10( info.maxThreadsPerBlock()[ i ] ) )
                 + 1 );
            out << " | Max. threads per block: "
                << info.maxThreadsPerBlock()[ i ]
                << std::string( ( 21 > threadDigits ?
                                  21 - threadDigits : 0 ), ' ' )
                << "|" << std::endl;
            out << " | Concurrent kernels: "
                << ( info.concurrentKernels()[ i ] ? "true " : "false" )
                << std::string( 20, ' ' ) << "|" << std::endl;
            static const double MegaBytes = 1.0 / ( 1024 * 1024 );
            const double totalMem = info.totalMemory()[ i ] * MegaBytes;
            const std::size_t memDigits =
               ( static_cast< std::size_t >( std::log10( totalMem ) ) + 1 );
            out << " | Total memory: " << totalMem << " MB"
                << std::string( ( 25 > memDigits ?
                                  25 - memDigits : 0 ), ' ' ) << "|"
                << std::endl;
            out << " \\" << std::string( 46, '-' ) << "/";
         }
      } else {
         out << "<No CUDA device(s)>";
      }
      return out;
   }

} // namespace AthCUDA
