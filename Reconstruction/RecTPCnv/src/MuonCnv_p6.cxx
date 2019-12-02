///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p6.cxx 
// Implementation file for class MuonCnv_p6
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 


// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplIPtCotThPhiMCnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"

// muonEvent includes
#include "muonEvent/Muon.h"

// RecTPCnv includes
#include "RecTPCnv/MuonCnv_p6.h"

typedef ElementLinkCnv_p3<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkCnv_p3<ElementLink<MuonCaloEnergyContainer> > caloEnergyLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Trk::SegmentCollection> > segmentLinkCnv_t;

// pre-allocate converters
static const P4ImplIPtCotThPhiMCnv_p1   momCnv;
static const ParticleBaseCnv_p2     partBaseCnv;
static const TrackLinkCnv_t         trackCnv;
static const ClusterLinkCnv_t       clusterCnv;
static const segmentLinkCnv_t       segmentCnv;
static const caloEnergyLinkCnv_t    caloEnergyCnv;


void MuonCnv_p6::persToTrans( const Muon_p6* pers,
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

  ElementLink<Rec::TrackParticleContainer> statCombinedTrackLink;
  trackCnv.persToTrans( &pers->m_statCombinedMuonTrackParticle,
			&statCombinedTrackLink,
			msg );
  trans->setStatCombinedTrackLink (statCombinedTrackLink);

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
    if (params.size() >= 24) {
      trans->set_parameter(MuonParameters::spectrometerFieldIntegral,       params[21] );
      trans->set_parameter(MuonParameters::scatteringCurvatureSignificance, params[22] );
      trans->set_parameter(MuonParameters::scatteringNeighbourSignificance, params[23] );
      if (params.size() >= 25) {
	trans->set_parameter(MuonParameters::momentumBalanceSignificance,     params[24] ); // added muonEvent-05-05-26
	trans->set_parameter(MuonParameters::etconeNoEm10, params[ 25] ); // added muonEvent-05-05-30
	trans->set_parameter(MuonParameters::etconeNoEm20, params[ 26] );
	trans->set_parameter(MuonParameters::etconeNoEm30, params[ 27] );
	trans->set_parameter(MuonParameters::etconeNoEm40, params[ 28] );
      }
    }

    // author
    trans->set_author (static_cast<MuonParameters::Author>(pers->m_author));

    // chi2 of the track matching
    trans->set_matchChi2 (pers->m_matchChi2);

    // Low Pt muon stuff
    trans->set_numberOfAssociatedEtaDigits (pers->m_associatedEtaDigits);
    trans->set_numberOfAssociatedPhiDigits (pers->m_associatedPhiDigits);

    trans->set_bestMatch (pers->m_bestMatch);
    trans->set_matchNumberDoF (pers->m_matchNumberDoF);

    double outerMatchChi2PerNdof = (double) trans->parameter(MuonParameters::segmentChi2OverDoF);
    if (pers->m_outerMatchNumberDoF == 0 &&
	outerMatchChi2PerNdof>0)
    {
      int segmentMatchNdof = 4;
      double chi2 = segmentMatchNdof * outerMatchChi2PerNdof;
      trans->set_outerMatchChi2 (chi2, segmentMatchNdof);
      trans->set_outerMatchChi2 (0, 0);
      trans->set_parameter(MuonParameters::segmentChi2OverDoF, outerMatchChi2PerNdof);
    }
    else {
      double chi2 = pers->m_outerMatchNumberDoF * outerMatchChi2PerNdof;
      trans->set_outerMatchChi2 (chi2, pers->m_outerMatchNumberDoF);
    }

    // this muon is also found by the lowPT reconstruction algorithm
    trans->set_isAlsoFoundByLowPt (pers->m_isAlsoFoundByLowPt);

    // this muon is also found by the Calo Muon ID reconstruction algorithm
    trans->set_isAlsoFoundByCaloMuonId (pers->m_isAlsoFoundByCaloMuonId);

    /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
    trans->set_caloMuonAlsoFoundByMuonReco (pers->m_caloMuonAlsoFoundByMuonReco);

    trans->set_isCorrected (pers->m_isCorrected);

    trans->set_allAuthors (pers->m_allAuthors);
    trans->set_isMuonBits ( pers->m_isMuonBits );
    trans->set_isMuonLikelihood (pers->m_isMuonLikelihood);

//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endmsg;

  return;
}

void MuonCnv_p6::transToPers( const Analysis::Muon* trans, 
			      Muon_p6* pers, 
			      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of Muon..."
//       << endmsg;

  // base classes
  momCnv.transToPers     ( &trans->momentumBase(), &pers->m_momentum,     msg );
  partBaseCnv.transToPers( &trans->particleBase(), &pers->m_particleBase, msg );

  // element links
  trackCnv.transToPers( &trans->inDetTrackLink(),
			&pers->m_inDetTrackParticle,
			msg );
  trackCnv.transToPers( &trans->muonSpectrometerTrackLink(),
                        &pers->m_muonSpectrometerTrackParticle,
                        msg );
  trackCnv.transToPers( &trans->muonExtrapTrackLink(),
			&pers->m_muonExtrapolatedTrackParticle,
			msg );
  trackCnv.transToPers( &trans->innerExtrapTrackLink(),
			&pers->m_innerExtrapolatedTrackParticle,
			msg );
  trackCnv.transToPers( &trans->combinedTrackLink(),
			&pers->m_combinedMuonTrackParticle,
			msg );
  trackCnv.transToPers( &trans->statCombinedTrackLink(),
			&pers->m_statCombinedMuonTrackParticle,
			msg );

  clusterCnv.transToPers( &trans->clusterLink(),
			  &pers->m_cluster,
			  msg );

  segmentCnv.transToPers( &trans->muonSegmentLink(),
                          &pers->m_muonSegments,
                          msg );

  /// energy loss in calorimeter
  caloEnergyCnv.transToPers( &trans->caloEnergyLink(),
                             &pers ->m_caloEnergyLoss,
                             msg );

  // muon parameters
  std::vector<float>& params = pers->m_parameters;
  params.resize( 29 );
  params[ 0] = trans->parameter(MuonParameters::etcone10);
  params[ 1] = trans->parameter(MuonParameters::etcone20);
  params[ 2] = trans->parameter(MuonParameters::etcone30);
  params[ 3] = trans->parameter(MuonParameters::etcone40);
		
  params[ 4] = trans->parameter(MuonParameters::nucone10);
  params[ 5] = trans->parameter(MuonParameters::nucone20);
  params[ 6] = trans->parameter(MuonParameters::nucone30);
  params[ 7] = trans->parameter(MuonParameters::nucone40);

  params[ 8] = trans->parameter(MuonParameters::ptcone10);
  params[ 9] = trans->parameter(MuonParameters::ptcone20);
  params[10] = trans->parameter(MuonParameters::ptcone30);
  params[11] = trans->parameter(MuonParameters::ptcone40);

  params[12] = trans->parameter(MuonParameters::segmentDeltaEta);
  params[13] = trans->parameter(MuonParameters::segmentDeltaPhi);
  params[14] = trans->parameter(MuonParameters::segmentChi2OverDoF);
  params[15] = trans->parameter(MuonParameters::annBarrel);
  params[16] = trans->parameter(MuonParameters::annEndCap);
  params[17] = trans->parameter(MuonParameters::innAngle);
  params[18] = trans->parameter(MuonParameters::midAngle);

  params[19] = trans->parameter(MuonParameters::t0);
  params[20] = trans->parameter(MuonParameters::beta);
  params[21] = trans->parameter(MuonParameters::spectrometerFieldIntegral);
  params[22] = trans->parameter(MuonParameters::scatteringCurvatureSignificance);
  params[23] = trans->parameter(MuonParameters::scatteringNeighbourSignificance);
  params[24] = trans->parameter(MuonParameters::momentumBalanceSignificance);

  params[25] = trans->parameter(MuonParameters::etconeNoEm10);
  params[26] = trans->parameter(MuonParameters::etconeNoEm20);
  params[27] = trans->parameter(MuonParameters::etconeNoEm30);
  params[28] = trans->parameter(MuonParameters::etconeNoEm40);

  pers->m_author = trans->author();
 
  // needed ? most probably...
  pers->m_hasCombinedMuon = false;
  pers->m_hasInDetTrackParticle = false;
  pers->m_hasMuonExtrapolatedTrackParticle = trans->hasMuonExtrapolatedTrackParticleFlag();
  pers->m_hasCombinedMuonTrackParticle = trans->hasCombinedMuonTrackParticleFlag();
  pers->m_hasInnerExtrapolatedTrackParticle = false;
 
  // not used
  pers->m_hasCluster = false;

  // chi2 of the track matching
  pers->m_matchChi2 = trans->matchChi2();

  // Low Pt muon stuff
  pers->m_associatedEtaDigits = trans->associatedEtaDigits();
  pers->m_associatedPhiDigits = trans->associatedPhiDigits();
  // 

  pers->m_bestMatch      = trans->bestMatch();
  pers->m_matchNumberDoF = trans->matchNumberDoF();
  pers->m_outerMatchNumberDoF = trans->outerMatchNumberDoF();

  /** this muon is also found by the lowPT reconstruction algorithm */
  pers->m_isAlsoFoundByLowPt = trans->alsoFoundByLowPt();

  /** this muon is also found by Calo Muon ID reconstruction algorithm */
  pers->m_isAlsoFoundByCaloMuonId = trans->alsoFoundByCaloMuonId();

  /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
  pers->m_caloMuonAlsoFoundByMuonReco = trans->caloMuonAlsoFoundByMuonReco();

  pers->m_isCorrected = trans->isCorrected();
 
  pers->m_allAuthors  = trans->allAuthors();

  pers->m_isMuonBits  = trans->isMuon().qualityWord();

  pers->m_isMuonLikelihood = trans->isMuonLikelihood();

//   msg << MSG::DEBUG << "Created persistent state of Muon [OK]"
//       << endmsg;
  return;
}
