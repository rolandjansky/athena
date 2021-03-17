/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file
#include <vector> 
#include <cmath>
 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "EFPhotonTauHypo.h"
#include "TrigNavStructure/TrigNavStructure.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Constructor
EFPhotonTauHypo::EFPhotonTauHypo(const std::string& name, ISvcLocator* pSvcLocator):
	HLT::HypoAlgo(name, pSvcLocator)
{

        declareProperty("MvisMin", m_mvis_min = 0.);
        declareProperty("MvisMax", m_mvis_max = 0.);

	declareMonitoredVariable("MvisOfAccepted", m_mon_mvis = -1.);
	declareMonitoredVariable("CutCounter",   m_cutCounter = 0);

        m_mvis = 0.; 
}

//Destructor
EFPhotonTauHypo::~EFPhotonTauHypo()
{  
}

//Athena Hooks
HLT::ErrorCode EFPhotonTauHypo::hltInitialize()
// ----------------------------------------------------------------------
{
	msg() << MSG::INFO << "in initialize()" << endmsg;
        msg() << MSG::INFO << " REGTEST: EFPhotonTauHypo will cut on "<<endmsg;
	msg() << MSG::INFO << " REGTEST: param MvisMin [MeV] " << m_mvis_min <<endmsg;
        msg() << MSG::INFO << " REGTEST: param MvisMax [MeV] " << m_mvis_max <<endmsg;
	
	if(m_mvis_max < m_mvis_min){
		msg() << MSG::ERROR << "EFPhotonTauHypo is uninitialized! " << endmsg;
		return HLT::BAD_JOB_SETUP;
	}

	return HLT::OK;
}

HLT::ErrorCode EFPhotonTauHypo::hltFinalize()
{
	ATH_MSG_DEBUG("Finalising EFPhotonTauHypo"); 
	return HLT::OK;
}

HLT::ErrorCode EFPhotonTauHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
	pass = false;
        m_mvis = 0.;
	m_mon_mvis = -1.0;
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
      		if(comp->name()!="EFPhotonTau_mVis"){
          		ATH_MSG_DEBUG("REGTEST: Not EFPhotonTauInfo TrigComposite");
          		continue;
      		}
      		if(!comp->getDetail("mVis",m_mvis)){
          		ATH_MSG_ERROR("REGTEST: Cannot retrieve mVis");
          		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      		}

                if( m_mvis>=m_mvis_min &&  m_mvis<m_mvis_max ){
			m_mon_mvis = m_mvis*0.001;
			m_cutCounter++;
          		ATH_MSG_DEBUG("Good pair with mVis: " << m_mon_mvis);
			pass = true;
			return HLT::OK;
		}

  	} // End of loop over TrigComposites in L2DielectronInfo 

        return HLT::OK;

}


