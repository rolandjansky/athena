/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Extra documentation (doxygen) in header file
#include <vector> 
#include <cmath>
 
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthContainers/DataVector.h"

#include "TrigTauHypo/HLTVertexPreSelHypo.h"
#include "xAODTracking/VertexContainer.h"

#include "TrigNavStructure/TrigNavStructure.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"


// Constructor
HLTVertexPreSelHypo::HLTVertexPreSelHypo(const std::string& name, ISvcLocator* pSvcLocator):
	HLT::HypoAlgo(name, pSvcLocator)
{
	declareProperty("useVertices", m_useVertices = true);
	declareProperty("useLeadingTrackZ0", m_useLeadingTrackZ0 = true);
	declareProperty("mustUseSameSource", m_mustUseSameSource = false);
	declareProperty("acceptableZ0Distance", m_acceptableZ0Distance = 2);
}

//Destructor
HLTVertexPreSelHypo::~HLTVertexPreSelHypo()
{  
}

//Athena Hooks
HLT::ErrorCode HLTVertexPreSelHypo::hltInitialize()
// ----------------------------------------------------------------------
{
	ATH_MSG_DEBUG("Initialising HLTVertexPreSelHypo");
	ATH_MSG_DEBUG("AcceptableZ0Distance: " << m_acceptableZ0Distance);
	ATH_MSG_DEBUG("useVertices: " << m_useVertices);
	ATH_MSG_DEBUG("mustUseSameSource: " << m_mustUseSameSource);
	ATH_MSG_DEBUG("useLeadingTrackZ0: " << m_useLeadingTrackZ0);
	
	return HLT::OK;
}

HLT::ErrorCode HLTVertexPreSelHypo::hltFinalize()
{
	ATH_MSG_DEBUG("Finalising HLTVertexPreSelHypo"); 
	return HLT::OK;
}

HLT::ErrorCode HLTVertexPreSelHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass)
{
	pass = false;
	HLT::ErrorCode status;
	float z0Jet1 = 0;
	float z0Jet2 = 0;
	
	
	//get trigger Elements to check
	std::vector<HLT::TriggerElement*> triggerElementVector;
	triggerElementVector = HLT::Navigation::getDirectPredecessors(inputTE);
	if(triggerElementVector.size() != 2)
	{
		ATH_MSG_ERROR("Did not get 2 trigger elements as direct predecessors, instead got " << triggerElementVector.size());
		return HLT::BAD_JOB_SETUP;
	}
	
	
	//get the two taujets
	HLT::TriggerElement* triggerElement1 = triggerElementVector.at(0);
	HLT::TriggerElement* triggerElement2 = triggerElementVector.at(1);
	const xAOD::TauJetContainer* tauJetContainer(NULL);
	status = getFeature(triggerElement1, tauJetContainer);
	if(status != HLT::OK)
	{
		ATH_MSG_ERROR("No tauJetContainer found");
		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
	}
	if(tauJetContainer->size() == 0)
	{
		ATH_MSG_ERROR("Empty tauJetContainer");
		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
	}
	//use last one added
	const xAOD::TauJet* tauJet1 = tauJetContainer->back();
	
	status = getFeature(triggerElement2, tauJetContainer);
	if(status != HLT::OK)
	{
		ATH_MSG_ERROR("No tauJetContainer found");
		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
	}
	if(tauJetContainer->size() == 0)
	{
		ATH_MSG_ERROR("Empty tauJetContainer");
		return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
	}
	//use last one added
	const xAOD::TauJet* tauJet2 = tauJetContainer->back();
	
	
	//if using vertices try and use their information
	const xAOD::Vertex* vertex1(NULL);
	const xAOD::Vertex* vertex2(NULL);
	if(m_useVertices)
	{
		try
		{
			vertex1 = tauJet1->vertex();
			z0Jet1 = vertex1->z();
			ATH_MSG_DEBUG("Found Vertex 1, Z0: " << z0Jet1);
		}
		catch(std::exception e)
		{
			//do nothing
		}
		try
		{
			vertex2 = tauJet2->vertex();
			z0Jet2 = vertex2->z();
			ATH_MSG_DEBUG("Found Vertex 2, Z0: " << z0Jet2);
		}
		catch(std::exception e)
		{
			//do nothing
		}
		//compare the vertices
		if(vertex1 != NULL && vertex2 != NULL)
		{
			
			//assumes vertices come from the same source 
			if(vertex1->z() == vertex2->z())
			{
				ATH_MSG_DEBUG("PASSED");
				pass = true;
			}
			return HLT::OK;
		}
		
	}
	
	
	if(m_useLeadingTrackZ0)
	{
		//use the leading track Z0 if the vertex hasn't aleady given this information 
		//Or if we want to use the same source for the z0 for each Taujet
		if(vertex1 == NULL || m_mustUseSameSource)
		{
			size_t numberOfTracks = tauJet1->nTracks();
			if(numberOfTracks == 0)
			{
				ATH_MSG_ERROR("No Core tracks in Taujet, cannot find z0");
				return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
			}
			z0Jet1 = findLeadingTrackZ0(numberOfTracks, tauJet1);
		}
		if(vertex2 == NULL || m_mustUseSameSource)
		{
			size_t numberOfTracks = tauJet2->nTracks();
			if(numberOfTracks == 0)
			{
				ATH_MSG_ERROR("No Core tracks in Taujet, cannot find z0");
				return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
			}
			z0Jet2 = findLeadingTrackZ0(numberOfTracks, tauJet2);
		}
		
		
		//compare
		if(abs(z0Jet1 - z0Jet2) <= m_acceptableZ0Distance)
		{
			ATH_MSG_DEBUG("PASSED");
			pass = true;
		}
		return HLT::OK;
	}

	ATH_MSG_ERROR("Could not find z0 for both Taujets within the current parameters");
	return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);

}

float HLTVertexPreSelHypo::findLeadingTrackZ0(size_t numberOfTracks, const xAOD::TauJet* tauJet)
{
	//search for track with highest pT and use this track's z0
	double highestpT = 0;
	const xAOD::TrackParticle* leadingTrack(NULL);
	for(size_t i = 0; i < numberOfTracks; i++)
	{
		const xAOD::TrackParticle* currentTrack = tauJet->track(i);
		ATH_MSG_DEBUG("Track " << (i + 1) << "  | pT: " << currentTrack->pt());
		if(abs(currentTrack->pt()) > highestpT)
		{
			leadingTrack = currentTrack;
			highestpT = abs(currentTrack->pt());
		}
	}
	float z0 = leadingTrack->z0();
	ATH_MSG_DEBUG("leading track z0: " << z0);
	return z0;
}
