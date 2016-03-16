/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkEventTPCnv/TrkV0Vertex/V0HypothesisCnv_p1.h"


void  V0HypothesisCnv_p1::persToTrans(const Trk::V0Hypothesis_p1 * persObj, Trk::V0Hypothesis * transObj, MsgStream &log)
{
// fillTransFromPStore( &m_extendedVxCandidateConverter, persObj->m_extendedVxCandidate,transObj, log);

 fillTransFromPStore( &m_vxCandidateConverter, persObj->m_extendedVxCandidate,transObj, log);
 transObj->m_positiveTrackID = persObj->m_positiveTrackID;
 transObj->m_negativeTrackID = persObj->m_negativeTrackID;
 transObj->m_constraintID    = persObj->m_constraintID;
}

void  V0HypothesisCnv_p1::transToPers(const Trk::V0Hypothesis * transObj, Trk::V0Hypothesis_p1 * persObj, MsgStream &log)
{
// persObj->m_extendedVxCandidate = baseToPersistent(&m_extendedVxCandidateConverter,  transObj, log );

 persObj->m_extendedVxCandidate = baseToPersistent(&m_vxCandidateConverter,  transObj, log );
 persObj->m_positiveTrackID = transObj->m_positiveTrackID;
 persObj->m_negativeTrackID = transObj->m_negativeTrackID;
 persObj->m_constraintID    = transObj->m_constraintID;
}
