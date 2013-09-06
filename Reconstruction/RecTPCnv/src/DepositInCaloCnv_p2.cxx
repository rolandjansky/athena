///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p2.cxx 
// Implementation file for class DepositInCaloCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#define private public
#define protected public
#include "muonEvent/DepositInCalo.h"
#undef private
#undef protected

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p2.h"

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void DepositInCaloCnv_p2::persToTrans( const DepositInCalo_p2* pers,
				       DepositInCalo* trans, 
				       MsgStream& /*msg*/ ) 
{
//   msg << MSG::DEBUG << "Loading DepositInCalo from persistent state..."
//       << endreq;

  // calorimeter sampling identifier
  trans->m_subCaloId = static_cast<CaloCell_ID::CaloSample>(pers->m_subCaloId);

  // energy deposited
  trans->m_energyDeposited = pers->m_energyDeposited;

  // energy Loss of the muons computed using the extrapolator
  trans->m_muonEnergyLoss = pers->m_muonEnergyLoss;

  // energy loss Et
  trans->m_etDeposited = pers->m_etDeposited;

//   msg << MSG::DEBUG << "Loaded DepositInCalo from persistent state [OK]"
//       << endreq;

  return;
}

void DepositInCaloCnv_p2::transToPers( const DepositInCalo* trans, 
				       DepositInCalo_p2* pers, 
				       MsgStream& /*msg*/ ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of DepositInCalo..."
//       << endreq;

  // calorimeter sampling identifier
  pers->m_subCaloId = static_cast<CaloCell_ID::CaloSample>(trans->m_subCaloId);

  // energy deposited
  pers->m_energyDeposited = trans->m_energyDeposited;

  // energy Loss of the muons computed using the extrapolator
  pers->m_muonEnergyLoss = trans->m_muonEnergyLoss;

  // Energy loss Et
  pers->m_etDeposited = trans->m_etDeposited;
  
//   msg << MSG::DEBUG << "Created persistent state of DepositInCalo [OK]"
//       << endreq;
  return;
}
