/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file
 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/HLTVertexCombo.h"

//Constructor
HLTVertexCombo::HLTVertexCombo(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){}

//Destructor
HLTVertexCombo::~HLTVertexCombo(){}


HLT::ErrorCode HLTVertexCombo::hltInitialize()
{
	ATH_MSG_DEBUG("HLTVertexCombo Initialize");
	return HLT::OK;
}

HLT::ErrorCode HLTVertexCombo::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
{
	(void)inputTE;
	(void)outputTE;
	ATH_MSG_DEBUG("HLTVertexCombo Execute");
	return HLT::OK;
}

HLT::ErrorCode HLTVertexCombo::hltFinalize()
{
	ATH_MSG_DEBUG("HLTVertexCombo Finalize");
	return HLT::OK;
}

HLT::ErrorCode HLTVertexCombo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{
	pass = false;
	if(inputTE.size() != 2) 
	{
		ATH_MSG_DEBUG("Did not get two Trigger Elements");
		
		return HLT::BAD_JOB_SETUP;
	}
	ATH_MSG_DEBUG("Two Trigger Elements");
	pass = true;
	return HLT::OK;
}
