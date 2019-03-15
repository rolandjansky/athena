/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFTAUTOPOHYPO_H
#define EFTAUTOPOHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>

/**
 *	@brief EF hypo for chains with HLT dr cut
	@date 30th March 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class EFTauTopoHypo : public HLT::HypoAlgo
{

 public:
	
	EFTauTopoHypo(const std::string& name, ISvcLocator* pSvcLocator);
	~EFTauTopoHypo();

	//Athena Hooks
	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

 private:
        float m_monDRall; 
	float m_monDR;
	float m_dR;
	float m_dRmax;
	float m_dRmin;
	int m_cutCounter;
};
#endif

