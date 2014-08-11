// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConfigAccess.h 502898 2012-05-29 11:15:13Z krasznaa $
#ifndef TRIGROOTANALYSIS_CONFIGACCESS_H
#define TRIGROOTANALYSIS_CONFIGACCESS_H

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "IConfigAccess.h"
#include "IDataAccess.h"
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
       *         Note that the implementation needs to access the
       *         IDataAccess interface, which makes the class structure a
       *         bit complicated...
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 502898 $
       * $Date: 2012-05-29 13:15:13 +0200 (Tue, 29 May 2012) $
       */
      class ConfigAccess : public virtual ::TNamed,
                           public virtual D3PD::Trig::IConfigAccess,
                           public virtual D3PD::Trig::IDataAccess {

      public:
         /// Constructor with the configuration tree
         ConfigAccess( ::TTree* confTree );

         /// Return a reference to the trigger configuration service
         virtual D3PD::IITrigConfigSvcD3PD&
         GetConfigSvc( ::Bool_t update = kTRUE );

         /// Set up to use a new configuration tree
         ::Bool_t SetConfigTree( ::TTree* confTree );

      private:
         D3PD::TrigConfigSvcD3PD m_configSvc; ///< The trigger configuration service

         ClassDef( D3PD::Trig::ConfigAccess, 0 )

      }; // class ConfigAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_CONFIGACCESS_H
