/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file
#include <vector> 
#include <cmath>
 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "L2TauTopoHypo.h"
#include "TrigNavStructure/TrigNavStructure.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Constructor
L2TauTopoHypo::L2TauTopoHypo(const std::string& name, ISvcLocator* pSvcLocator):
	HLT::HypoAlgo(name, pSvcLocator)
{

	declareMonitoredVariable("DROfAccepted", m_monDR=-1.);
	declareMonitoredVariable("CutCounter",   m_cutCounter=0);
}

//Destructor
L2TauTopoHypo::~L2TauTopoHypo()
{  
}

//Athena Hooks
HLT::ErrorCode L2TauTopoHypo::hltInitialize()
// ----------------------------------------------------------------------
{
	msg() << MSG::INFO << "in initialize()" << endmsg;
	
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoHypo::hltFinalize()
{
	ATH_MSG_DEBUG("Finalising L2TauTopoHypo"); 
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
	pass = false;
	m_monDR = 0;
	m_cutCounter = 0;

  	std::vector<const xAOD::TrigCompositeContainer*> vms;
  	if (getFeatures(inputTE, vms) != HLT::OK) {
      		ATH_MSG_WARNING("Failed to get TrigCompositeContainer");
    		return HLT::MISSING_FEATURE;
  	} else {
      		ATH_MSG_DEBUG("Number of TrigCompositeContainers " << vms.size());
  	}

	const xAOD::TrigCompositeContainer *cont=vms.back();
  	if(!cont){
      		ATH_MSG_ERROR("REGTEST: Retrieval of TrigCompositeContainer from vector failed");
      		return HLT::OK;
  	}
  	else ATH_MSG_DEBUG("REGTEST: Number of TrigComposites " << cont->size());

	for(const xAOD::TrigComposite* comp:*cont){
      		if(comp->name()!="L2TAUTSF_dR"){
          		ATH_MSG_DEBUG("REGTEST: Not L2TAUTSFInfo TrigComposite");
          		continue;
      		}
      		if(!comp->getDetail("dR",m_monDR)){
          		ATH_MSG_ERROR("REGTEST: Cannot retrieve dR");
          		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      		}

		// This hypo at the moment doesn't do any selection. Only monitor the dR to check the correctness of the topo_start_from RoI conbination (D.Zanzi)

          	ATH_MSG_DEBUG("Combination with dR: " << m_monDR);

  	} // End of loop over TrigComposites in L2DielectronInfo 

        pass = true;
        return HLT::OK;


}


