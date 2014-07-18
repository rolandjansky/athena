// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IConfigAccess.h 325162 2010-10-05 06:58:44Z krasznaa $
#ifndef TRIGGERD3PDMAKER_ICONFIGACCESS_H
#define TRIGGERD3PDMAKER_ICONFIGACCESS_H

namespace D3PD {

   // Forward declaration(s):
   class TrigConfigSvcD3PD;

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
       * $Revision: 337331 $
       * $Date: 2010-10-05 08:58:44 +0200 (Tue, 05 Oct 2010) $
       */
      class IConfigAccess {

      public:
         /// Virtual destructor to make vtable happy
         virtual ~IConfigAccess() {}

         /// Function giving access to the trigger configuration service object
         virtual TrigConfigSvcD3PD& GetConfigSvc() = 0;

      }; // class IConfigAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_ICONFIGACCESS_H
