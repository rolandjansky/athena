/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#undef private
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p2.h"

void TrigTauClusterDetailsCnv_p2 :: persToTrans( const TrigTauClusterDetails_p2 *persObj,
                                            TrigTauClusterDetails    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p2::persToTrans" << endreq;

  for( int i=0; i<NUMEMSAMP; i++ ){
    transObj->m_EMRadius[i]    = persObj->m_allEMFloats[i]       ;
    transObj->m_EMenergyWidth[i] = persObj->m_allEMFloats[i+NUMEMSAMP]  ;
    transObj->m_EMenergyWide[i]   = persObj->m_allEMFloats[i+2*NUMEMSAMP]    ;
    transObj->m_EMenergyMedium[i]   = persObj->m_allEMFloats[i+3*NUMEMSAMP]    ;
    transObj->m_EMenergyNarrow[i]   = persObj->m_allEMFloats[i+4*NUMEMSAMP]    ;
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    transObj->m_HADRadius[i]   = persObj->m_allHADFloats[i]       ;
    transObj->m_HADenergyWidth[i]= persObj->m_allHADFloats[i+NUMHADSAMP] ;
    transObj->m_HADenergyWide[i]  = persObj->m_allHADFloats[i+2*NUMHADSAMP]   ;
    transObj->m_HADenergyMedium[i]  = persObj->m_allHADFloats[i+3*NUMHADSAMP]   ;
    transObj->m_HADenergyNarrow[i]  = persObj->m_allHADFloats[i+4*NUMHADSAMP]   ;
  }

}

void TrigTauClusterDetailsCnv_p2 :: transToPers( const TrigTauClusterDetails    *transObj,
                                            TrigTauClusterDetails_p2 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::transToPers" << endreq;


  for( int i=0; i<NUMEMSAMP; i++ ){
    persObj->m_allEMFloats[i]     = transObj->m_EMRadius[i]       ;
    persObj->m_allEMFloats[i+NUMEMSAMP] = transObj->m_EMenergyWidth[i]  ;
    persObj->m_allEMFloats[i+2*NUMEMSAMP]   = transObj->m_EMenergyWide[i]    ;
    persObj->m_allEMFloats[i+3*NUMEMSAMP]   = transObj->m_EMenergyMedium[i]    ;
    persObj->m_allEMFloats[i+4*NUMEMSAMP]   = transObj->m_EMenergyNarrow[i]    ;
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    persObj->m_allHADFloats[i]   = transObj->m_HADRadius[i]       ;
    persObj->m_allHADFloats[i+NUMHADSAMP] = transObj->m_HADenergyWidth[i] ;
    persObj->m_allHADFloats[i+2*NUMHADSAMP]  = transObj->m_HADenergyWide[i]   ;
    persObj->m_allHADFloats[i+3*NUMHADSAMP]  = transObj->m_HADenergyMedium[i]   ;
    persObj->m_allHADFloats[i+4*NUMHADSAMP]  = transObj->m_HADenergyNarrow[i]   ;
  }

}
