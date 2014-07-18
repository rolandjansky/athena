// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolD3PD.h 318255 2010-08-27 11:46:34Z krasznaa $
#ifndef TRIGGERD3PDMAKER_TRIGDECISIONTOOLD3PD_H
#define TRIGGERD3PDMAKER_TRIGDECISIONTOOLD3PD_H

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
#include "TrigConfigSvcD3PD.h"
#include "DataAccess.h"
#include "ConfigAccess.h"
#include "ChainGroupHandling.h"
#include "Conditions.h"

// Forward declaration(s):
class TTree;

namespace D3PD {

   /**
    *  @short Tool for interpreting trigger information in the D3PDs
    *
    *         This tool can be used to interpret the trigger information from the
    *         D3PDs, which is stored in an encoded way. The way in which the data
    *         is stored is explained in
    *
    *         https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerD3PDMaker
    *
    *         Currently the only implemented feature is to ask the tool if a trigger
    *         item passed various conditions.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 337331 $
    * $Date: 2010-08-27 13:46:34 +0200 (Fri, 27 Aug 2010) $
    */
   class TrigDecisionToolD3PD : public virtual TNamed,
                                public Trig::DataAccess,
                                public Trig::ConfigAccess,
                                public Trig::ChainGroupHandling {

   public:
      /// Constructor specifying the D3PD trees that the tool should act on
      TrigDecisionToolD3PD( TTree* eventTree = 0, TTree* confTree = 0,
                            const TString& prefix = "trig_" );

      /// Find out whether a given LVL1 item / HLT chain was passed in the event
      Bool_t IsPassed( const std::string& name,
                       TrigDefs::DecisionTypes type = TrigDefs::Physics );

      ClassDef( TrigDecisionToolD3PD, 0 )

   }; // class TrigDecisionToolD3PD

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_TRIGDECISIONTOOLD3PD_H
