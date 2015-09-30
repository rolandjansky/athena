/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigTauCluster.h"
#undef private

#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p2.h"

void TrigTauClusterCnv_p2 :: persToTrans( const TrigTauCluster_p2 *persObj,
                                            TrigTauCluster    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p2::persToTrans" << endreq;

  transObj->m_EMenergy      = persObj->m_EMenergy      ;
  transObj->m_HADenergy	    = persObj->m_HADenergy     ;	    
  transObj->m_eCalib	    = persObj->m_eCalib	       ;
  transObj->m_EMRadius2	    = persObj->m_EMRadius2      ;
  transObj->m_CaloRadius    = persObj->m_CaloRadius      ;
  transObj->m_IsoFrac       = persObj->m_IsoFrac       ;
  transObj->m_numStripCells = persObj->m_numStripCells ;
  transObj->m_numTotCells = persObj->m_numTotCells ;
  transObj->m_stripWidth    = persObj->m_stripWidth    ;
  transObj->m_stripWidthOffline    = persObj->m_stripWidthOffline    ;


  m_ELinkTauClusterDetailsCnv.persToTrans( &persObj->m_details, &transObj->m_clusterDetails, log );
  fillTransFromPStore( &m_trigCaloClusterCnv, persObj->m_trigCaloCluster, transObj, log );

}

void TrigTauClusterCnv_p2 :: transToPers( const TrigTauCluster    *transObj,
                                            TrigTauCluster_p2 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p2::transToPers" << endreq;

  persObj->m_EMenergy       = transObj->m_EMenergy      ;
  persObj->m_HADenergy	    = transObj->m_HADenergy     ;	    
  persObj->m_eCalib	    = transObj->m_eCalib	;
  persObj->m_EMRadius2	    = transObj->m_EMRadius2      ;
  persObj->m_CaloRadius	    = transObj->m_CaloRadius      ;
  persObj->m_IsoFrac        = transObj->m_IsoFrac       ;
  persObj->m_numStripCells  = transObj->m_numStripCells ;
  persObj->m_numTotCells  = transObj->m_numTotCells ;
  persObj->m_stripWidth     = transObj->m_stripWidth    ;
  persObj->m_stripWidthOffline     = transObj->m_stripWidthOffline    ;


  m_ELinkTauClusterDetailsCnv.transToPers( &transObj->m_clusterDetails, &persObj->m_details, log );
  persObj->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, transObj, log );

}
