/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthD2Decorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Robert Les (robert.les@cern.ch)
//

#include "DerivationFrameworkMCTruth/TruthD2Decorator.h"
#include "xAODJet/JetContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  TruthD2Decorator::TruthD2Decorator(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }

  StatusCode TruthD2Decorator::addBranches() const
  {

      // Set up the decorators 
      SG::AuxElement::Decorator< float > decoratorD2("D2"); 

      // Get the Large-R jet Container
      const xAOD::JetContainer* largeRjets = evtStore()->retrieve< const xAOD::JetContainer >("AntiKt10TruthTrimmedPtFrac5SmallR20Jets");
      
      if(!largeRjets) {
        ATH_MSG_ERROR ("Couldn't retrieve JetContainer with key AntiKt10TruthTrimmedPtFrac5SmallR20Jets");
        return StatusCode::FAILURE;
      }

      // loop over jet collection
      for (xAOD::JetContainer::const_iterator jet=largeRjets->begin(); jet!=largeRjets->end(); ++jet) {
        //get ECF
        float ecf1 = (*jet)->getAttribute<float>("ECF1");
        float ecf2 = (*jet)->getAttribute<float>("ECF2");
        float ecf3 = (*jet)->getAttribute<float>("ECF3");
        
        //calculate D2 and decorate
        float D2=-999;
        if(fabs(ecf2)>1e-8)
          D2=ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
        decoratorD2(**jet) = D2;
      }

      return StatusCode::SUCCESS;
  }
}
