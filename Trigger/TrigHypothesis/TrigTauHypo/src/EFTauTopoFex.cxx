/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/EFTauTopoFex.h"

//Constructor
EFTauTopoFex::EFTauTopoFex(const std::string& name, ISvcLocator* pSvcLocator) : HLT::ComboAlgo(name, pSvcLocator){}

//Destructor
EFTauTopoFex::~EFTauTopoFex(){}


HLT::ErrorCode EFTauTopoFex::hltInitialize()
{
	ATH_MSG_DEBUG("EFTauTopoFex Initialize");
	return HLT::OK;
}

HLT::ErrorCode EFTauTopoFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
{
	(void)inputTE;
	(void)outputTE;
	ATH_MSG_DEBUG("EFTauTopoFex Execute");
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
        (void)inputTE;






	pass = true;
	return HLT::OK;
}
