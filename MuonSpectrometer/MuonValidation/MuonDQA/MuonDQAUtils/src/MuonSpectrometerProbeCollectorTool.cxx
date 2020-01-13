/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/MuonSpectrometerProbeCollectorTool.h"

//================ Constructor =================================================
namespace Muon {
  MuonSpectrometerProbeCollectorTool::MuonSpectrometerProbeCollectorTool(const std::string& t,
									 const std::string& n,
									 const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_InsituPerformanceTools("InsituTrackPerformanceTools/TestTool")
  {
    declareInterface<IProbeCollectorTool>(this);

    declareProperty("InnerTrackContainerName",		m_InnerTrackContainerName = "TrackParticleCandidate");
    declareProperty("MSTrackContainerName",		m_MSTrackContainerName = "MuonboyTrackParticles" );
    declareProperty("CombinedMuonTracksContainerName",	m_CombinedMuonTracksContainerName = "StacoMuonCollection");
    declareProperty("RequireTrigger",			m_RequireTrigger = false);
    declareProperty("InsituPerformanceTools",		m_InsituPerformanceTools);
    declareProperty("MuonPtCut", m_muonPtCut=20000.);
  }

  //================ Initialisation =================================================

  StatusCode MuonSpectrometerProbeCollectorTool::initialize()
  {
    ATH_CHECK(AlgTool::initialize());

    /// Getting InsituPerformanceTools
    ATH_CHECK(m_InsituPerformanceTools->initialize());

    ATH_MSG_INFO("initialize() successful");
    return StatusCode::SUCCESS;
  }

  //============================================================================================

  StatusCode MuonSpectrometerProbeCollectorTool::createProbeCollection()
  {
    ATH_MSG_DEBUG("createProbeCollection() for Muon Spectrometer");
	
    /// Record the container of Probe Muons in evtStore
    m_MSProbeTrackContainer = new Rec::TrackParticleContainer();
    ATH_CHECK(evtStore()->record(m_MSProbeTrackContainer,"MuonSpectrometerProbeTracks"));
    ATH_MSG_DEBUG("MuonSpectrometerProbeTracks Container recorded in evtStore.");

    /// Retrieve Inner Tracks
    const Rec::TrackParticleContainer* trackTES=nullptr;
    ATH_CHECK(evtStore()->retrieve( trackTES, m_InnerTrackContainerName));
    if (!trackTES ) {
      ATH_MSG_WARNING("No " << m_InnerTrackContainerName << " container found in TDS"); 
      return StatusCode::FAILURE;
    }  
    ATH_MSG_DEBUG("TrackParticleContainer successfully retrieved");

    /// Retrieve Combined Tracks
    const Analysis::MuonContainer* muonTDS=nullptr;
    ATH_CHECK(evtStore()->retrieve( muonTDS, m_CombinedMuonTracksContainerName));
    if (!muonTDS) {
      ATH_MSG_WARNING("No AOD "<<m_CombinedMuonTracksContainerName<<" container of muons found in TDS"); 
      return StatusCode::FAILURE;
    }  
    ATH_MSG_DEBUG("MuonContainer successfully retrieved");
	
    /// Loop over Combined Muon Tracks
    Analysis::MuonContainer::const_iterator muonItr  = muonTDS->begin();
    Analysis::MuonContainer::const_iterator muonItrE = muonTDS->end();
    for (; muonItr != muonItrE; ++muonItr)
      {
	if (((*muonItr)->pt()>m_muonPtCut) && ((*muonItr)->isCombinedMuon()))
	  {
	    const Rec::TrackParticle *muonTrack	= (*muonItr)->track();
	    if ((m_RequireTrigger==false) || (m_InsituPerformanceTools->isTriggeredMuon(const_cast<Rec::TrackParticle*>(muonTrack))==true))
	      {
		Rec::TrackParticleContainer::const_iterator innerTrackItr  = trackTES->begin();
		Rec::TrackParticleContainer::const_iterator innerTrackItrE = trackTES->end();
		for (; innerTrackItr != innerTrackItrE; ++innerTrackItr) 
		  {
		    const Rec::TrackParticle *innerTrack	= (*innerTrackItr);
		    if (innerTrack->pt()>m_muonPtCut)
		      {
			if (m_InsituPerformanceTools->isZBosonCandidate(const_cast<Rec::TrackParticle*>(innerTrack), const_cast<Rec::TrackParticle*>(muonTrack))==true)
			  {
			    Rec::TrackParticle temp;
			    temp.setPx(innerTrack->px());
			    temp.setPy(innerTrack->py());
			    temp.setPz(innerTrack->pz());
			    temp.setE(innerTrack->e());
			    m_MSProbeTrackContainer->push_back( new Rec::TrackParticle(temp));	
			  }
		      }
		  }
	      }
	  }
      }
    ATH_CHECK(evtStore()->setConst(m_MSProbeTrackContainer)); 
    return StatusCode::SUCCESS;
  }
}//namespace


