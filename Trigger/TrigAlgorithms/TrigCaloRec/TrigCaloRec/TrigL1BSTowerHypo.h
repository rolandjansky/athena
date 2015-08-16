/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************
 *
 * NAME : TrigL1BSTowerHypo.h
 * PACKAGE : Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR : Denis Oliveira Damazio
 * CREATED : 2015, July
 *
**********************************************/

#ifndef TRIGL1BSTOWERHYPO_H
#define TRIGL1BSTOWERHYPO_H

#include "TrigInterfaces/HypoAlgo.h"


class TrigL1BSTowerHypo : public HLT::HypoAlgo {

public :
	TrigL1BSTowerHypo( const std::string& name, ISvcLocator* pSvcLocator );
	~TrigL1BSTowerHypo(); 

	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute( const HLT::TriggerElement* outputTE, bool& pass );

private :
	std::vector<float> m_EtEM;
	std::vector<float> m_EtHAD;
	std::vector<float> m_etaEM;
	std::vector<float> m_phiEM;
	std::vector<float> m_etaHAD;
	std::vector<float> m_phiHAD;
	std::vector<float> m_AdcEM;
	std::vector<float> m_AdcHAD;
	float m_adcThreshold;

};


#endif
