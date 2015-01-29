/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigInDetTrack.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p5.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p5.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p5::persToTrans( const TrigInDetTrack_p5 *persObj, TrigInDetTrack *transObj, MsgStream  &log  )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p5::persToTrans called " << endreq;

  transObj->m_algId             = (TrigInDetTrack::AlgoId) persObj->m_allIntegers[0];
  transObj->m_NStrawHits        = persObj->m_allIntegers[1];
  transObj->m_NStraw            = persObj->m_allIntegers[2];
  transObj->m_NStrawTime        = persObj->m_allIntegers[3];
  transObj->m_NTRHits           = persObj->m_allIntegers[4];
  transObj->m_NPixelSpacePoints = persObj->m_allIntegers[5];
  transObj->m_NSCT_SpacePoints  = persObj->m_allIntegers[6];
  transObj->m_HitPattern        = persObj->m_allIntegers[7];

  transObj->m_chi2        = persObj->m_chi2;
  transObj->m_param       = createTransFromPStore( &m_fpCnv, persObj->m_param, log );
  transObj->m_endParam    = createTransFromPStore( &m_fpCnv, persObj->m_endParam, log );
  transObj->m_rdoList.resize( persObj->m_rdoList.size() ); 
  std::copy( persObj->m_rdoList.begin(), persObj->m_rdoList.end(), transObj->m_rdoList.begin() ); 
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p5::transToPers( const TrigInDetTrack *transObj, TrigInDetTrack_p5 *persObj, MsgStream &log )
{

  // log << MSG::DEBUG << "TrigInDetTrackCnv_p5::transToPers called " << endreq;
  persObj->m_allIntegers[0]= transObj->m_algId      ;
  persObj->m_allIntegers[1]= transObj->m_NStrawHits ;
  persObj->m_allIntegers[2]= transObj->m_NStraw     ;
  persObj->m_allIntegers[3]= transObj->m_NStrawTime ;
  persObj->m_allIntegers[4]= transObj->m_NTRHits    ;  
  persObj->m_allIntegers[5]= transObj->m_NPixelSpacePoints;
  persObj->m_allIntegers[6]= transObj->m_NSCT_SpacePoints;
  persObj->m_allIntegers[7]= transObj->m_HitPattern;


  persObj->m_chi2        = transObj->m_chi2       ;
  persObj->m_param       = toPersistent( &m_fpCnv, transObj->m_param, log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->m_endParam, log );

  if( transObj->m_rdoList.size()!=0){
      persObj->m_rdoList.resize( transObj->m_rdoList.size() );

      unsigned int i=0; 
      for( std::vector<Identifier>::const_iterator it = transObj->m_rdoList.begin(); it != transObj->m_rdoList.end();++it,++i) 
          persObj->m_rdoList[i] =it->get_compact(); 
  }
}
