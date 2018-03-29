/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowUtils/RetrievePFOTool.h"

namespace CP {

  RetrievePFOTool::RetrievePFOTool(const std::string& name) : asg::AsgTool( name ) {     
    declareProperty("ChargedInputContainer", m_inCharged="");
    declareProperty("NeutralInputContainer", m_inNeutral="");
}

  const xAOD::PFOContainer* RetrievePFOTool::retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale) const {
    return this->retrievePFO(theScale, CP::all);
  }

  const xAOD::PFOContainer* RetrievePFOTool::retrievePFO(const CP::PFO_JetMETConfig_inputScale& theScale, const CP::PFO_JetMETConfig_charge& theCharge) const {
    
    //This is a new VIEW container that we will fill with the clients chosen PFO - the client owns this object, and should delete it
    xAOD::PFOContainer* newContainer = new xAOD::PFOContainer(SG::VIEW_ELEMENTS);

    StatusCode sc;
    //Then we retrieve the charged PFO - this is the same for both EM and LC modes
    if (CP::charged == theCharge || CP::all == theCharge) {
      if(m_inCharged!="") sc = this->fillPFOContainer(newContainer, m_inCharged);
      else sc = this->fillPFOContainer(newContainer, "JetETMissChargedParticleFlowObjects");
      if (sc.isFailure()) ATH_MSG_WARNING( " could not fill charged pfo container ");
    }

    //Then we retrieve the neutral PFO - thu is different in the EM and LC modes
    if (CP::neutral == theCharge || CP::all == theCharge) {
      sc = this->retrieve_neutralPFO(theScale,newContainer);
      if (sc.isFailure()) ATH_MSG_WARNING(" could not fill neutral pfo container ");
    }

    //The client is only allowed a const pointer, to prevent them modifying the PFO
    const xAOD::PFOContainer* newPFO = const_cast<xAOD::PFOContainer*>(newContainer);

    return newPFO;

  }

  StatusCode RetrievePFOTool::retrieve_neutralPFO(const CP::PFO_JetMETConfig_inputScale& theScale, xAOD::PFOContainer* theContainer) const {

    if (CP::EM == theScale) {
      //Get neutral PFO for EM mode - stored in one container
      if(m_inNeutral!="") ATH_CHECK(this->fillPFOContainer(theContainer, m_inNeutral));
      else ATH_CHECK(this->fillPFOContainer(theContainer, "JetETMissNeutralParticleFlowObjects"));
    }// EM mode
    else if (CP::LC == theScale){
      //Get neutral PFO for LC mode - stored in two containers
      if(m_inNeutral!="")
        ATH_CHECK(this->fillPFOContainer(theContainer, m_inNeutral));
      else{
        //Get neutral PFO for LC mode - stored in two containers
        ATH_CHECK(this->fillPFOContainer(theContainer, "JetETMissLCNeutralParticleFlowObjects"));
        ATH_CHECK(this->fillPFOContainer(theContainer, "JetETMissLCNonModifiedNeutralParticleFlowObjects"));
      }
    }//LC mode

    return StatusCode::SUCCESS;

  }


  StatusCode RetrievePFOTool::fillPFOContainer( xAOD::PFOContainer* newContainer, const std::string& theName) const {

    const xAOD::PFOContainer* thePFO = NULL;
    ATH_CHECK( evtStore()->retrieve(thePFO, theName));
      
    xAOD::PFOContainer::const_iterator firstPFO = thePFO->begin();
    xAOD::PFOContainer::const_iterator lastPFO = thePFO->end();

    for (; firstPFO != lastPFO; ++firstPFO) {
      xAOD::PFO* thePFO = const_cast<xAOD::PFO*>(*firstPFO);
      newContainer->push_back(thePFO);
    }//PFO loop

    return StatusCode::SUCCESS;
  }

}
