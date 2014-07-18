// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConfigAccess.h 325162 2010-10-05 06:58:44Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CONFIGACCESS_H
#define TRIGGERD3PDMAKER_CONFIGACCESS_H

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "IConfigAccess.h"
#include "TrigConfigSvcD3PD.h"

// Forward declaration(s):
class TTree;

namespace D3PD {

   namespace Trig {

      /**
       *  @short Implementation of the IConfigAccess interface
       *
       *         This class implements a functionality of the TDT that's used by
       *         multiple other components of the class.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 337331 $
       * $Date: 2010-10-05 08:58:44 +0200 (Tue, 05 Oct 2010) $
       */
      class ConfigAccess : public virtual TNamed,
                           public virtual IConfigAccess {

      public:
         /// Constructor with the configuration tree
         ConfigAccess( TTree* confTree );

         /// Return a reference to the trigger configuration service
         virtual TrigConfigSvcD3PD& GetConfigSvc();

      private:
         TrigConfigSvcD3PD m_configSvc; ///< The trigger configuration service

         ClassDef( ConfigAccess, 0 )

      }; // class ConfigAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CONFIGACCESS_H
