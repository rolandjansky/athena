///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#define private public
#define protected public
#include "muonEvent/Muon.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MuonCnv_p6.h"

typedef ElementLinkCnv_p3<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkCnv_p3<ElementLink<MuonCaloEnergyContainer> > caloEnergyLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Trk::SegmentCollection> > segmentLinkCnv_t;

// pre-allocate converters
static P4ImplIPtCotThPhiMCnv_p1   momCnv;
static ParticleBaseCnv_p2     partBaseCnv;
static TrackLinkCnv_t         trackCnv;
static ClusterLinkCnv_t       clusterCnv;
static segmentLinkCnv_t       segmentCnv;
static caloEnergyLinkCnv_t    caloEnergyCnv;

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void MuonCnv_p6::persToTrans( const Muon_p6* pers,
			      Analysis::Muon* trans, 
			      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Muon from persistent state..."
//       << endreq;
  
  // base classes
  momCnv.persToTrans     ( &pers->m_momentum,     &trans->momentumBase(), msg );
  partBaseCnv.persToTrans( &pers->m_particleBase, &trans->particleBase(), msg );

  // element links
  trackCnv.persToTrans( &pers->m_inDetTrackParticle,
                        &trans->m_inDetTrackParticle,
                        msg );

  trackCnv.persToTrans( &pers->m_muonSpectrometerTrackParticle,
                        &trans->m_muonSpectrometerTrackParticle,
                        msg );

  trackCnv.persToTrans( &pers->m_muonExtrapolatedTrackParticle,
			&trans->m_muonExtrapolatedTrackParticle,
			msg );

  trackCnv.persToTrans( &pers->m_innerExtrapolatedTrackParticle,
			&trans->m_innerExtrapolatedTrackParticle,
			msg );

  trackCnv.persToTrans( &pers->m_combinedMuonTrackParticle,
			&trans->m_combinedMuonTrackParticle,
			msg );

  trackCnv.persToTrans( &pers->m_statCombinedMuonTrackParticle,
			&trans->m_statCombinedMuonTrackParticle,
			msg );

  clusterCnv.persToTrans( &pers->m_cluster,
			  &trans->m_cluster,
			  msg );

  segmentCnv.persToTrans( &pers->m_muonSegments,
                          &trans->m_muonSegments,
                          msg );

  caloEnergyCnv.persToTrans( &pers->m_caloEnergyLoss,
                             &trans->m_caloEnergyLoss,
                             msg );

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
    trans->m_author = static_cast<MuonParameters::Author>(pers->m_author);

    // needed ?
    trans->m_hasCombinedMuon = pers->m_hasCombinedMuon;
    trans->m_hasInDetTrackParticle = pers->m_hasInDetTrackParticle;
    trans->m_hasMuonExtrapolatedTrackParticle = pers->m_hasMuonExtrapolatedTrackParticle;
    trans->m_hasCombinedMuonTrackParticle = pers->m_hasCombinedMuonTrackParticle;
    trans->m_hasInnerExtrapolatedTrackParticle = pers->m_hasInnerExtrapolatedTrackParticle;

    // not used
    trans->m_hasCluster = pers->m_hasCluster;

    // chi2 of the track matching
    trans->m_matchChi2 = pers->m_matchChi2;

    // Low Pt muon stuff
    trans->m_associatedEtaDigits = pers->m_associatedEtaDigits;
    trans->m_associatedPhiDigits = pers->m_associatedPhiDigits;

    trans->m_bestMatch = pers->m_bestMatch;
    trans->m_matchNumberDoF = pers->m_matchNumberDoF;
    trans->m_outerMatchNumberDoF = pers->m_outerMatchNumberDoF;

    /* create transient/cached fitquality, with compatibility for data before rel 16
       (that data didn't have ndof stored for segment match chi2) */
    double outerMatchChi2PerNdof = (double) trans->parameter(MuonParameters::segmentChi2OverDoF);
    if (trans->m_outerMatchNumberDoF == 0 &&
	outerMatchChi2PerNdof>0) { // 
      int segmentMatchNdof = 4;
      trans->m_transientOuterMatchChi2 = 
	Trk::FitQuality(segmentMatchNdof*outerMatchChi2PerNdof,segmentMatchNdof);
    } else {
      trans->m_transientOuterMatchChi2 = 
	Trk::FitQuality(pers->m_outerMatchNumberDoF*outerMatchChi2PerNdof,pers->m_outerMatchNumberDoF);
    }

    // this muon is also found by the lowPT reconstruction algorithm
    trans->m_isAlsoFoundByLowPt = pers->m_isAlsoFoundByLowPt;

    // this muon is also found by the Calo Muon ID reconstruction algorithm
    trans->m_isAlsoFoundByCaloMuonId = pers->m_isAlsoFoundByCaloMuonId;

    /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
    trans->m_caloMuonAlsoFoundByMuonReco = pers->m_caloMuonAlsoFoundByMuonReco;

    trans->m_isCorrected = pers->m_isCorrected;

    trans->m_allAuthors  = pers->m_allAuthors;
    trans->m_isMuonBits.set( pers->m_isMuonBits );
    trans->m_isMuonLikelihood = pers->m_isMuonLikelihood;

//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endreq;

  return;
}

void MuonCnv_p6::transToPers( const Analysis::Muon* trans, 
			      Muon_p6* pers, 
			      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Muon..."
//       << endreq;

  // base classes
  momCnv.transToPers     ( &trans->momentumBase(), &pers->m_momentum,     msg );
  partBaseCnv.transToPers( &trans->particleBase(), &pers->m_particleBase, msg );

  // element links
  trackCnv.transToPers( &trans->m_inDetTrackParticle,
			&pers->m_inDetTrackParticle,
			msg );
  trackCnv.transToPers( &trans->m_muonSpectrometerTrackParticle,
                        &pers->m_muonSpectrometerTrackParticle,
                        msg );
  trackCnv.transToPers( &trans->m_muonExtrapolatedTrackParticle,
			&pers->m_muonExtrapolatedTrackParticle,
			msg );
  trackCnv.transToPers( &trans->m_innerExtrapolatedTrackParticle,
			&pers->m_innerExtrapolatedTrackParticle,
			msg );
  trackCnv.transToPers( &trans->m_combinedMuonTrackParticle,
			&pers->m_combinedMuonTrackParticle,
			msg );
  trackCnv.transToPers( &trans->m_statCombinedMuonTrackParticle,
			&pers->m_statCombinedMuonTrackParticle,
			msg );

  clusterCnv.transToPers( &trans->m_cluster, 
			  &pers->m_cluster,
			  msg );

  segmentCnv.transToPers( &trans->m_muonSegments,
                          &pers->m_muonSegments,
                          msg );

  /// energy loss in calorimeter
  caloEnergyCnv.transToPers( &trans->m_caloEnergyLoss,
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

  pers->m_author = trans->m_author;
 
  // needed ? most probably...
  pers->m_hasCombinedMuon = trans->m_hasCombinedMuon;
  pers->m_hasInDetTrackParticle = trans->m_hasInDetTrackParticle;
  pers->m_hasMuonExtrapolatedTrackParticle = trans->m_hasMuonExtrapolatedTrackParticle;
  pers->m_hasCombinedMuonTrackParticle = trans->m_hasCombinedMuonTrackParticle;
  pers->m_hasInnerExtrapolatedTrackParticle = trans->m_hasInnerExtrapolatedTrackParticle;
 
  // not used
  pers->m_hasCluster = trans->m_hasCluster;

  // chi2 of the track matching
  pers->m_matchChi2 = trans->m_matchChi2;

  // Low Pt muon stuff
  pers->m_associatedEtaDigits = trans->m_associatedEtaDigits;
  pers->m_associatedPhiDigits = trans->m_associatedPhiDigits;
  // 

  pers->m_bestMatch      = trans->m_bestMatch;
  pers->m_matchNumberDoF = trans->m_matchNumberDoF;
  pers->m_outerMatchNumberDoF = trans->m_outerMatchNumberDoF;

  /** this muon is also found by the lowPT reconstruction algorithm */
  pers->m_isAlsoFoundByLowPt = trans->m_isAlsoFoundByLowPt;

  /** this muon is also found by Calo Muon ID reconstruction algorithm */
  pers->m_isAlsoFoundByCaloMuonId = trans->m_isAlsoFoundByCaloMuonId;

  /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
  pers->m_caloMuonAlsoFoundByMuonReco = trans->m_caloMuonAlsoFoundByMuonReco;

  pers->m_isCorrected = trans->m_isCorrected;
 
  pers->m_allAuthors  = trans->m_allAuthors;

  pers->m_isMuonBits  = trans->m_isMuonBits.qualityWord();

  pers->m_isMuonLikelihood = trans->m_isMuonLikelihood;

//   msg << MSG::DEBUG << "Created persistent state of Muon [OK]"
//       << endreq;
  return;
}
