///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p1.cxx 
// Implementation file for class MuonCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplIPtCotThPhiMCnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// muonEvent includes
#define private public
#define protected public
#include "muonEvent/Muon.h"
#include "muonEvent/MuonCaloEnergyContainer.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p1.h"
#include "RecTPCnv/MuonCnv_p1.h"

typedef ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > TrackLinkCnv_t;
typedef ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;

// pre-allocate converters
static P4ImplIPtCotThPhiMCnv_p1   momCnv;
static ParticleBaseCnv_p1     partBaseCnv;
static TrackLinkCnv_t         trackCnv;
static ClusterLinkCnv_t       clusterCnv;
static DepositInCaloCnv_p1    depositCnv;

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void MuonCnv_p1::persToTrans( const Muon_p1* pers,
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

    // author
    trans->m_author = static_cast<MuonParameters::Author>(pers->m_author);
    trans->m_allAuthors = 0;
    trans->add_author ( trans->m_author );

    // needed ?
    trans->m_hasCombinedMuon = pers->m_hasCombinedMuon;
    trans->m_hasInDetTrackParticle = pers->m_hasInDetTrackParticle;
    trans->m_hasMuonExtrapolatedTrackParticle = pers->m_hasMuonExtrapolatedTrackParticle;
    trans->m_hasMuonExtrapolatedTrackParticle = pers->m_hasMuonExtrapolatedTrackParticle;
    trans->m_hasCombinedMuonTrackParticle = pers->m_hasCombinedMuonTrackParticle;

    //  cluster link
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

    /// energy loss in calorimeter

    /** retrieve the Muon CaloEnergy container */
    MuonCaloEnergyContainer * caloEnergyContainer = 0;
    if (m_storeGate->retrieve(caloEnergyContainer, m_muonCaloEnergyContainerName).isFailure() || caloEnergyContainer == NULL) {
      msg << MSG::WARNING << "Cannot retrieve MuonCaloEnergyContainer" << m_muonCaloEnergyContainerName 
          <<  endreq;
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
       trans->m_caloEnergyLoss.toContainedElement(*caloEnergyContainer, const_cast<CaloEnergy*>(caloEnergy));
    } 

//   msg << MSG::DEBUG << "Loaded Muon from persistent state [OK]"
//       << endreq;

    return;
}

void MuonCnv_p1::transToPers( const Analysis::Muon* trans, 
			      Muon_p1* pers, 
			      MsgStream& msg ) 
{
  msg << MSG::ERROR << "Analysis::Muon at " << trans << " Persistent Muon_p1 at " << pers << " Cannot write to Muon_p1" << endreq;
  throw std::runtime_error("Writing to Muon_p1 is not supported - MuonCnv_p1::transToPers(...)");
  return;
}
