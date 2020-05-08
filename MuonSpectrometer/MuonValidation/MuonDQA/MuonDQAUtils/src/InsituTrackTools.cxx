/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDQAUtils/InsituTrackTools.h"
#include "EventKernel/INavigable4Momentum.h"
#include <cmath>

namespace Muon {
  InsituTrackTools::InsituTrackTools(const std::string& t, const std::string& n, const IInterface*  p ):
    AthAlgTool(t,n,p)
  {
    declareInterface<IInsituTrackTools>(this);

    declareProperty("InnerTrackContainerName",		m_InnerTrackContainerName = "TrackParticleCandidate");
    declareProperty("ConeJetContainerName",		m_ConeJetContainerName = "Cone4H1TowerJets");
    declareProperty("m_MaximalMassDifferenceToZBoson",	m_MaximalMassDifferenceToZBoson = 20000.); // MeV
    declareProperty("m_MaximalPTofTracksInCone",	m_MaximalPTofTracksInCone = 15000.); // MeV
    declareProperty("m_MaximalNumberofTracksInCone",	m_MaximalNumberofTracksInCone = 5);
    declareProperty("m_MaximalJetEnergyInCone",		m_MaximalJetEnergyInCone = 25000.); // MeV
		
  }

  bool 	InsituTrackTools::isZBosonCandidate(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    /// Check if these two tracks are not identical
    if (isCloseTrack(track1, track2)==true) return false;

    /// Invariant mass cut
    if (std::abs(getInvariantMass(track1, track2)-91187.6)>m_MaximalMassDifferenceToZBoson) return false;

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
  }

  bool	InsituTrackTools::isTriggeredMuon(INavigable4Momentum */*track1*/)
  {
    return true;
  }

  bool	InsituTrackTools::isTriggeredElectron(INavigable4Momentum */*track1*/)
  {
    return true;
  }

  bool	InsituTrackTools::isElectronCandidate(INavigable4Momentum */*track1*/)
  {
    return false;
  }

  bool 	InsituTrackTools::isCloseTrack(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    if (getDistance(track1, track2)<0.4) return true;
    return false;
  }

  double InsituTrackTools::getDistance(const INavigable4Momentum *track1, const INavigable4Momentum *track2)
  {
    double dphi = std::abs(track1->phi()-track2->phi());
    if (dphi>M_PI)	dphi = std::abs(dphi-2.0*M_PI);
    return std::sqrt(std::pow(track1->eta()-track2->eta(),2)+std::pow(dphi,2));
  }

  bool	InsituTrackTools::getTrackIsolation(const INavigable4Momentum *trackParticle, float &PtIsolation, int &NIsolation)
  {
    const Rec::TrackParticleContainer* trackTES=nullptr;
    StatusCode sc = evtStore()->retrieve(trackTES, m_InnerTrackContainerName);
    if (sc.isFailure() || !trackTES) return false;

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
		
	dr = std::sqrt(std::pow(id_eta-track_eta,2)+std::pow(id_phi-track_phi,2));
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
    const JetCollection* jetTES = nullptr;  //ESD Key
    StatusCode sc = evtStore()->retrieve(jetTES, m_ConeJetContainerName);
    if (sc.isFailure() || !jetTES) return -1.;

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
		
	dr = std::sqrt(std::pow(jet_eta-track_eta,2)+std::pow(jet_phi-track_phi,2));
	if ((0.01<dr) && (dr<0.4)) jet_energy+=(*jetItr)->pt();
      }
	
    return jet_energy;
  }
}//namespace
