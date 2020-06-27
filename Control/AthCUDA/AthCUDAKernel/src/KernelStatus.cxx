//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDAKernel/KernelStatus.h"

namespace AthCUDA {

   KernelStatus::KernelStatus()
   : m_code( 0 ), m_mutex() {

      m_mutex.lock();
   }

   void KernelStatus::finished( int code ) {

      m_code = code;
      m_mutex.unlock();
      return;
   }

   int KernelStatus::wait() {

      // Wait for the task to finish.
      std::lock_guard< std::mutex > lock( m_mutex );
      // Return the final status code of the kernel.
      return m_code;;
   }

} // namespace AthCUDA
