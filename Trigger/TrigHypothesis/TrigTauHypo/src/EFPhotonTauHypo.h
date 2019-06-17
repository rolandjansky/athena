/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFPHOTONTAUHYPO_H
#define EFPHOTONTAUHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>

/**
 *	@brief EF hypo for photon+tau chains
	@date 21st July 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class EFPhotonTauHypo : public HLT::HypoAlgo
{

 public:
	
	EFPhotonTauHypo(const std::string& name, ISvcLocator* pSvcLocator);
	~EFPhotonTauHypo();

	//Athena Hooks
	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

 private:
        float m_mvis;
        float m_mvis_max;
        float m_mvis_min;
	float m_mon_mvis;
	int m_cutCounter;
};
#endif

