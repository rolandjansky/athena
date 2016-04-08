/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p2.h"

void TrigTauClusterCnv_p2 :: persToTrans( const TrigTauCluster_p2 *persObj,
                                            TrigTauCluster    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p2::persToTrans" << endreq;

  transObj->setEMenergy       (persObj->m_EMenergy)      ;
  transObj->setHADenergy      (persObj->m_HADenergy)     ;	    
  transObj->setEnergyCalib    (persObj->m_eCalib)	       ;
  transObj->setEMRadius2      (persObj->m_EMRadius2)      ;
  transObj->setCaloRadius     (persObj->m_CaloRadius)      ;
  transObj->setIsoFrac        (persObj->m_IsoFrac)       ;
  transObj->setNumStripCells  (persObj->m_numStripCells) ;
  transObj->setNumTotCells    (persObj->m_numTotCells) ;
  transObj->setStripWidth     (persObj->m_stripWidth)    ;
  transObj->setStripWidthOffline (persObj->m_stripWidthOffline)    ;

  ElementLink<TrigTauClusterDetailsContainer> clusterDetails;
  m_ELinkTauClusterDetailsCnv.persToTrans( &persObj->m_details, &clusterDetails, log );
  transObj->setClusterDetails (clusterDetails);
  fillTransFromPStore( &m_trigCaloClusterCnv, persObj->m_trigCaloCluster, transObj, log );

}

void TrigTauClusterCnv_p2 :: transToPers( const TrigTauCluster    *transObj,
                                            TrigTauCluster_p2 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p2::transToPers" << endreq;

  persObj->m_EMenergy       = transObj->EMenergy()      ;
  persObj->m_HADenergy	    = transObj->HADenergy()     ;	    
  persObj->m_eCalib	    = transObj->eCalib()	;
  persObj->m_EMRadius2	    = transObj->EMRadius2()      ;
  persObj->m_CaloRadius	    = transObj->CaloRadius()      ;
  persObj->m_IsoFrac        = transObj->IsoFrac()       ;
  persObj->m_numStripCells  = transObj->numStripCells() ;
  persObj->m_numTotCells  = transObj->numTotCells() ;
  persObj->m_stripWidth     = transObj->stripWidth()    ;
  persObj->m_stripWidthOffline     = transObj->stripWidthOffline()    ;


  m_ELinkTauClusterDetailsCnv.transToPers( &transObj->clusterDetailsLink(), &persObj->m_details, log );
  persObj->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, transObj, log );

}
