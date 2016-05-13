/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFTAUTOPOFEX_H
#define EFTAUTOPOFEX_H


#include "TrigInterfaces/ComboAlgo.h"

/**
 *	@brief EF Fex for topo start from chains
	@date 30th March 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class EFTauTopoFex : public HLT::ComboAlgo
{
	public:

		EFTauTopoFex(const std::string& name, ISvcLocator* pSvcLocator);
		~EFTauTopoFex();

		//Athena Hooks
		HLT::ErrorCode hltInitialize();
		HLT::ErrorCode hltFinalize();
		HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass);
		HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
};
#endif
