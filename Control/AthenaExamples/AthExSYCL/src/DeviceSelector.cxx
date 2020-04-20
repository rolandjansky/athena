//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "DeviceSelector.h"

namespace AthSYCL {

   int DeviceSelector::operator()( const cl::sycl::device& device ) const {

      // Do not accept any NVidia OpenCL devices.
      const std::string vendor =
         device.get_info< cl::sycl::info::device::vendor >();
      const std::string version =
         device.get_info< cl::sycl::info::device::version >();
      if( ( vendor.find( "NVIDIA" ) != std::string::npos ) &&
          ( version.find( "OpenCL" ) != std::string::npos ) ) {
         return -1;
      }

      // For the remaining devices juse use the scoring implemented by
      // cl::sycl::default_selector.
      return m_defaultSelector( device );
   }

} // namespace AthSYCL
