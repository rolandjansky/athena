// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainGroupHandling.h 318255 2010-08-27 11:46:34Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CHAINGROUPHANDLING_H
#define TRIGGERD3PDMAKER_CHAINGROUPHANDLING_H

// STL include(s):
#include <vector>
#include <string>

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "IDataAccess.h"
#include "IConfigAccess.h"
#include "ChainGroup.h"

namespace D3PD {

   namespace Trig {

      /**
       *  @short Class implementing the functions that handle ChainGroup-s
       *
       *         The functionality of the TDT is split up in many pieces. This
       *         piece takes care of creating ChainGroup objects in the correct
       *         way.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 337331 $
       * $Date: 2010-08-27 13:46:34 +0200 (Fri, 27 Aug 2010) $
       */
      class ChainGroupHandling : public virtual TNamed,
                                 public virtual Trig::IDataAccess,
                                 public virtual Trig::IConfigAccess {

      public:
         /// Default constructor
         ChainGroupHandling();

         /// Create a chain group from a list of patterns
         ChainGroup GetChainGroup( const std::vector< std::string >& patterns );
         /// Create a chain group from a single pattern
         ChainGroup GetChainGroup( const std::string& pattern );

         ClassDef( ChainGroupHandling, 0 )

      }; // class ChainGroupHandling

   } // namespace Trig

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CHAINGROUPHANDLING_H
