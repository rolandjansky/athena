/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSpectrometerProbeCollectorTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonDQAUtils/MuonSpectrometerProbeCollectorTool.h"

//================ Constructor =================================================
namespace Muon {
  MuonSpectrometerProbeCollectorTool::MuonSpectrometerProbeCollectorTool(const std::string& t,
									 const std::string& n,
									 const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_log(msgSvc(),n),
    m_InsituPerformanceTools("InsituTrackPerformanceTools/TestTool")
  {
    declareInterface<IProbeCollectorTool>(this);

    declareProperty("InnerTrackContainerName",		m_InnerTrackContainerName = "TrackParticleCandidate");
    declareProperty("MSTrackContainerName",		m_MSTrackContainerName = "MuonboyTrackParticles" );
    declareProperty("CombinedMuonTracksContainerName",	m_CombinedMuonTracksContainerName = "StacoMuonCollection");
    declareProperty("RequireTrigger",			m_RequireTrigger = false);
    declareProperty("InsituPerformanceTools",		m_InsituPerformanceTools);
  }

  //================ Destructor =================================================
  MuonSpectrometerProbeCollectorTool::~MuonSpectrometerProbeCollectorTool(){}

  //================ Initialisation =================================================

  StatusCode MuonSpectrometerProbeCollectorTool::initialize()
  {
    StatusCode sc = AlgTool::initialize();
    m_log.setLevel(outputLevel());
    if (sc.isFailure()) return sc;
	
    m_log << MSG::DEBUG << "Initialize ()" << endreq;
	
    /// histogram location
    sc = service("THistSvc", m_thistSvc);
    if(sc.isFailure() ){
      m_log   << MSG::ERROR
	      << "Unable to retrieve pointer to THistSvc"
	      << endreq;
      return sc;
    }
	
    /// get StoreGate service
    sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure())
      {
	m_log << MSG::FATAL << "StoreGate service not found !" << endreq;
	return StatusCode::FAILURE;
      }

    /// Getting InsituPerformanceTools
    if (m_InsituPerformanceTools.retrieve().isFailure())
      {
	m_log << MSG::FATAL << "My Tool Service not found" << endreq;
	return StatusCode::FAILURE;
      }
    sc = m_InsituPerformanceTools->initialize();
    if (sc.isFailure())
      {
	m_log << MSG::FATAL << "InsituPerformanceTools service not found !" << endreq;
	return StatusCode::FAILURE;
      }
    m_log << MSG::INFO << "initialize() successful in " << name() << endreq;
    return StatusCode::SUCCESS;
  }

  //================ Finalisation =================================================

  StatusCode MuonSpectrometerProbeCollectorTool::finalize()
  {
    m_log << MSG::DEBUG << "Finalize ()" << endreq;
    StatusCode sc = AlgTool::finalize();
    return sc;
  }

  //============================================================================================

  StatusCode MuonSpectrometerProbeCollectorTool::createProbeCollection()
  {
    m_log << MSG::DEBUG << "createProbeCollection() for Muon Spectrometer" << endreq;
	
    StatusCode sc = StatusCode::SUCCESS;
    /// Record the container of Probe Muons in StoreGate
    m_MSProbeTrackContainer = new Rec::TrackParticleContainer();
    sc = m_storeGate->record(m_MSProbeTrackContainer,"MuonSpectrometerProbeTracks");
    if (sc.isFailure())  
      {
	m_log << MSG::ERROR << "Unable to record MuonSpectrometerProbeTracks in StoreGate" 
	      << endreq;
	return sc;
      } else
	m_log << MSG::DEBUG << "MuonSpectrometerProbeTracks Container recorded in StoreGate." << endreq;

	
    /// Retrieve Inner Tracks
    const Rec::TrackParticleContainer* trackTES=0;
    sc=m_storeGate->retrieve( trackTES, m_InnerTrackContainerName);
    if( sc.isFailure()  ||  !trackTES ) 
      {
	m_log << MSG::WARNING	<< "No " << m_InnerTrackContainerName << " container found in TDS"	<< endreq; 
	return StatusCode::FAILURE;
      }  
    m_log << MSG::DEBUG << "TrackParticleContainer successfully retrieved" << endreq;

    /// Retrieve Combined Tracks
    const Analysis::MuonContainer* muonTDS=0;
    sc=m_storeGate->retrieve( muonTDS, m_CombinedMuonTracksContainerName);
    if( sc.isFailure()  ||  !muonTDS ) 
      {
	m_log << MSG::WARNING	<< "No AOD "<<m_CombinedMuonTracksContainerName<<" container of muons found in TDS"<< endreq; 
	return StatusCode::FAILURE;
      }  
    m_log << MSG::DEBUG << "MuonContainer successfully retrieved" << endreq;
	
    /// Loop over Combined Muon Tracks
    Analysis::MuonContainer::const_iterator muonItr  = muonTDS->begin();
    Analysis::MuonContainer::const_iterator muonItrE = muonTDS->end();
    for (; muonItr != muonItrE; ++muonItr)
      {
	if (((*muonItr)->pt()>20000.) && ((*muonItr)->isCombinedMuon()))
	  {
	    const Rec::TrackParticle *muonTrack	= (*muonItr)->track();
	    if ((m_RequireTrigger==false) || (m_InsituPerformanceTools->isTriggeredMuon(const_cast<Rec::TrackParticle*>(muonTrack))==true))
	      {
		Rec::TrackParticleContainer::const_iterator innerTrackItr  = trackTES->begin();
		Rec::TrackParticleContainer::const_iterator innerTrackItrE = trackTES->end();
		for (; innerTrackItr != innerTrackItrE; ++innerTrackItr) 
		  {
		    const Rec::TrackParticle *innerTrack	= (*innerTrackItr);
		    if (fabs(innerTrack->pt())>20000.)
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
    sc = m_storeGate->setConst(m_MSProbeTrackContainer);
    if( sc.isFailure()) 
      {
	m_log << MSG::WARNING	<< "set const failed"<< endreq; 
	return StatusCode::FAILURE;
      }  
    return StatusCode::SUCCESS;
  }
}//namespace


