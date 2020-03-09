///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p2.cxx 
// Implementation file for class MuonCnv_p2
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
#include "muonEvent/MuonCaloEnergyContainer.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p1.h"
#include "RecTPCnv/MuonCnv_p2.h"

typedef ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
typedef ElementLinkVectorCnv_p1<ElementLinkVector<Trk::SegmentCollection> > segmentLinkCnv_t;

// pre-allocate converters
static const P4ImplIPtCotThPhiMCnv_p1   momCnv;
static const ParticleBaseCnv_p1     partBaseCnv;
static const TrackLinkCnv_t         trackCnv;
static const ClusterLinkCnv_t       clusterCnv;
static const segmentLinkCnv_t       segmentCnv;
static const DepositInCaloCnv_p1    depositCnv;


void MuonCnv_p2::persToTrans( const Muon_p2* pers,
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
    trans->set_author (static_cast<MuonParameters::Author>(pers->m_author));
    trans->set_allAuthors (0);
    trans->add_author ( trans->author() );

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

    /// energy loss in calorimeter
    /** retrieve the Muon CaloEnergy container */
    MuonCaloEnergyContainer * caloEnergyContainer = 0;
    if (m_storeGate->retrieve(caloEnergyContainer, m_muonCaloEnergyContainerName).isFailure() || caloEnergyContainer == NULL) {
      msg << MSG::WARNING << "Cannot retrieve MuonCaloEnergyContainer" << m_muonCaloEnergyContainerName
          <<  endmsg;
    } else {
      float deltaE = pers->m_caloEnergyLoss.m_energyDeposit;
      float sigmaM = pers->m_caloEnergyLoss.m_energySigmaMinus;
      float sigmaP = pers->m_caloEnergyLoss.m_energySigmaPlus;
      float sigma  = (sigmaM+sigmaP)/2.0;
      unsigned short elossT = static_cast<unsigned short> (pers->m_caloEnergyLoss.m_energyLossType);
      float caloLR          = pers->m_caloEnergyLoss.m_caloLRLikelihood;
      unsigned short tag    = static_cast<unsigned short>(pers->m_caloEnergyLoss.m_fsrCandidateEnergy);
      float fsrEnergy       = pers->m_caloEnergyLoss.m_fsrCandidateEnergy;

      CaloEnergy * caloEnergy  = new CaloEnergy(deltaE, sigma, sigmaM, sigmaP, elossT, caloLR, tag) ;
      caloEnergy->set_fsrCandidateEnergy ( fsrEnergy );
      std::vector<DepositInCalo> deposits;
      deposits.reserve( pers->m_caloEnergyLoss.m_deposits.size() );
      typedef std::vector<DepositInCalo_p1> Deposits_t;
      for ( Deposits_t::const_iterator
            itr  = pers->m_caloEnergyLoss.m_deposits.begin(),
            iEnd = pers->m_caloEnergyLoss.m_deposits.end();
            itr != iEnd;
            ++itr ) {
         deposits.push_back( DepositInCalo() );
         depositCnv.persToTrans( &*itr, &deposits.back(), msg );
       }
       caloEnergy->set_deposits( deposits );
       caloEnergyContainer->push_back( caloEnergy );
       ElementLink<MuonCaloEnergyContainer> caloEnergyLink;
       caloEnergyLink.toContainedElement(*caloEnergyContainer, caloEnergy);
       trans->setCaloEnergyLink (caloEnergyLink);
    }

//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endmsg;

    return;
}

void MuonCnv_p2::transToPers( const Analysis::Muon* trans, 
			      Muon_p2* pers, 
			      MsgStream& msg ) const
{
  msg << MSG::ERROR << "Analysis::Muon at " << trans << " Persistent Muon_p2 at " << pers << " Cannot write to Muon_p2" << endmsg;
  throw std::runtime_error("Writing to Muon_p2 is not supported - MuonCnv_p2::transToPers(...)");
  return;

}

