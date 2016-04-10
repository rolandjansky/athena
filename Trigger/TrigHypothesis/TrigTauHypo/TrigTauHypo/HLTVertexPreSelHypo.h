/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTVERTEXPRESELHYPO_H
#define HLTVERTEXPRESELHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>

/**
 *	@brief Topological Hypothesis that works on 2 Trigger Elements to determine if 2 TauJet come from the same vertex.
	@date 17th September 2015
	@author Peter McNamara  (peter.charles.mcnamara@cern.ch)
 */
class HLTVertexPreSelHypo : public HLT::HypoAlgo
{

 public:
	
	HLTVertexPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator);
	~HLTVertexPreSelHypo();

	//Athena Hooks
	HLT::ErrorCode hltInitialize();
	HLT::ErrorCode hltFinalize();
	HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);

 private:
	/** Variable to set if we will look for and use vertices in the TauJets */
	bool m_useVertices;
	/** Variable to set if we will look for and use leading track information from the TauJets */
	bool m_useLeadingTrackZ0;
	/** Variable to set if both the sources (vertex or leading track) for comparison must be the same */
	bool m_mustUseSameSource;
	/** Variable to set how close the z0 must be to be considered to come from the same vertex, doesn;t apply when comparing vertices*/
	float m_acceptableZ0Distance;
	/**
		@brief finds the z0 for the leading track of a TauJet
		@param numberOfTracks the number of core tracks in the TauJet
		@param tauJet the taujet object to look in to find the leading track
		@return the z0 of the leading track as a @c float
	*/
	float findLeadingTrackZ0(size_t numberOfTracks, const xAOD::TauJet* tauJet);

	// monitoring
	int m_cutCounter;
	float m_Z0DistanceAll;
	float m_Z0DistanceAccepted;
};
#endif

