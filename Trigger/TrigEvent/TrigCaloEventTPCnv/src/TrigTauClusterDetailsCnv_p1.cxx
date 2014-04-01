/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#undef private
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h"

void TrigTauClusterDetailsCnv_p1 :: persToTrans( const TrigTauClusterDetails_p1 *persObj,
                                            TrigTauClusterDetails    *transObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::persToTrans" << endreq;

  for( int i=0; i<NUMEMSAMP; i++ ){
    transObj->m_EMRadius[i]      = persObj->m_EMRadius[i]       ;
    transObj->m_EMenergyWidth[i] = persObj->m_EMenergyWidth[i]  ;
    transObj->m_EMenergyWide[i]   = persObj->m_EMenergyWide[i]    ;
    transObj->m_EMenergyMedium[i]   = persObj->m_EMenergyMedium[i]    ;
    transObj->m_EMenergyNarrow[i]   = persObj->m_EMenergyNarrow[i]    ;
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    transObj->m_HADRadius[i]     = persObj->m_HADRadius[i]       ;
    transObj->m_HADenergyWidth[i]= persObj->m_HADenergyWidth[i] ;
    transObj->m_HADenergyWide[i]  = persObj->m_HADenergyWide[i]   ;
    transObj->m_HADenergyMedium[i]  = persObj->m_HADenergyMedium[i]   ;
    transObj->m_HADenergyNarrow[i]  = persObj->m_HADenergyNarrow[i]   ;
  }

}

void TrigTauClusterDetailsCnv_p1 :: transToPers( const TrigTauClusterDetails    *transObj,
                                            TrigTauClusterDetails_p1 *persObj,
                                            MsgStream& log )
{

  log << MSG::DEBUG << "TrigTauClusterDetailsCnv_p1::transToPers" << endreq;


  for( int i=0; i<NUMEMSAMP; i++ ){
    persObj->m_EMRadius[i]      = transObj->m_EMRadius[i]       ;
    persObj->m_EMenergyWidth[i] = transObj->m_EMenergyWidth[i]  ;
    persObj->m_EMenergyWide[i]   = transObj->m_EMenergyWide[i]    ;
    persObj->m_EMenergyMedium[i]   = transObj->m_EMenergyMedium[i]    ;
    persObj->m_EMenergyNarrow[i]   = transObj->m_EMenergyNarrow[i]    ;
  }

  for( int i=0; i<NUMHADSAMP; i++ ){
    persObj->m_HADRadius[i]     = transObj->m_HADRadius[i]       ;
    persObj->m_HADenergyWidth[i]= transObj->m_HADenergyWidth[i] ;
    persObj->m_HADenergyWide[i]  = transObj->m_HADenergyWide[i]   ;
    persObj->m_HADenergyMedium[i]  = transObj->m_HADenergyMedium[i]   ;
    persObj->m_HADenergyNarrow[i]  = transObj->m_HADenergyNarrow[i]   ;
  }

}
