/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p1.h"

void TrigTauClusterCnv_p1 :: persToTrans( const TrigTauCluster_p1 *persObj,
                                            TrigTauCluster    *transObj,
                                            MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p1::persToTrans" << endmsg;

  transObj->setEMenergy          (persObj->m_EMenergy)      ;
  transObj->setHADenergy         (persObj->m_HADenergy)     ;	    
  transObj->setEnergyCalib       (persObj->m_eCalib)	       ;
  transObj->setEMRadius2         (persObj->m_EMRadius[2])   ;
  transObj->setCaloRadius        (-99.); // Hadronic radius not available in old TauCluster
  transObj->setIsoFrac           (persObj->m_IsoFrac)       ;
  transObj->setNumStripCells     (persObj->m_numStripCells) ;
  transObj->setNumTotCells       (-99) ;  // Total number of cells not available in old TauCluster
  transObj->setStripWidth        (persObj->m_stripWidth)    ;
  transObj->setStripWidthOffline (persObj->m_EMenergyWidth[1])    ;

  ITPConverterFor<TrigCaloCluster>*    cnv = nullptr;
  fillTransFromPStore( &cnv, persObj->m_trigCaloCluster, transObj, log );

  transObj->setEta(persObj->m_Eta);
  transObj->setPhi(persObj->m_Phi);
}

void TrigTauClusterCnv_p1 :: transToPers( const TrigTauCluster */*transObj*/,
                                          TrigTauCluster_p1 */*persObj*/,
					  MsgStream& log ) const
{

  log << MSG::DEBUG << "TrigTauClusterCnv_p1::transToPers" << endmsg;
  log << MSG::WARNING << "Trying to write TrigTauCluster to obsolete persistent class TrigTauCluster_p1!" << endmsg;
/*
  persObj->m_EMenergy       = transObj->m_EMenergy      ;
  persObj->m_HADenergy	    = transObj->m_HADenergy     ;	    
  persObj->m_eCalib	    = transObj->m_eCalib	;
  persObj->m_IsoFrac        = transObj->m_IsoFrac       ;
  persObj->m_numStripCells  = transObj->m_numStripCells ;
  persObj->m_stripWidth     = transObj->m_stripWidth    ;

  persObj->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, transObj, log );
*/
}
