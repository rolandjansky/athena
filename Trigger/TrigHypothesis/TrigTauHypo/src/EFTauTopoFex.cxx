/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"
#include "xAODTau/TauJetContainer.h"
#include "TrigTauHypo/EFTauTopoFex.h"

//Constructor
EFTauTopoFex::EFTauTopoFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){
	
	m_cont = 0;

}

//Destructor
EFTauTopoFex::~EFTauTopoFex(){}


HLT::ErrorCode EFTauTopoFex::hltInitialize()
{
	msg() << MSG::INFO << "in initialize()" << endmsg;
	return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE)
{
	ATH_MSG_DEBUG("EFTauTopoFex Execute");
	
	m_cont=new xAOD::TrigCompositeContainer();
  	xAOD::TrigCompositeAuxContainer contaux;
	m_cont->setStore(&contaux);
	for(const auto &dR:m_dR) {
		xAOD::TrigComposite *comp=new xAOD::TrigComposite();
		m_cont->push_back(comp);
		comp->setName("EFTAU_dR");
    		comp->setDetail("dR",dR);
	}
	attachFeature(outputTE,m_cont,"EFTAUInfo"); 

	ATH_MSG_DEBUG("EFTAUInfo attached feature");
	return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::hltFinalize()
{
	ATH_MSG_DEBUG("EFTauTopoFex Finalize");
	return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{
	pass = false;
	m_dR.clear();	

  	ATH_MSG_DEBUG("Running EFTauTopoFex::acceptInputs");

  	if ( inputTE.size() != 2 ) {
    		ATH_MSG_ERROR( "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured");
    		return HLT::BAD_JOB_SETUP;
  	}

	std::vector<const xAOD::TauJetContainer*> vectorTauContainers1;
	std::vector<const xAOD::TauJetContainer*> vectorTauContainers2;

	const HLT::TriggerElement* te1 = inputTE[0];
        const HLT::TriggerElement* te2 = inputTE[1];


	if ( getFeatures(te1, vectorTauContainers1) != HLT::OK || getFeatures(te2, vectorTauContainers2) != HLT::OK )
		{
                       msg() << MSG::WARNING << "Failed to get xAOD::TauJetContainer collections" << endmsg;
                        return HLT::MISSING_FEATURE;
                }
	else { msg() << MSG::DEBUG  << "xAOD::TauJetContainer collections successfully retrieved" << endmsg; }

        if (vectorTauContainers1.size() < 1 || vectorTauContainers2.size() < 1) {
                msg() << MSG::DEBUG << " empty xAOD::TauJetContainer" << endmsg;
                return HLT::OK;
        }

	const xAOD::TauJetContainer* TauJetContainer1 = vectorTauContainers1.back();
	const xAOD::TauJetContainer* TauJetContainer2 = vectorTauContainers2.back();
	xAOD::TauJetContainer::const_iterator tau1, tau2;

	for ( tau1 = TauJetContainer1->begin(); tau1 != TauJetContainer1->end(); ++tau1 ) {
		for ( tau2 = TauJetContainer2->begin(); tau2 != TauJetContainer2->end(); ++tau2 ) {
			        float dEta = fabs((*tau1)->eta() - (*tau2)->eta());
        			float dPhi = fabs(HLT::wrapPhi((*tau1)->phi()-(*tau2)->phi()));
        			float dR = sqrt(dEta*dEta+dPhi*dPhi); // compute dR
				m_dR.push_back(dR);
		}
	}

	pass = true;
	return HLT::OK;
}
