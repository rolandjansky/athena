///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloEnergyCnv_p1.cxx 
// Implementation file for class CaloEnergyCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#define private public
#define protected public
#include "muonEvent/DepositInCalo.h"
#include "muonEvent/CaloEnergy.h"
#undef private
#undef protected

// RecTPCnv includes
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLoss_p1.h"
#include "RecTPCnv/DepositInCaloCnv_p1.h"
#include "RecTPCnv/CaloEnergyCnv_p1.h"

// pre-allocate converters
static EnergyLossCnv_p1 energyLossCnv;

// pre-allocate converters
static DepositInCaloCnv_p1 depositCnv;

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void CaloEnergyCnv_p1::persToTrans( const CaloEnergy_p1* pers,
				    CaloEnergy* trans, 
				    MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading CaloEnergy from persistent state..."
//       << endreq;
  Trk::EnergyLoss_p1 energyLoss;
  energyLoss.m_deltaE           = pers->m_energyDeposit;
  energyLoss.m_sigmaMinusDeltaE = pers->m_energySigmaMinus;
  energyLoss.m_sigmaPlusDeltaE  = pers->m_energySigmaPlus;

  //Trk :: EnergyLoss * transObject = trans;
  energyLossCnv.persToTrans( &energyLoss,
                             trans,
                             msg );

  trans->m_energyLossType     = static_cast<CaloEnergy::EnergyLossType>(pers->m_energyLossType);
  trans->m_caloLRLikelihood   = pers->m_caloLRLikelihood;
  trans->m_caloMuonIdTag      = pers->m_caloMuonIdTag;
  trans->m_fsrCandidateEnergy = pers->m_fsrCandidateEnergy;

  // reserve enough space so no re-alloc occurs
  trans->m_deposits.reserve( pers->m_deposits.size() );

  typedef std::vector<DepositInCalo_p1> Deposits_t;
  for ( Deposits_t::const_iterator 
	  itr  = pers->m_deposits.begin(),
	  iEnd = pers->m_deposits.end();
	itr != iEnd;
	++itr ) {
    trans->m_deposits.push_back( DepositInCalo() );
    depositCnv.persToTrans( &*itr, &trans->m_deposits.back(), msg );
  }

//   msg << MSG::DEBUG << "Loaded CaloEnergy from persistent state [OK]"
//       << endreq;

  return;
}

void CaloEnergyCnv_p1::transToPers( const CaloEnergy* trans, 
				    CaloEnergy_p1* pers, 
				    MsgStream& msg ) 
{

  msg << MSG::ERROR << "CaloEnergy at " << trans << " Persistent CaloEnergy_p1 at " 
                    << pers << " Cannot write to CaloEnergy_p1" << endreq;
  throw std::runtime_error("Writing to CaloEnergy_p1 is not supported - CaloEnergyCnv_p1::transToPers(...)");
  return;

}
