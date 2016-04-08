/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p2.h"

void TrigTauClusterDetailsCnv_p2 :: persToTrans( const TrigTauClusterDetails_p2 *persObj,
                                            TrigTauClusterDetails    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p2::persToTrans" << endreq;

  for( int i=0; i<NUMEMSAMP; i++ ){
    transObj->setEMRadius       (i, persObj->m_allEMFloats[i]);
    transObj->setEMenergyWidth  (i, persObj->m_allEMFloats[i+NUMEMSAMP]);
    transObj->setEMenergyWide   (i, persObj->m_allEMFloats[i+2*NUMEMSAMP]);
    transObj->setEMenergyMedium (i, persObj->m_allEMFloats[i+3*NUMEMSAMP]);
    transObj->setEMenergyNarrow (i, persObj->m_allEMFloats[i+4*NUMEMSAMP]);
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    transObj->setHADRadius       (i, persObj->m_allHADFloats[i]);
    transObj->setHADenergyWidth  (i, persObj->m_allHADFloats[i+NUMHADSAMP]);
    transObj->setHADenergyWide   (i, persObj->m_allHADFloats[i+2*NUMHADSAMP]);
    transObj->setHADenergyMedium (i, persObj->m_allHADFloats[i+3*NUMHADSAMP]);
    transObj->setHADenergyNarrow (i, persObj->m_allHADFloats[i+4*NUMHADSAMP]);
  }

}

void TrigTauClusterDetailsCnv_p2 :: transToPers( const TrigTauClusterDetails    *transObj,
                                            TrigTauClusterDetails_p2 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::transToPers" << endreq;


  for( int i=0; i<NUMEMSAMP; i++ ){
    persObj->m_allEMFloats[i]     = transObj->EMRadius(i);
    persObj->m_allEMFloats[i+NUMEMSAMP] = transObj->EMenergyWidth(i);
    persObj->m_allEMFloats[i+2*NUMEMSAMP]   = transObj->EMenergyWide(i);
    persObj->m_allEMFloats[i+3*NUMEMSAMP]   = transObj->EMenergyMedium(i);
    persObj->m_allEMFloats[i+4*NUMEMSAMP]   = transObj->EMenergyNarrow(i);
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    persObj->m_allHADFloats[i]   = transObj->HADRadius(i);
    persObj->m_allHADFloats[i+NUMHADSAMP] = transObj->HADenergyWidth(i);
    persObj->m_allHADFloats[i+2*NUMHADSAMP]  = transObj->HADenergyWide(i);
    persObj->m_allHADFloats[i+3*NUMHADSAMP]  = transObj->HADenergyMedium(i);
    persObj->m_allHADFloats[i+4*NUMHADSAMP]  = transObj->HADenergyNarrow(i);
  }

}
