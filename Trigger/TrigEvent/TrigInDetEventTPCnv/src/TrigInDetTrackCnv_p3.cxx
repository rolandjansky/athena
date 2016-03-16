/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p3.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p3::persToTrans( const TrigInDetTrack_p3 *persObj, TrigInDetTrack *transObj, MsgStream  &log  )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p3::persToTrans called " << endreq;

  transObj->algorithmId       ((TrigInDetTrack::AlgoId) persObj->m_allIntegers[0]);
  transObj->StrawHits         (persObj->m_allIntegers[1]);
  transObj->Straw             (persObj->m_allIntegers[2]);
  transObj->StrawTime         (persObj->m_allIntegers[3]);
  transObj->TRHits            (persObj->m_allIntegers[4]);
  transObj->NPixelSpacePoints (persObj->m_allIntegers[5]);
  transObj->NSCT_SpacePoints  (persObj->m_allIntegers[6]);
  transObj->HitPattern        (persObj->m_allIntegers[7]);

  transObj->chi2              (persObj->m_chi2)       ;

  transObj->param      (createTransFromPStore( &m_fpCnv, persObj->m_param, log ));
  transObj->endParam   (createTransFromPStore( &m_fpCnv, persObj->m_endParam, log ));
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p3::transToPers( const TrigInDetTrack *transObj, TrigInDetTrack_p3 *persObj, MsgStream &log )
{
  // log << MSG::DEBUG << "TrigInDetTrackCnv_p3::transToPers called " << endreq;
  persObj->m_allIntegers[0]= transObj->algorithmId()      ;
  persObj->m_allIntegers[1]= transObj->NStrawHits() ;
  persObj->m_allIntegers[2]= transObj->NStraw()     ;
  persObj->m_allIntegers[3]= transObj->NStrawTime() ;
  persObj->m_allIntegers[4]= transObj->NTRHits()    ;  
  persObj->m_allIntegers[5]= transObj->NPixelSpacePoints();
  persObj->m_allIntegers[6]= transObj->NSCT_SpacePoints();
  persObj->m_allIntegers[7]= transObj->HitPattern();

  persObj->m_chi2        = transObj->chi2()       ;

  persObj->m_param       = toPersistent( &m_fpCnv, transObj->param(), log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->endParam(), log );
}
