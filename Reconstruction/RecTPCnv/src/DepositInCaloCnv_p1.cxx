///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p1.cxx 
// Implementation file for class DepositInCaloCnv_p1
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
#include "RecTPCnv/DepositInCaloCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void DepositInCaloCnv_p1::persToTrans( const DepositInCalo_p1* pers,
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

  // energy deposited et - not available in the p1 versions
  trans->m_etDeposited = 0.0;

//   msg << MSG::DEBUG << "Loaded DepositInCalo from persistent state [OK]"
//       << endreq;

  return;
}

void DepositInCaloCnv_p1::transToPers( const DepositInCalo* trans, 
				       DepositInCalo_p1* pers, 
				       MsgStream& msg ) 
{

  msg << MSG::ERROR << "DepositInCalo at " << trans << " Persistent DepositInCalo_p1 at " << pers 
      << " Cannot write to DepositInCalo_p1" << endreq;
  throw std::runtime_error("Writing to DepositInCalo_p1 is not supported - DepositInCaloCnv_p1::transToPers(...)");

  return;
}
