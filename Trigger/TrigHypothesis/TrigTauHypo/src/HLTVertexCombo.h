/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTVERTEXCOMBO_H
#define HLTVERTEXCOMBO_H

#include "TrigInterfaces/ComboAlgo.h"

/**
 *	@brief Combo Algorithm to combine 2 TEs ready for vertex based selection
	@date 20th November 2015
	@author Peter McNamara  (peter.charles.mcnamara@cern.ch)
 */
class HLTVertexCombo : public HLT::ComboAlgo
{
	public:

		HLTVertexCombo(const std::string& name, ISvcLocator* pSvcLocator);
		~HLTVertexCombo();

		//Athena Hooks
		HLT::ErrorCode hltInitialize();
		HLT::ErrorCode hltFinalize();
		HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass);
		HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
};
#endif