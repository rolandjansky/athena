/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define protected public
#include "TrigCaloEvent/TrigCaloCluster.h"
#undef protected

#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"
#include "CxxUtils/copy_bounded.h"

void TrigCaloClusterCnv_p2 :: persToTrans( const TrigCaloCluster_p2 *persObj,
                                            TrigCaloCluster    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p2::persToTrans" << endreq;

  transObj->m_rawEnergy           = persObj->m_rawEnergy          ;
  transObj->m_rawEt               = persObj->m_rawEt              ;
  transObj->m_rawEta              = persObj->m_rawEta             ;
  transObj->m_rawPhi              = persObj->m_rawPhi             ;
  transObj->m_roiWord             = persObj->m_roiWord            ;
  transObj->m_numberUsedCells     = persObj->m_numberUsedCells    ;
  transObj->m_quality             = persObj->m_quality            ;
  auto out = CxxUtils::copy_bounded (persObj->m_rawEnergyS,
                                     transObj->m_rawEnergyS);
  std::fill (out, std::end(transObj->m_rawEnergyS), 0);
}

void TrigCaloClusterCnv_p2 :: transToPers( const TrigCaloCluster    *transObj,
                                            TrigCaloCluster_p2 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p2::transToPers" << endreq;

  persObj->m_rawEnergy           = transObj->m_rawEnergy          ;
  persObj->m_rawEt               = transObj->m_rawEt              ;
  persObj->m_rawEta              = transObj->m_rawEta             ;
  persObj->m_rawPhi              = transObj->m_rawPhi             ;
  persObj->m_roiWord             = transObj->m_roiWord            ;
  persObj->m_numberUsedCells     = transObj->m_numberUsedCells    ;
  persObj->m_quality             = transObj->m_quality            ;
  auto out = CxxUtils::copy_bounded (transObj->m_rawEnergyS,
                                     persObj->m_rawEnergyS);
  std::fill (out, std::end(persObj->m_rawEnergyS), 0);
}
