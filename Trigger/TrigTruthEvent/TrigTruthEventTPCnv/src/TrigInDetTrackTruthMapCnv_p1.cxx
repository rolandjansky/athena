/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_p1.h" 
#include "TrigTruthEventTPCnv/TrigInDetTrackTruthCnv_p1.h"

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<TrigInDetTrackCollection> > trackVectCnv;
//static TrigInDetTrackTruthCnv_p1  trigInDetTrackTruthCnv;


//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackTruthMapCnv_p1::persToTrans( const TrigInDetTrackTruthMap_p1 *persObj,
					     TrigInDetTrackTruthMap    *transObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigInDetTrackTruthMapCnv_p1::persToTrans called " << endreq;

  transObj->m_elink_vec.clear();
  trackVectCnv.persToTrans(&persObj->m_elink_vec, &transObj->m_elink_vec, log);

  int n_tracks = (persObj->m_truth_vec).size();
  transObj->m_truth_vec.clear();
  transObj->m_truth_vec.resize( n_tracks );

  int index=0;
  for(std::vector<TPObjRef>::const_iterator it = persObj->m_truth_vec.begin();
      it!=persObj->m_truth_vec.end();++it)
    {
      fillTransFromPStore( &m_trigInDetTrackTruthCnv,(*it), &(transObj->m_truth_vec[index]), log  );   
      index++;
    }
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackTruthMapCnv_p1::transToPers( const TrigInDetTrackTruthMap    *transObj,
					     TrigInDetTrackTruthMap_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigInDetTrackTruthMapCnv_p1::transToPers called " << endreq;

  trackVectCnv.transToPers(&transObj->m_elink_vec, &persObj->m_elink_vec, log);

  int n_tracks = transObj->m_truth_vec.size();
  persObj->m_truth_vec.clear();
  persObj->m_truth_vec.reserve( n_tracks );

  for(std::vector<TrigInDetTrackTruth>::const_iterator it = transObj->m_truth_vec.begin();
      it!=transObj->m_truth_vec.end();++it)
    {
      persObj->m_truth_vec.push_back(toPersistent( &m_trigInDetTrackTruthCnv, &(*it), log  ));    
    }

}
