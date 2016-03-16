/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p5.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p5.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p5::persToTrans( const TrigInDetTrack_p5 *persObj, TrigInDetTrack *transObj, MsgStream  &log  )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p5::persToTrans called " << endreq;

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

  std::vector<Identifier> rdoList;
  rdoList.resize( persObj->m_rdoList.size() ); 
  std::copy( persObj->m_rdoList.begin(), persObj->m_rdoList.end(), rdoList.begin() ); 
  transObj->rdoList (std::move(rdoList));
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p5::transToPers( const TrigInDetTrack *transObj, TrigInDetTrack_p5 *persObj, MsgStream &log )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p5::transToPers called " << endreq;
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

  const std::vector<Identifier>& rdoList = transObj->rdoList();
  persObj->m_rdoList.clear();
  persObj->m_rdoList.reserve( rdoList.size() );
  for (const Identifier& id : rdoList)
    persObj->m_rdoList.push_back (id.get_compact());
}
