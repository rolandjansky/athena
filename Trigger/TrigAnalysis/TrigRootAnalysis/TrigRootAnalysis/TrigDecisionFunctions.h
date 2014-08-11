// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionFunctions.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_TRIGDECISIONFUNCTIONS_H
#define TRIGROOTANALYSIS_TRIGDECISIONFUNCTIONS_H

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "IITrigDecisionToolD3PD.h"
#include "IDataAccess.h"
#include "IConfigAccess.h"

namespace D3PD {

   namespace Trig {

      /**
       *  @short Class collecting the higher lever TDT functions
       *
       *         In order not to have to implement these functions in multiple
       *         places, this class was made to hold them.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 502443 $
       * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
       */
      class TrigDecisionFunctions : public virtual IITrigDecisionToolD3PD,
                                    public virtual D3PD::Trig::IDataAccess,
                                    public virtual D3PD::Trig::IConfigAccess {

      public:
         /// Find out whether a given LVL1 item / HLT chain was passed in the event
         virtual ::Bool_t IsPassed( const std::string& name,
                                    D3PD::TrigDefs::DecisionTypes type =
                                    D3PD::TrigDefs::Physics );

         /// Get the list of triggers from this chain group that passed the current event
         virtual std::vector< std::string >
         GetPassedTriggers( D3PD::TrigDefs::DecisionTypes type =
                            D3PD::TrigDefs::Physics );
         /// Get the list of LVL1 triggers from the chain group that passed the current event
         virtual std::vector< std::string >
         GetPassedL1Triggers( D3PD::TrigDefs::DecisionTypes type =
                              D3PD::TrigDefs::Physics );
         /// Get the list of LVL2 triggers from the chain group that passed the current event
         virtual std::vector< std::string >
         GetPassedL2Triggers( D3PD::TrigDefs::DecisionTypes type =
                              D3PD::TrigDefs::Physics );
         /// Get the list of EF triggers from the chain group that passed the current event
         virtual std::vector< std::string >
         GetPassedEFTriggers( D3PD::TrigDefs::DecisionTypes type =
                              D3PD::TrigDefs::Physics );

      }; // class TrigDecisionFunctions

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_TRIGDECISIONFUNCTIONS_H
