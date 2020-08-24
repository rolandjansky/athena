//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "KernelRunnerSvcImpl.h"
#include "KernelRunnerSvc.h"

// AthCUDA include(s).
#include "AthCUDAInterfaces/IStreamPoolSvc.h"

// System include(s).
#include <cassert>

namespace AthCUDA {

   KernelRunnerSvcImplCallback::
   KernelRunnerSvcImplCallback( KernelRunnerSvc& svc )
   : m_svc( &svc ) {

   }

   KernelRunnerSvcImplCallback& KernelRunnerSvcImplCallback::operator()() {

      assert( m_svc != nullptr );
      m_svc->setTaskFinished();
      return *this;
   }

   KernelRunnerSvcImpl::KernelRunnerSvcImpl( IStreamPoolSvc& streamPool,
                                             KernelRunnerSvc& svc )
   : m_streamPool( &streamPool ), m_callback( svc ) {

   }

   StreamHolder KernelRunnerSvcImpl::getAvailableStream() {

      assert( m_streamPool != nullptr );
      return m_streamPool->getAvailableStream();
   }

} // namespace AthCUDA
