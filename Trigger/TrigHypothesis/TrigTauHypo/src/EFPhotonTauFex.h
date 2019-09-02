/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFPHOTONTAUFEX_H
#define EFPHOTONTAUFEX_H

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "TrigInterfaces/ComboAlgo.h"

/**
 *	@brief Fex for photon+tau chains
	@date 21th July 2016
	@author Daniele Zanzi  (daniele.zanzi@cern.ch)
 */
class EFPhotonTauFex : public HLT::ComboAlgo
{
	public:

		EFPhotonTauFex(const std::string& name, ISvcLocator* pSvcLocator);
		~EFPhotonTauFex();

		//Athena Hooks
		HLT::ErrorCode hltInitialize();
		HLT::ErrorCode hltFinalize();
		HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass);
		HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);

	private:
		xAOD::TrigCompositeContainer *m_cont;
		std::vector<float> m_mvis;

};
#endif
