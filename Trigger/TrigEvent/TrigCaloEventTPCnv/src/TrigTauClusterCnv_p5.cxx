/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_p5.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p5.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"

// pre-allocate converters
static const TrigCaloClusterCnv_p2 trigCaloClusterCnv;

void TrigTauClusterCnv_p5 :: persToTrans(const TrigTauCluster_p5 *persObj, TrigTauCluster *transObj, MsgStream& log) const
{
  log << MSG::DEBUG << "TrigTauClusterCnv_p5::persToTrans" << endmsg;

  transObj->setEMenergy          (persObj->m_allTheFloats[0]) ;
  transObj->setHADenergy	 (persObj->m_allTheFloats[1]) ;	    
  transObj->setEnergyCalib       (persObj->m_allTheFloats[2]) ;
  transObj->setEMRadius2	 (persObj->m_allTheFloats[3]) ;
  transObj->setCaloRadius        (persObj->m_allTheFloats[4]) ;
  transObj->setIsoFrac           (persObj->m_allTheFloats[5]) ;
  transObj->setStripWidth        (persObj->m_allTheFloats[6]) ;
  transObj->setStripWidthOffline (persObj->m_allTheFloats[7]) ;
  transObj->setEMRadius3S        (persObj->m_allTheFloats[8]);
  transObj->setCoreFrac          (persObj->m_allTheFloats[9]);
  transObj->setEMFrac            (persObj->m_allTheFloats[10]);
  transObj->setHadRadius         (persObj->m_allTheFloats[11]);

  transObj->setNumStripCells     (persObj->m_allTheInts[0])   ;
  transObj->setNumTotCells       (persObj->m_allTheInts[1])   ;

  ElementLink<TrigTauClusterDetailsContainer> clusterDetails;
  m_ELinkTauClusterDetailsCnv.persToTrans( &persObj->m_details, &clusterDetails, log );
  transObj->setClusterDetails (clusterDetails);
  trigCaloClusterCnv.persToTrans(&persObj->m_trigCaloCluster, transObj, log);
}

void TrigTauClusterCnv_p5 :: transToPers(const TrigTauCluster *transObj, TrigTauCluster_p5 *persObj, MsgStream& log) const
{
  log << MSG::DEBUG << "TrigTauClusterCnv_p5::transToPers" << endmsg;

  persObj->m_allTheFloats[0] = transObj->EMenergy()          ;
  persObj->m_allTheFloats[1] = transObj->HADenergy()         ;	    
  persObj->m_allTheFloats[2] = transObj->eCalib()     	     ;
  persObj->m_allTheFloats[3] = transObj->EMRadius2()         ;
  persObj->m_allTheFloats[4] = transObj->CaloRadius()        ;
  persObj->m_allTheFloats[5] = transObj->IsoFrac()           ;
  persObj->m_allTheFloats[6] = transObj->stripWidth()        ;
  persObj->m_allTheFloats[7] = transObj->stripWidthOffline() ;
  persObj->m_allTheFloats[8] = transObj->EMRadius3S();
  persObj->m_allTheFloats[9] = transObj->CoreFrac();
  persObj->m_allTheFloats[10] = transObj->EMFrac();
  persObj->m_allTheFloats[11] = transObj->HadRadius();

  persObj->m_allTheInts[0]   = transObj->numStripCells() ;
  persObj->m_allTheInts[1]   = transObj->numTotCells()   ;

  log << MSG::DEBUG << "TrigTauClusterCnv_p5::transToPers - fill EL" << endmsg;

  m_ELinkTauClusterDetailsCnv.transToPers(&transObj->clusterDetailsLink(), &persObj->m_details, log);

  log << MSG::DEBUG << "TrigTauClusterCnv_p5::transToPers - fill trigcalocluster" << endmsg;

  trigCaloClusterCnv.transToPers(transObj, &persObj->m_trigCaloCluster, log);

  log << MSG::DEBUG << "TrigTauClusterCnv_p5::transToPers - filled" << endmsg;
}
