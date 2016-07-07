/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L2TAUTOPOFEX_H
#define L2TAUTOPOFEX_H

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "TrigInterfaces/ComboAlgo.h"

/**
 *	@brief Fex for topo start from chains
	@date 30th March 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class L2TauTopoFex : public HLT::ComboAlgo
{
	public:

		L2TauTopoFex(const std::string& name, ISvcLocator* pSvcLocator);
		~L2TauTopoFex();

		//Athena Hooks
		HLT::ErrorCode hltInitialize();
		HLT::ErrorCode hltFinalize();
		HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass);
		HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

	private:
		xAOD::TrigCompositeContainer *m_cont;
		float m_dR;

};
#endif
