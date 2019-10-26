/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigCaloCluster.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"
#include "CxxUtils/copy_bounded.h"

void TrigCaloClusterCnv_p2 :: persToTrans( const TrigCaloCluster_p2 *persObj,
                                            TrigCaloCluster    *transObj,
                                            MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p2::persToTrans" << endmsg;

  transObj->setRawEnergy           (persObj->m_rawEnergy);
  transObj->setRawEt               (persObj->m_rawEt);
  transObj->setRawEta              (persObj->m_rawEta);
  transObj->setRawPhi              (persObj->m_rawPhi);
  transObj->setRoIword             (persObj->m_roiWord);
  transObj->setNCells              (persObj->m_numberUsedCells);
  transObj->setquality             (persObj->m_quality);
  transObj->setRawEnergies (std::begin (persObj->m_rawEnergyS),
                            std::end   (persObj->m_rawEnergyS));
}

void TrigCaloClusterCnv_p2 :: transToPers( const TrigCaloCluster    *transObj,
                                            TrigCaloCluster_p2 *persObj,
                                            MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigCaloClusterCnv_p2::transToPers" << endmsg;

  persObj->m_rawEnergy           = transObj->rawEnergy()          ;
  persObj->m_rawEt               = transObj->rawEt()              ;
  persObj->m_rawEta              = transObj->rawEta()             ;
  persObj->m_rawPhi              = transObj->rawPhi()             ;
  persObj->m_roiWord             = transObj->RoIword()            ;
  persObj->m_numberUsedCells     = transObj->nCells()             ;
  persObj->m_quality             = transObj->quality()            ;
  transObj->rawEnergies (std::begin (persObj->m_rawEnergyS),
                         std::end   (persObj->m_rawEnergyS));
}
