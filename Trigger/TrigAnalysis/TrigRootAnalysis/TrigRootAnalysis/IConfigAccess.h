// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IConfigAccess.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_ICONFIGACCESS_H
#define TRIGROOTANALYSIS_ICONFIGACCESS_H

// ROOT include(s):
#include <Rtypes.h>

namespace D3PD {

   // Forward declaration(s):
   class IITrigConfigSvcD3PD;

   namespace Trig {

      /**
       *  @short Interface providing access to the configuration service
       *
       *         In order to be able to split up the functionality of the TDT into
       *         multiple parts, the separate parts communicate with each other
       *         through such interfaces. This interface gives access to all the other
       *         components to the trigger configuration service object.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 502443 $
       * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
       */
      class IConfigAccess {

      public:
         /// Virtual destructor to make vtable happy
         virtual ~IConfigAccess() {}

         /// Function giving access to the trigger configuration service object
         virtual D3PD::IITrigConfigSvcD3PD&
         GetConfigSvc( ::Bool_t update = kTRUE ) = 0;

      }; // class IConfigAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ICONFIGACCESS_H
