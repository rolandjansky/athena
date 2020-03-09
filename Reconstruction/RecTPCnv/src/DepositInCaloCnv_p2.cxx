///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p2.cxx 
// Implementation file for class DepositInCaloCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#include "muonEvent/DepositInCalo.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p2.h"


void DepositInCaloCnv_p2::persToTrans( const DepositInCalo_p2* pers,
				       DepositInCalo* trans, 
				       MsgStream& /*msg*/ ) const
{
//   msg << MSG::DEBUG << "Loading DepositInCalo from persistent state..."
//       << endmsg;

  *trans = DepositInCalo(static_cast<CaloCell_ID::CaloSample>(pers->m_subCaloId),
                         pers->m_energyDeposited,
                         pers->m_muonEnergyLoss,
                         pers->m_etDeposited);


//   msg << MSG::DEBUG << "Loaded DepositInCalo from persistent state [OK]"
//       << endmsg;

  return;
}

void DepositInCaloCnv_p2::transToPers( const DepositInCalo* trans, 
				       DepositInCalo_p2* pers, 
				       MsgStream& /*msg*/ ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of DepositInCalo..."
//       << endmsg;

  // calorimeter sampling identifier
  pers->m_subCaloId = static_cast<CaloCell_ID::CaloSample>(trans->subCaloId());

  // energy deposited
  pers->m_energyDeposited = trans->energyDeposited();

  // energy Loss of the muons computed using the extrapolator
  pers->m_muonEnergyLoss = trans->muonEnergyLoss();

  // Energy loss Et
  pers->m_etDeposited = trans->etDeposited();
  
  //   msg << MSG::DEBUG << "Created persistent state of DepositInCalo [OK]"
//       << endmsg;
  return;
}
