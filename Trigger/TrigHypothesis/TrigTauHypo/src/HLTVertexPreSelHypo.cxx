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

        declareMonitoredVariable("CutCounter",        	m_cutCounter=0);
        declareMonitoredVariable("Z0DistanceAccepted",  m_Z0DistanceAccepted=-10.);
	declareMonitoredVariable("Z0DistanceAll",  	m_Z0DistanceAll=-10.);
}

//Destructor
HLTVertexPreSelHypo::~HLTVertexPreSelHypo()
{  
}

//Athena Hooks
HLT::ErrorCode HLTVertexPreSelHypo::hltInitialize()
// ----------------------------------------------------------------------
{

	msg() << MSG::INFO << "in HLTVertexPreSelHypo initialize()" << endmsg;
        msg() << MSG::INFO << " REGTEST: HLTVertexPreSelHypo will cut on "<<endmsg;
        msg() << MSG::INFO << " REGTEST: param acceptableZ0Distance " << m_acceptableZ0Distance <<endmsg;
        msg() << MSG::INFO << " REGTEST: param useVertices " << m_useVertices <<endmsg;
        msg() << MSG::INFO << " REGTEST: param mustUseSameSource " << m_mustUseSameSource <<endmsg;
        msg() << MSG::INFO << " REGTEST: param useLeadingTrackZ0 " << m_useLeadingTrackZ0 <<endmsg;
	
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
	
	m_cutCounter = 0;
	m_Z0DistanceAll = -10.;
	m_Z0DistanceAccepted = -10.;
	
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
	if(status != HLT::OK || tauJetContainer==NULL)
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
	if(status != HLT::OK || tauJetContainer==NULL)
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
        m_cutCounter++;	

        // ifat least one of the taus has no tracks, pass this hypo
        size_t numberOfTracks1 = tauJet1->nTracks();
	size_t numberOfTracks2 = tauJet2->nTracks();
        if(numberOfTracks1==0 || numberOfTracks2==0){
            ATH_MSG_DEBUG("One of the taus has no core tracks. Will not apply selection");
            pass = true;
            return HLT::OK;
        }

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
			m_cutCounter++;
			//assumes vertices come from the same source 
			if(vertex1->z() == vertex2->z())
			{
				ATH_MSG_DEBUG("PASSED");
				pass = true;
				m_cutCounter++;
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
				ATH_MSG_DEBUG("No Core tracks in Taujet, cannot find z0 for tau with pt "<<tauJet1->pt());
				//return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
				//ATR-15473: becuase of ATR-12851, 0trk candidates with highpt are not rejected. Events with these taus are accepted here
				pass = true;
				return HLT::OK;
			}
			z0Jet1 = findLeadingTrackZ0(numberOfTracks, tauJet1);
		}
		if(vertex2 == NULL || m_mustUseSameSource)
		{
			size_t numberOfTracks = tauJet2->nTracks();
			if(numberOfTracks == 0)
			{
				ATH_MSG_DEBUG("No Core tracks in Taujet, cannot find z0 for tau with pt "<<tauJet2->pt());
				//return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
                                pass = true;
                                return HLT::OK;
			}
			z0Jet2 = findLeadingTrackZ0(numberOfTracks, tauJet2);
		}
		
		m_cutCounter++;
		//compare
		m_Z0DistanceAll = fabs(z0Jet1 - z0Jet2);
		if(fabs(z0Jet1 - z0Jet2) <= m_acceptableZ0Distance)
		{
			ATH_MSG_DEBUG("PASSED");
			pass = true;
			m_cutCounter++;
			m_Z0DistanceAccepted = m_Z0DistanceAll;
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
		const xAOD::TrackParticle* currentTrack = 0;
		#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
		currentTrack = tauJet->track(i);
		#else
		currentTrack = tauJet->track(i)->track();
		#endif
		ATH_MSG_DEBUG("Track " << (i + 1) << "  | pT: " << currentTrack->pt());
		if(fabs(currentTrack->pt()) > highestpT)
		{
			leadingTrack = currentTrack;
			highestpT = fabs(currentTrack->pt());
		}
	}
	float z0 = leadingTrack->z0();
	ATH_MSG_DEBUG("leading track z0: " << z0);
	return z0;
}
