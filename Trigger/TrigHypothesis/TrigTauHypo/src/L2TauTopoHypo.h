/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L2TAUTOPOHYPO_H
#define L2TAUTOPOHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>

/**
 *	@brief L2 hypo for topo start from chains
	@date 30th March 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class L2TauTopoHypo : public HLT::HypoAlgo
{

 public:
	
	L2TauTopoHypo(const std::string& name, ISvcLocator* pSvcLocator);
	~L2TauTopoHypo();

	//Athena Hooks
	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

 private:
	float m_monDR;
	int m_cutCounter;
};
#endif

