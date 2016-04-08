/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"

void TrigTauClusterDetailsCnv_p1 :: persToTrans( const TrigTauClusterDetails_p1 *persObj,
                                            TrigTauClusterDetails    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::persToTrans" << endreq;

  for( int i=0; i<NUMEMSAMP; i++ ){
    transObj->setEMRadius       (i, persObj->m_EMRadius[i]);
    transObj->setEMenergyWidth  (i, persObj->m_EMenergyWidth[i]);
    transObj->setEMenergyWide   (i, persObj->m_EMenergyWide[i]);
    transObj->setEMenergyMedium (i, persObj->m_EMenergyMedium[i]);
    transObj->setEMenergyNarrow (i, persObj->m_EMenergyNarrow[i]);
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    transObj->setHADRadius       (i, persObj->m_HADRadius[i]);
    transObj->setHADenergyWidth  (i, persObj->m_HADenergyWidth[i]);
    transObj->setHADenergyWide   (i, persObj->m_HADenergyWide[i]);
    transObj->setHADenergyMedium (i, persObj->m_HADenergyMedium[i]);
    transObj->setHADenergyNarrow (i, persObj->m_HADenergyNarrow[i]);
  }

}

void TrigTauClusterDetailsCnv_p1 :: transToPers( const TrigTauClusterDetails    *transObj,
                                            TrigTauClusterDetails_p1 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::transToPers" << endreq;


  for( int i=0; i<NUMEMSAMP; i++ ){
    persObj->m_EMRadius[i]      = transObj->EMRadius(i);
    persObj->m_EMenergyWidth[i] = transObj->EMenergyWidth(i);
    persObj->m_EMenergyWide[i]   = transObj->EMenergyWide(i);
    persObj->m_EMenergyMedium[i]   = transObj->EMenergyMedium(i);
    persObj->m_EMenergyNarrow[i]   = transObj->EMenergyNarrow(i);
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    persObj->m_HADRadius[i]     = transObj->HADRadius(i);
    persObj->m_HADenergyWidth[i]= transObj->HADenergyWidth(i);
    persObj->m_HADenergyWide[i]  = transObj->HADenergyWide(i);
    persObj->m_HADenergyMedium[i]  = transObj->HADenergyMedium(i);
    persObj->m_HADenergyNarrow[i]  = transObj->HADenergyNarrow(i);
  }

}
