/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/L2TauTopoFex.h"

//Constructor
L2TauTopoFex::L2TauTopoFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){}

//Destructor
L2TauTopoFex::~L2TauTopoFex(){}


HLT::ErrorCode L2TauTopoFex::hltInitialize()
{
	ATH_MSG_DEBUG("L2TauTopoFex Initialize");
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
{
	(void)inputTE;
	(void)outputTE;
	ATH_MSG_DEBUG("L2TauTopoFex Execute");
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::hltFinalize()
{
	ATH_MSG_DEBUG("L2TauTopoFex Finalize");
	return HLT::OK;
}

HLT::ErrorCode L2TauTopoFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass )
{
	pass = false;
        (void)inputTE;






	pass = true;
	return HLT::OK;
}
