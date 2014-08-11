// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthConfigAccess.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_ATHCONFIGACCESS_H
#define TRIGROOTANALYSIS_ATHCONFIGACCESS_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

// Local include(s):
#include "../TrigRootAnalysis/IConfigAccess.h"
#include "../TrigRootAnalysis/ITrigConfigSvcD3PD.h"

namespace D3PD {

   /**
    *  @short Athena implementation of the IConfigAccess interface
    *
    *         This implementation of the IConfigAccess interface is used as
    *         a component of the Athena version of ITrigDecisionToolD3PD.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class AthConfigAccess : public virtual D3PD::Trig::IConfigAccess {

   public:
      /// Default constructor
      AthConfigAccess();

      /// Initialization function
      StatusCode initialize();

      /// Function giving access to the trigger configuration service object
      virtual D3PD::IITrigConfigSvcD3PD&
      GetConfigSvc( ::Bool_t update = kTRUE );

   private:
      /// Handle to the configuration service
      ServiceHandle< ITrigConfigSvcD3PD > m_service;

   }; // class AthConfigAccess

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ATHCONFIGACCESS_H
