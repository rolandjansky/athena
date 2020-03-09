///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DepositInCaloCnv_p1.cxx 
// Implementation file for class DepositInCaloCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// muonEvent includes
#include "muonEvent/DepositInCalo.h"

// RecTPCnv includes
#include "RecTPCnv/DepositInCaloCnv_p1.h"


void DepositInCaloCnv_p1::persToTrans( const DepositInCalo_p1* pers,
				       DepositInCalo* trans, 
				       MsgStream& /*msg*/ ) const
{
//   msg << MSG::DEBUG << "Loading DepositInCalo from persistent state..."
//       << endmsg;

  *trans = DepositInCalo(static_cast<CaloCell_ID::CaloSample>(pers->m_subCaloId),
                         pers->m_energyDeposited,
                         pers->m_muonEnergyLoss,
                         0.0); // energy deposited et - not available in the p1 versions
                         
//   msg << MSG::DEBUG << "Loaded DepositInCalo from persistent state [OK]"
//       << endmsg;

  return;
}

void DepositInCaloCnv_p1::transToPers( const DepositInCalo* trans, 
				       DepositInCalo_p1* pers, 
				       MsgStream& msg ) const
{

  msg << MSG::ERROR << "DepositInCalo at " << trans << " Persistent DepositInCalo_p1 at " << pers 
      << " Cannot write to DepositInCalo_p1" << endmsg;
  throw std::runtime_error("Writing to DepositInCalo_p1 is not supported - DepositInCaloCnv_p1::transToPers(...)");

  return;
}
