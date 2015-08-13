///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p3.cxx 
// Implementation file for class MuonCnv_p3
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
#define private public
#define protected public
#include "muonEvent/Muon.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/MuonCnv_p3.h"

typedef ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<MuonCaloEnergyContainer> > caloEnergyLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Trk::SegmentCollection> > segmentLinkCnv_t;

// pre-allocate converters
static P4ImplIPtCotThPhiMCnv_p1   momCnv;
static ParticleBaseCnv_p1     partBaseCnv;
static TrackLinkCnv_t         trackCnv;
static ClusterLinkCnv_t       clusterCnv;
static segmentLinkCnv_t       segmentCnv;
static caloEnergyLinkCnv_t    caloEnergyCnv;

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void MuonCnv_p3::persToTrans( const Muon_p3* pers,
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
    					      
    trans->set_parameter(MuonParameters::nucone10, params[ 8] );
    trans->set_parameter(MuonParameters::nucone20, params[ 9] );
    trans->set_parameter(MuonParameters::nucone30, params[10] );
    trans->set_parameter(MuonParameters::nucone40, params[11] );
    
    trans->set_parameter(MuonParameters::segmentDeltaEta,    params[16] );
    trans->set_parameter(MuonParameters::segmentDeltaPhi,    params[17] );
    trans->set_parameter(MuonParameters::segmentChi2OverDoF, params[18] );
    trans->set_parameter(MuonParameters::annBarrel,          params[19] );
    trans->set_parameter(MuonParameters::annEndCap,          params[20] );
    trans->set_parameter(MuonParameters::innAngle,           params[21] );
    trans->set_parameter(MuonParameters::midAngle,           params[22] );

    trans->set_parameter(MuonParameters::t0,                 params[23] );
    trans->set_parameter(MuonParameters::beta,               params[24] );

    // author
    trans->m_author = static_cast<MuonParameters::Author>(pers->m_author);
    trans->m_allAuthors = 0;
    trans->add_author ( trans->m_author );

    // needed ?
    trans->m_hasCombinedMuon = pers->m_hasCombinedMuon;
    trans->m_hasInDetTrackParticle = pers->m_hasInDetTrackParticle;
    trans->m_hasMuonExtrapolatedTrackParticle = pers->m_hasMuonExtrapolatedTrackParticle;
    trans->m_hasCombinedMuonTrackParticle = pers->m_hasCombinedMuonTrackParticle;
  

    // not used
    trans->m_hasCluster = pers->m_hasCluster;

    // chi2 of the track matching
    trans->m_matchChi2 = pers->m_matchChi2;

    // Low Pt muon stuff
    trans->m_associatedEtaDigits = pers->m_associatedEtaDigits;
    trans->m_associatedPhiDigits = pers->m_associatedPhiDigits;

    trans->m_bestMatch = pers->m_bestMatch;
    trans->m_matchNumberDoF = pers->m_matchNumberDoF;

    // this muon is also found by the lowPT reconstruction algorithm
    trans->m_isAlsoFoundByLowPt = pers->m_isAlsoFoundByLowPt;

    // this muon is also found by the Calo Muon ID reconstruction algorithm
    trans->m_isAlsoFoundByCaloMuonId = pers->m_isAlsoFoundByCaloMuonId;

    /** this calo muon is also reconstructed by one of the standard muon reco algorithms */
    trans->m_caloMuonAlsoFoundByMuonReco = pers->m_caloMuonAlsoFoundByMuonReco;

    trans->m_isCorrected = pers->m_isCorrected;

//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endreq;

  return;
}

void MuonCnv_p3::transToPers( const Analysis::Muon* trans, 
			      Muon_p3* pers, 
			      MsgStream& msg ) 
{

  msg << MSG::ERROR << "Analysis::Muon at " << trans << " Persistent Muon_p3 at " << pers << " Cannot write to Muon_p3" << endreq;
  throw std::runtime_error("Writing to Muon_p3 is not supported - MuonCnv_p3::transToPers(...)");
  return;

}
