// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IITrigDecisionToolD3PD.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_IITRIGDECISIONTOOLD3PD_H
#define TRIGROOTANALYSIS_IITRIGDECISIONTOOLD3PD_H

// STL include(s):
#include <vector>
#include <string>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "Conditions.h"
#include "ChainGroup.h"

namespace D3PD {

   /**
    *  @short Pure virtual interface for the D3PD-based TrigDecisionTool
    *
    *         In order to go for sure that both implementations (Athena and
    *         standalone) of the TDT follow the same interface, they both
    *         have to implement this pure virtual interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class IITrigDecisionToolD3PD {

   public:
      /// Virtual destructor to make vtable happy
      virtual ~IITrigDecisionToolD3PD() {}

      /// Find out whether a given LVL1 item / HLT chain was passed in the event
      virtual ::Bool_t IsPassed( const std::string& name,
                                 D3PD::TrigDefs::DecisionTypes type =
                                 D3PD::TrigDefs::Physics ) = 0;

      /// Get the list of triggers from this chain group that passed the current event
      virtual std::vector< std::string >
      GetPassedTriggers( D3PD::TrigDefs::DecisionTypes type =
                         D3PD::TrigDefs::Physics ) = 0;
      /// Get the list of LVL1 triggers from the chain group that passed the current event
      virtual std::vector< std::string >
      GetPassedL1Triggers( D3PD::TrigDefs::DecisionTypes type =
                           D3PD::TrigDefs::Physics ) = 0;
      /// Get the list of LVL2 triggers from the chain group that passed the current event
      virtual std::vector< std::string >
      GetPassedL2Triggers( D3PD::TrigDefs::DecisionTypes type =
                           D3PD::TrigDefs::Physics ) = 0;
      /// Get the list of EF triggers from the chain group that passed the current event
      virtual std::vector< std::string >
      GetPassedEFTriggers( D3PD::TrigDefs::DecisionTypes type =
                           D3PD::TrigDefs::Physics ) = 0;

      /// Create a chain group from a list of patterns
      virtual D3PD::ChainGroup
      GetChainGroup( const std::vector< std::string >& patterns ) = 0;
      /// Create a chain group from a single pattern
      virtual D3PD::ChainGroup GetChainGroup( const std::string& pattern ) = 0;

   }; // class IITrigDecisionToolD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_IITRIGDECISIONTOOLD3PD_H
