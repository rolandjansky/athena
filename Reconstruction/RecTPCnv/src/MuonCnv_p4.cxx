///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p4.cxx 
// Implementation file for class MuonCnv_p4
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 


// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplIPtCotThPhiMCnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// muonEvent includes
#include "muonEvent/Muon.h"

// RecTPCnv includes
#include "RecTPCnv/MuonCnv_p4.h"

typedef ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<MuonCaloEnergyContainer> > caloEnergyLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Trk::SegmentCollection> > segmentLinkCnv_t;

// pre-allocate converters
static const P4ImplIPtCotThPhiMCnv_p1   momCnv;
static const ParticleBaseCnv_p1     partBaseCnv;
static const TrackLinkCnv_t         trackCnv;
static const ClusterLinkCnv_t       clusterCnv;
static const segmentLinkCnv_t       segmentCnv;
static const caloEnergyLinkCnv_t    caloEnergyCnv;


void MuonCnv_p4::persToTrans( const Muon_p4* pers,
			      Analysis::Muon* trans, 
			      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading Muon from persistent state..."
//       << endmsg;
  
  // base classes
  momCnv.persToTrans     ( &pers->m_momentum,     &trans->momentumBase(), msg );
  partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(), msg );

  // element links
  ElementLink<Rec::TrackParticleContainer> inDetTrackLink;
  trackCnv.persToTrans( &pers->m_inDetTrackParticle,
                        &inDetTrackLink,
                        msg );
  trans->setInDetTrackLink (inDetTrackLink);

  ElementLink<Rec::TrackParticleContainer> muonSpectrometerTrackLink;
  trackCnv.persToTrans( &pers->m_muonSpectrometerTrackParticle,
			&muonSpectrometerTrackLink,
			msg );
  trans->setMuonSpectrometerTrackLink (muonSpectrometerTrackLink);

  ElementLink<Rec::TrackParticleContainer> muonExtrapTrackLink;
  trackCnv.persToTrans( &pers->m_muonExtrapolatedTrackParticle,
			&muonExtrapTrackLink,
			msg );
  trans->setMuonExtrapTrackLink (muonExtrapTrackLink,
                                 pers->m_hasMuonExtrapolatedTrackParticle);

  ElementLink<Rec::TrackParticleContainer> innerExtrapTrackLink;
  trackCnv.persToTrans( &pers->m_innerExtrapolatedTrackParticle,
			&innerExtrapTrackLink,
			msg );
  trans->setInnerExtrapTrackLink (innerExtrapTrackLink);

  ElementLink<Rec::TrackParticleContainer> combinedTrackLink;
  trackCnv.persToTrans( &pers->m_combinedMuonTrackParticle,
			&combinedTrackLink,
			msg );
  trans->setCombinedTrackLink (combinedTrackLink,
                               pers->m_hasCombinedMuonTrackParticle);

  ElementLink<CaloClusterContainer> clusterLink;
  clusterCnv.persToTrans( &pers->m_cluster,
			  &clusterLink,
			  msg );
  trans->setClusterLink (clusterLink);

  ElementLinkVector<Trk::SegmentCollection> muonSegmentLink;
  segmentCnv.persToTrans( &pers->m_muonSegments,
                          &muonSegmentLink,
                          msg );
  trans->setMuonSegmentLink (muonSegmentLink);

  ElementLink<MuonCaloEnergyContainer> caloEnergyLink;
  caloEnergyCnv.persToTrans( &pers->m_caloEnergyLoss,
                             &caloEnergyLink,
                             msg );
  trans->setCaloEnergyLink (caloEnergyLink);

  // muon parameters
    const std::vector<float>& params = pers->m_parameters;
    trans->set_parameter(MuonParameters::etcone10, params[ 0] );
    trans->set_parameter(MuonParameters::etcone20, params[ 1] );
    trans->set_parameter(MuonParameters::etcone30, params[ 2] );
    trans->set_parameter(MuonParameters::etcone40, params[ 3] );
    					      
    trans->set_parameter(MuonParameters::nucone10, params[ 4] );
    trans->set_parameter(MuonParameters::nucone20, params[ 5] );
    trans->set_parameter(MuonParameters::nucone30, params[ 6] );
    trans->set_parameter(MuonParameters::nucone40, params[ 7] );
    
    trans->set_parameter(MuonParameters::ptcone10, params[ 8] );
    trans->set_parameter(MuonParameters::ptcone20, params[ 9] );
    trans->set_parameter(MuonParameters::ptcone30, params[10] );
    trans->set_parameter(MuonParameters::ptcone40, params[11] );

    trans->set_parameter(MuonParameters::segmentDeltaEta,    params[12] );
    trans->set_parameter(MuonParameters::segmentDeltaPhi,    params[13] );
    trans->set_parameter(MuonParameters::segmentChi2OverDoF, params[14] );
    trans->set_parameter(MuonParameters::annBarrel,          params[15] );
    trans->set_parameter(MuonParameters::annEndCap,          params[16] );
    trans->set_parameter(MuonParameters::innAngle,           params[17] );
    trans->set_parameter(MuonParameters::midAngle,           params[18] );

    trans->set_parameter(MuonParameters::t0,                 params[19] );
    trans->set_parameter(MuonParameters::beta,               params[20] );

    // author
    trans->set_author (static_cast<MuonParameters::Author>(pers->m_author));

    // chi2 of the track matching
    trans->set_matchChi2 (pers->m_matchChi2);

    // Low Pt muon stuff
    trans->set_numberOfAssociatedEtaDigits (pers->m_associatedEtaDigits);
    trans->set_numberOfAssociatedPhiDigits (pers->m_associatedPhiDigits);

    trans->set_bestMatch (pers->m_bestMatch);
    trans->set_matchNumberDoF (pers->m_matchNumberDoF);

    // this muon is also found by the lowPT reconstruction algorithm
    trans->set_isAlsoFoundByLowPt (pers->m_isAlsoFoundByLowPt);

    // this muon is also found by the Calo Muon ID reconstruction algorithm
    trans->set_isAlsoFoundByCaloMuonId (pers->m_isAlsoFoundByCaloMuonId);

    /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
    trans->set_caloMuonAlsoFoundByMuonReco (pers->m_caloMuonAlsoFoundByMuonReco);

    trans->set_isCorrected (pers->m_isCorrected);

    trans->set_allAuthors (pers->m_allAuthors);
//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endmsg;

  return;
}

void MuonCnv_p4::transToPers( const Analysis::Muon* trans, 
			      Muon_p4* pers, 
			      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of Muon..."
//       << endmsg;
  msg << MSG::ERROR << "Analysis::Muon at " << trans << " Persistent Muon_p4 at " << pers << " Cannot write to Muon_p4" << endmsg;
  throw std::runtime_error("Writing to Muon_p4 is not supported - MuonCnv_p4::transToPers(...)");
  return;
}
