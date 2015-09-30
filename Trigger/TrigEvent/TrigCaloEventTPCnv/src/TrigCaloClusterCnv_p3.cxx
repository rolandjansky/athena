/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define protected public
#include "TrigCaloEvent/TrigCaloCluster.h"
#undef protected

#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p3.h"
#include "CxxUtils/copy_bounded.h"

void TrigCaloClusterCnv_p3 :: persToTrans( const TrigCaloCluster_p3 *persObj,
                                            TrigCaloCluster    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p3::persToTrans" << endreq;

  transObj->m_rawEnergy           = persObj->m_allFloats[0]       ;
  transObj->m_rawEt               = persObj->m_allFloats[1]       ;
  transObj->m_rawEta              = persObj->m_allFloats[2]       ;
  transObj->m_rawPhi              = persObj->m_allFloats[3]       ;
  transObj->m_roiWord             = persObj->m_roiWord            ;
  transObj->m_numberUsedCells     = persObj->m_numberUsedCells    ;
  transObj->m_quality             = persObj->m_quality            ;
  auto out = CxxUtils::copy_bounded (std::begin(persObj->m_allFloats)+4,
                                     std::end(persObj->m_allFloats),
                                     std::begin(transObj->m_rawEnergyS),
                                     std::end(transObj->m_rawEnergyS));
  std::fill (out, std::end(transObj->m_rawEnergyS), 0);
}

void TrigCaloClusterCnv_p3 :: transToPers( const TrigCaloCluster    *transObj,
                                            TrigCaloCluster_p3 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p3::transToPers" << endreq;

  persObj->m_allFloats[0]        = transObj->m_rawEnergy          ;
  persObj->m_allFloats[1]        = transObj->m_rawEt              ;
  persObj->m_allFloats[2]        = transObj->m_rawEta             ;
  persObj->m_allFloats[3]        = transObj->m_rawPhi             ;
  persObj->m_roiWord             = transObj->m_roiWord            ;
  persObj->m_numberUsedCells     = transObj->m_numberUsedCells    ;
  persObj->m_quality             = transObj->m_quality            ;
  auto out = CxxUtils::copy_bounded (std::begin(transObj->m_rawEnergyS),
                                     std::end(transObj->m_rawEnergyS),
                                     std::begin(persObj->m_allFloats)+4,
                                     std::end(persObj->m_allFloats));
  std::fill (out, std::end(persObj->m_allFloats), 0);
}
