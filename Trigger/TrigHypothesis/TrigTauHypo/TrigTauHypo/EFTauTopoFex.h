/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFTAUTOPOFEX_H
#define EFTAUTOPOFEX_H

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "TrigInterfaces/ComboAlgo.h"

/**
 *	@brief Fex for EF dR cut
	@date 17th August 2016
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

	private:
		xAOD::TrigCompositeContainer *m_cont;
		std::vector<float> m_dR;

};
#endif
