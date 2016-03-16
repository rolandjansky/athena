/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEventTPCnv/TrigVertex_p2.h"
#include "TrigInDetEventTPCnv/TrigVertexCnv_p2.h"


//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigVertexCnv_p2::persToTrans( const TrigVertex_p2 *persObj,
				    TrigVertex    *transObj,
				    MsgStream       &log )
{
  log << MSG::DEBUG << "TrigVertexCnv_p2::persToTrans called " << endreq;
  
  transObj->m_x                 = persObj->m_allFloats[0];
  transObj->m_y                 = persObj->m_allFloats[1];
  transObj->m_z                 = persObj->m_allFloats[2];
  transObj->m_mass              = persObj->m_allFloats[3];
  transObj->m_massVar           = persObj->m_allFloats[4];
  for(int i=0;i<6;i++){
    transObj->m_cov[i]          = persObj->m_allFloats[5+i];
  }
  transObj->m_energyFraction    = persObj->m_allFloats[11];
  transObj->m_chiSquared        = persObj->m_allFloats[12];

  transObj->m_nTwoTracksSecVtx  = persObj->m_allInts[0];
  transObj->m_nDOF              = persObj->m_allInts[1]; 
  transObj->m_algId             = static_cast<TrigVertex::AlgoId>(persObj->m_allInts[2]) ;
  
  if(&(persObj->m_tracks)!=NULL){  
    m_TrigInDetTrackVectorCnv.persToTrans( &(persObj->m_tracks), &m_trackInVertexVector, log);
    if(m_trackInVertexVector.size() != 0){

      //TrackInVertexList L;
      //L.clear();
      //
      //TrackInVertexVector::const_iterator iVBeg = m_trackInVertexVector.begin();
      //TrackInVertexVector::const_iterator iVEnd = m_trackInVertexVector.end();
      //for(TrackInVertexVector::const_iterator iV=iVBeg; iV != iVEnd; ++iV){
      //	L.push_back(*iV);
      //}
      //	
      //transObj->m_tracks = new TrackInVertexList;
      //*(transObj->m_tracks) = L;
      
      if (transObj->m_tracks)
        transObj->m_tracks->assign (m_trackInVertexVector.begin(),
                                    m_trackInVertexVector.end());
      else
        transObj->m_tracks =
          new TrackInVertexList (m_trackInVertexVector.begin(),
                                 m_trackInVertexVector.end());
      transObj->m_ownTracks = true;
      

    }
  } else {
    log << MSG::DEBUG << "TrigVertexCnv_p2::persToTrans: No tracks associated to the vertex" << endreq;
  }
  
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigVertexCnv_p2::transToPers( const TrigVertex    *transObj,
				    TrigVertex_p2 *persObj ,
				    MsgStream     &log      )
{
  log << MSG::DEBUG << "TrigVertexCnv_p2::transToPers called " << endreq;

  persObj->m_allFloats[0]      = transObj->m_x                 ;
  persObj->m_allFloats[1]      = transObj->m_y                 ;
  persObj->m_allFloats[2]      = transObj->m_z                 ;
  persObj->m_allFloats[3]      = transObj->m_mass              ;
  persObj->m_allFloats[4]      = transObj->m_massVar           ;
  persObj->m_allFloats[5]      = transObj->m_cov[0]            ;
  persObj->m_allFloats[6]      = transObj->m_cov[1]            ;
  persObj->m_allFloats[7]      = transObj->m_cov[2]            ;
  persObj->m_allFloats[8]      = transObj->m_cov[3]            ;
  persObj->m_allFloats[9]      = transObj->m_cov[4]            ;
  persObj->m_allFloats[10]     = transObj->m_cov[5]            ;
  persObj->m_allFloats[11]     = transObj->m_energyFraction    ;
  persObj->m_allFloats[12]     = transObj->m_chiSquared        ;

  persObj->m_allInts[0]        = transObj->m_nTwoTracksSecVtx  ;
  persObj->m_allInts[1]        = transObj->m_nDOF              ;
  persObj->m_allInts[2]        = transObj->m_algId             ;
  
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
    log << MSG::DEBUG << "TrigVertexCnv_p2::transToPers: No tracks associated to the vertex" << endreq;
  }
  
}
