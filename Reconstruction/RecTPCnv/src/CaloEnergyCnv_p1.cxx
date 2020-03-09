///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CaloEnergyCnv_p1.cxx 
// Implementation file for class CaloEnergyCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#include "muonEvent/DepositInCalo.h"
#include "muonEvent/CaloEnergy.h"

// RecTPCnv includes
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLoss_p1.h"
#include "RecTPCnv/DepositInCaloCnv_p1.h"
#include "RecTPCnv/CaloEnergyCnv_p1.h"

// pre-allocate converters
static const EnergyLossCnv_p1 energyLossCnv;

// pre-allocate converters
static const DepositInCaloCnv_p1 depositCnv;

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void CaloEnergyCnv_p1::persToTrans( const CaloEnergy_p1* pers,
				    CaloEnergy* trans, 
				    MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading CaloEnergy from persistent state..."
//       << endmsg;
  Trk::EnergyLoss_p1 energyLoss;
  energyLoss.m_deltaE           = pers->m_energyDeposit;
  energyLoss.m_sigmaMinusDeltaE = pers->m_energySigmaMinus;
  energyLoss.m_sigmaPlusDeltaE  = pers->m_energySigmaPlus;

  //Trk :: EnergyLoss * transObject = trans;
  energyLossCnv.persToTrans( &energyLoss,
                             trans,
                             msg );

  trans->set_energyLossType   (static_cast<CaloEnergy::EnergyLossType>(pers->m_energyLossType));
  trans->set_caloLRLikelihood  (pers->m_caloLRLikelihood);
  trans->set_caloMuonIdTag     (pers->m_caloMuonIdTag);
  trans->set_fsrCandidateEnergy(pers->m_fsrCandidateEnergy);

  // reserve enough space so no re-alloc occurs
  std::vector<DepositInCalo> deposits (pers->m_deposits.size());

  size_t ideposit = 0;
  for (const DepositInCalo_p1& pers_dep : pers->m_deposits) {
    depositCnv.persToTrans( &pers_dep, &deposits[ideposit++], msg );
  }
  trans->set_deposits (std::move (deposits));

//   msg << MSG::DEBUG << "Loaded CaloEnergy from persistent state [OK]"
//       << endmsg;

  return;
}

void CaloEnergyCnv_p1::transToPers( const CaloEnergy* trans, 
				    CaloEnergy_p1* pers, 
				    MsgStream& msg ) const
{

  msg << MSG::ERROR << "CaloEnergy at " << trans << " Persistent CaloEnergy_p1 at " 
                    << pers << " Cannot write to CaloEnergy_p1" << endmsg;
  throw std::runtime_error("Writing to CaloEnergy_p1 is not supported - CaloEnergyCnv_p1::transToPers(...)");
  return;

}
