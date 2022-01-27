/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigVertexCnv_p1.h"


//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigVertexCnv_p1::persToTrans( const TrigVertex_p1 *persObj,
				    TrigVertex    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigVertexCnv_p1::persToTrans called " << endmsg;
  
  transObj->m_x                 = persObj->m_x                 ;
  transObj->m_y                 = persObj->m_y                 ;
  transObj->m_z                 = persObj->m_z                 ;
  transObj->m_mass              = persObj->m_mass              ;
  transObj->m_massVar           = persObj->m_massVar           ;
  transObj->m_energyFraction    = persObj->m_energyFraction    ;
  transObj->m_nTwoTracksSecVtx  = persObj->m_nTwoTracksSecVtx  ;
  transObj->m_chiSquared        = persObj->m_chiSquared        ;
  transObj->m_nDOF              = persObj->m_nDOF              ;
  for(int i=0;i<6;i++){
    transObj->m_cov[i]          = persObj->m_cov[i]            ;
  }
  
  transObj->m_algId             = static_cast<TrigVertex::AlgoId>(persObj->m_algId) ;
  
  m_TrigInDetTrackVectorCnv.persToTrans( &(persObj->m_tracks), &m_trackInVertexVector, log);
  if(!m_trackInVertexVector.empty()) {
    if (transObj->m_tracks)
      transObj->m_tracks->assign (m_trackInVertexVector.begin(),
                                  m_trackInVertexVector.end());
    else
      transObj->m_tracks =
        new TrackInVertexList (m_trackInVertexVector.begin(),
                               m_trackInVertexVector.end());
    transObj->m_ownTracks = true;
  }
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigVertexCnv_p1::transToPers( const TrigVertex    *transObj,
				    TrigVertex_p1 *persObj ,
				    MsgStream     &log      )
{
  log << MSG::DEBUG << "TrigVertexCnv_p1::transToPers called " << endmsg;

  persObj->m_x                 = transObj->m_x                 ;
  persObj->m_y                 = transObj->m_y                 ;
  persObj->m_z                 = transObj->m_z                 ;
  persObj->m_mass              = transObj->m_mass              ;
  persObj->m_massVar           = transObj->m_massVar           ;
  persObj->m_energyFraction    = transObj->m_energyFraction    ;
  persObj->m_nTwoTracksSecVtx  = transObj->m_nTwoTracksSecVtx  ;
  persObj->m_chiSquared        = transObj->m_chiSquared        ;
  persObj->m_nDOF              = transObj->m_nDOF              ;
  for(int i=0;i<6;i++){
    persObj->m_cov[i]            = transObj->m_cov[i]            ;
  }
  
  persObj->m_algId             = static_cast<TrigVertex_p1::AlgoId_p1>(transObj->m_algId)             ;
  
  if(transObj->m_tracks!=NULL ){ 
       
    TrackInVertexList L = *(transObj->m_tracks);
    TrackInVertexList::const_iterator iBeg = L.begin();
    TrackInVertexList::const_iterator iEnd = L.end();
    m_trackInVertexVector.clear();
    m_trackInVertexVector.reserve(L.size());

    for(; iBeg != iEnd; ++iBeg){
      m_trackInVertexVector.push_back(*iBeg);
    }

    //TrackInVertexVector::const_iterator iVBeg = m_trackInVertexVector.begin();
    //TrackInVertexVector::const_iterator iVEnd = m_trackInVertexVector.end();
    
    m_TrigInDetTrackVectorCnv.transToPers( &m_trackInVertexVector, &(persObj->m_tracks), log);
    
  } else {
    log << MSG::DEBUG << "TrigVertexCnv_p1::transToPers: No tracks associated to the vertex" << endmsg;
  }
  
}
