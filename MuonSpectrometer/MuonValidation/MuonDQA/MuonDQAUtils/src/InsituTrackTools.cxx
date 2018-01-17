/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InsituTrackTools.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuonDQAUtils/InsituTrackTools.h"
#include "EventKernel/INavigable4Momentum.h" 

//================ Constructor =================================================
namespace Muon {
  InsituTrackTools::InsituTrackTools(const std::string& t, const std::string& n, const IInterface*  p ):
    AthAlgTool(t,n,p),
    m_log(msgSvc(),n)
  {
    declareInterface<IInsituTrackTools>(this);

    declareProperty("InnerTrackContainerName",		m_InnerTrackContainerName = "TrackParticleCandidate");
    declareProperty("ConeJetContainerName",		m_ConeJetContainerName = "Cone4H1TowerJets");
    declareProperty("m_MaximalMassDifferenceToZBoson",	m_MaximalMassDifferenceToZBoson = 20000.); // MeV
    declareProperty("m_MaximalPTofTracksInCone",	m_MaximalPTofTracksInCone = 15000.); // MeV
    declareProperty("m_MaximalNumberofTracksInCone",	m_MaximalNumberofTracksInCone = 5);
    declareProperty("m_MaximalJetEnergyInCone",		m_MaximalJetEnergyInCone = 25000.); // MeV
		
  }

  //================ Destructor =================================================

  InsituTrackTools::~InsituTrackTools(){}

  //================ Initialisation =================================================

  StatusCode InsituTrackTools::initialize()
  {
    StatusCode sc = AlgTool::initialize();
    m_log.setLevel(msgLevel());
    if (sc.isFailure()) return sc;
	
    /// histogram location
    sc = service("THistSvc", m_thistSvc);
    if(sc.isFailure() ){
      m_log   << MSG::ERROR
	      << "Unable to retrieve pointer to THistSvc"
	      << endmsg;
      return sc;
    }
	
    /// get StoreGate service
    sc = service("StoreGateSvc",m_storeGate);
    if (sc.isFailure())
      {
	m_log << MSG::FATAL << "StoreGate service not found !" << endmsg;
	return StatusCode::FAILURE;
      }
	
    m_log << MSG::INFO << "initialize() successful in " << name() << endmsg;
    return StatusCode::SUCCESS;
  }

  //================ Finalisation =================================================

  StatusCode InsituTrackTools::finalize()
  {
    StatusCode sc = AlgTool::finalize();
    return sc;
  }

  //============================================================================================


  bool 	InsituTrackTools::isZBosonCandidate(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    /// Check if these two tracks are not identical
    if (isCloseTrack(track1, track2)==true) return false;

    /// Invariant mass cut
    if (fabs(getInvariantMass(track1, track2)-91187.6)>m_MaximalMassDifferenceToZBoson) return false;

    if ((isIsolatedTrack(track1)==true) && (isIsolatedTrack(track2)==true)) return true;
    return false;
  }

  bool	InsituTrackTools::isIsolatedTrack(const INavigable4Momentum *track1)
  {
    float	TrackPTIsolation1 = 0.0;
    int	NIsolation1 = 0;
    getTrackIsolation(track1, TrackPTIsolation1, NIsolation1);
	
    if ((TrackPTIsolation1<m_MaximalPTofTracksInCone) && (NIsolation1<m_MaximalNumberofTracksInCone)) return true;
    return false;
  }

  bool	InsituTrackTools::isIsolatedMuon(const INavigable4Momentum *track1)
  {
    float	JetIsolation1 = getJetIsolation(track1);
    float	TrackPTIsolation1 = 0.0;
    int	NIsolation1 = 0;
    getTrackIsolation(track1, TrackPTIsolation1, NIsolation1);
	
    if ((JetIsolation1<m_MaximalJetEnergyInCone) && (TrackPTIsolation1<m_MaximalPTofTracksInCone) && (NIsolation1<m_MaximalNumberofTracksInCone)) return true;
    return false;
  }

  double InsituTrackTools::getInvariantMass(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    TLorentzVector tvec1;
    TLorentzVector tvec2;
    tvec1.SetPtEtaPhiM(track1->pt(), track1->eta(), track1->phi(), 105.65);
    tvec2.SetPtEtaPhiM(track2->pt(), track2->eta(), track2->phi(), 105.65);
    return (tvec1+tvec2).M();
    
    //return 0.0;
    
  }

  bool	InsituTrackTools::isTriggeredMuon(INavigable4Momentum */*track1*/)
  {
    // track1 = 0;
    return true;
  }

  bool	InsituTrackTools::isTriggeredElectron(INavigable4Momentum */*track1*/)
  {
    // track1 = 0;
    return true;
  }

  bool	InsituTrackTools::isElectronCandidate(INavigable4Momentum */*track1*/)
  {
    // track1 = 0;
    return false;
  }

  bool 	InsituTrackTools::isCloseTrack(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    if (getDistance(track1, track2)<0.4) return true;
    return false;
  }

  double InsituTrackTools::getDistance(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    double dphi = fabs(track1->phi()-track2->phi());
    double pi = 3.14159265;
    if (dphi>pi)	dphi = fabs(dphi-2.0*pi);
    return sqrt(pow(track1->eta()-track2->eta(),2)+pow(dphi,2));
  }

  bool	InsituTrackTools::getTrackIsolation(const INavigable4Momentum *trackParticle, float &PtIsolation, int &NIsolation)
  {
    StatusCode sc = StatusCode::SUCCESS;
	
    const Rec::TrackParticleContainer* trackTES=0;
    sc=m_storeGate->retrieve( trackTES, m_InnerTrackContainerName);
    if( sc.isFailure()  ||  !trackTES ) 	return false;

    NIsolation	= 0;
    PtIsolation	= 0.0;

    double	track_eta = trackParticle->eta();
    double	track_phi = trackParticle->phi();
  
    double	id_eta = 0.0;
    double	id_phi = 0.0;
    double	dr  = 0.0;
    /// iterators over the container 
    Rec::TrackParticleContainer::const_iterator trackItr  = trackTES->begin();
    Rec::TrackParticleContainer::const_iterator trackItrE = trackTES->end();
    for (; trackItr != trackItrE; ++trackItr) 
      {

	id_eta = (*trackItr)->eta();
	id_phi = (*trackItr)->phi();
		
	dr = sqrt(pow(id_eta-track_eta,2)+pow(id_phi-track_phi,2));
	if ((0.01<dr) && (dr<0.4))
	  {
	    NIsolation++;
	    PtIsolation+=(*trackItr)->pt();
	  } 
      }
	
    return true;
  }

  float InsituTrackTools::getTrackPtIsolation(INavigable4Momentum *trackParticle)
  {
    float	PtIsolation;
    int	NIsolation;
    if (getTrackIsolation(trackParticle, PtIsolation, NIsolation)==true) return PtIsolation;
    return -1.0;
  }

  int InsituTrackTools::getNTrackIsolation(INavigable4Momentum *trackParticle)
  {
    float	PtIsolation;
    int	NIsolation;
    if (getTrackIsolation(trackParticle, PtIsolation, NIsolation)==true) return NIsolation;
    return -1;
  }


  float InsituTrackTools::getJetIsolation(const INavigable4Momentum *trackParticle)
  {
    StatusCode sc = StatusCode::SUCCESS;
	
    //const ParticleJetContainer* jetTES;
    const JetCollection* jetTES = nullptr;  //ESD Key

    sc=m_storeGate->retrieve( jetTES, m_ConeJetContainerName);
    if( sc.isFailure()  ||  !jetTES ) 
      {
	return -1.0;
      }  

    double	track_eta = trackParticle->eta();
    double	track_phi = trackParticle->phi();
 
    double	jet_energy = 0.0; 
    double	jet_eta = 0.0;
    double	jet_phi = 0.0;
    double	dr  = 0.0;
	
    /// iterators over the container 
    JetCollection::const_iterator jetItr  = jetTES->begin();
    JetCollection::const_iterator jetItrE = jetTES->end();
	
    for(; jetItr != jetItrE; ++jetItr) 
      {
	jet_eta = (*jetItr)->eta();
	jet_phi = (*jetItr)->phi();
		
	dr = sqrt(pow(jet_eta-track_eta,2)+pow(jet_phi-track_phi,2));
	if ((0.01<dr) && (dr<0.4)) jet_energy+=(*jetItr)->pt();
      }
	
    return jet_energy;
  }
}//namespace
