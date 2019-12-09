/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p3.h"
#include "CxxUtils/copy_bounded.h"

void TrigCaloClusterCnv_p3 :: persToTrans( const TrigCaloCluster_p3 *persObj,
                                            TrigCaloCluster    *transObj,
                                            MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p3::persToTrans" << endmsg;

  transObj->setRawEnergy           (persObj->m_allFloats[0]);
  transObj->setRawEt               (persObj->m_allFloats[1]);
  transObj->setRawEta              (persObj->m_allFloats[2]);
  transObj->setRawPhi              (persObj->m_allFloats[3]);
  transObj->setRoIword             (persObj->m_roiWord);
  transObj->setNCells              (persObj->m_numberUsedCells);
  transObj->setquality             (persObj->m_quality);
  transObj->setRawEnergies (std::begin (persObj->m_allFloats)+4,
                            std::end   (persObj->m_allFloats));
}

void TrigCaloClusterCnv_p3 :: transToPers( const TrigCaloCluster    *transObj,
                                            TrigCaloCluster_p3 *persObj,
                                            MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p3::transToPers" << endmsg;

  persObj->m_allFloats[0]        = transObj->rawEnergy()          ;
  persObj->m_allFloats[1]        = transObj->rawEt()              ;
  persObj->m_allFloats[2]        = transObj->rawEta()             ;
  persObj->m_allFloats[3]        = transObj->rawPhi()             ;
  persObj->m_roiWord             = transObj->RoIword()            ;
  persObj->m_numberUsedCells     = transObj->nCells()             ;
  persObj->m_quality             = transObj->quality()            ;
  transObj->rawEnergies (std::begin (persObj->m_allFloats)+4,
                         std::end   (persObj->m_allFloats));
}
