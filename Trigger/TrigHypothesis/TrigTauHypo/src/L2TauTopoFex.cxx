/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/L2TauTopoFex.h"

//Constructor
L2TauTopoFex::L2TauTopoFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){

	m_cont = 0;
	m_dR = -1;

}

//Destructor
L2TauTopoFex::~L2TauTopoFex(){}


HLT::ErrorCode L2TauTopoFex::hltInitialize()
{
	msg() << MSG::INFO << "in initialize()" << endmsg;
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE)
{
	ATH_MSG_DEBUG("L2TauTopoFex Execute");
	
	m_cont=new xAOD::TrigCompositeContainer();
  	xAOD::TrigCompositeAuxContainer contaux;
	m_cont->setStore(&contaux);
	xAOD::TrigComposite *comp=new xAOD::TrigComposite();
	m_cont->push_back(comp);
	comp->setName("L2TAUTSF_dR");
    	comp->setDetail("dR",m_dR);
	attachFeature(outputTE,m_cont,"L2TAUTSFInfo"); 

	ATH_MSG_DEBUG("L2TAUTSFInfo attached feature");
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::hltFinalize()
{
	ATH_MSG_DEBUG("L2TauTopoFex Finalize");
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{

	HLT::ErrorCode status1 = HLT::OK; 
        HLT::ErrorCode status2 = HLT::OK;
	pass = false;
	m_dR = -1.;	

  	ATH_MSG_DEBUG("Running L2TauTopoFex::acceptInputs");

  	if ( inputTE.size() != 2 ) {
    		ATH_MSG_ERROR( "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured");
    		return HLT::BAD_JOB_SETUP;
  	}

	const HLT::TriggerElement* te1 = inputTE[0];
  	const HLT::TriggerElement* te2 = inputTE[1];

	const TrigRoiDescriptor* roiDescriptor1 = 0;
        const TrigRoiDescriptor* roiDescriptor2 = 0;
	status1 = getFeature(te1, roiDescriptor1);
	status2 = getFeature(te2, roiDescriptor2);	

      	if ( status1 != HLT::OK || roiDescriptor1 == 0 || status2 != HLT::OK || roiDescriptor2 == 0){ 
          	ATH_MSG_WARNING("No RoIDescriptors for this Trigger Elements! ");
		return HLT::MISSING_FEATURE;
	} else 
          	ATH_MSG_DEBUG("Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2);

	float dEta = fabs(roiDescriptor1->eta() - roiDescriptor2->eta());
        float dPhi = fabs(HLT::wrapPhi(roiDescriptor1->phi()-roiDescriptor2->phi()));

	m_dR = sqrt(dEta*dEta+dPhi*dPhi); // compute dR

	pass = true;
	return HLT::OK;
}
