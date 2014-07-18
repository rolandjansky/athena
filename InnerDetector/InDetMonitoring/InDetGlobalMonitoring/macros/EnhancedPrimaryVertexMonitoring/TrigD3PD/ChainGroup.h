// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainGroup.h 318255 2010-08-27 11:46:34Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CHAINGROUP_H
#define TRIGGERD3PDMAKER_CHAINGROUP_H

// STL include(s):
#include <vector>
#include <string>

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "Conditions.h"

namespace D3PD {

   // Forward declaration(s):
   class TrigConfigSvcD3PD;
   namespace Trig {
      class IDataAccess;
      class ChainGroupHandling;
   }

   /**
    *  @short Class implementing the chain group functionality
    *
    *         This class acts in pretty much the same way as the ChainGroup class in
    *         the TrigDecisionTool package. Such objects can only be produced by
    *         TrigDecisionToolD3PD, it has no public constructor.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 337331 $
    * $Date: 2010-08-27 13:46:34 +0200 (Fri, 27 Aug 2010) $
    */
   class ChainGroup : public TNamed {

      /// Allow the TDT to create these objects
      friend class D3PD::Trig::ChainGroupHandling;

   protected:
      /// Constructor receiving all the needed information
      ChainGroup( const std::vector< std::string >& triggerNames,
                  const Trig::IDataAccess& parent,
                  TrigConfigSvcD3PD& svc );

   public:
      /// Find out if the chain group passed the selection in the event
      Bool_t IsPassed( TrigDefs::DecisionTypes type = TrigDefs::Physics );
      /// Get the overall prescale of the chain group
      Float_t GetPrescale();
      /// Get the list of triggers matching the selection
      const std::vector< std::string >& GetListOfTriggers();

   private:
      /// Update the object using the trigger configuration
      Bool_t Update();
      /// Function splitting a comma separated list into a vector
      static std::vector< std::string > ToVector( const std::string& names );

      const Trig::IDataAccess& m_parent; ///< Interface for accessing the trigger data
      TrigConfigSvcD3PD& m_configSvc; ///< Reference to the configuration service

      const std::vector< std::string > m_triggerNames; ///< The names given by the user

      Int_t m_smk; ///< The last SMK that was used to update the object
      std::vector< std::string > m_existingTriggers; ///< Names of the triggers
      std::vector< Int_t >       m_existingIDs;      ///< IDs of the triggers

      ClassDef( ChainGroup, 0 )

   }; // class ChainGroup

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CHAINGROUP_H
