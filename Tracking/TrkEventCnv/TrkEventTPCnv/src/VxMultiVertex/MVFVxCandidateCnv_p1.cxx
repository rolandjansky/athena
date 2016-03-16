/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MVFVxCandidateCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "VxMultiVertex/MVFVxCandidate.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidateCnv_p1.h"

void  MVFVxCandidateCnv_p1::persToTrans(const Trk::MVFVxCandidate_p1 * persObj, Trk::MVFVxCandidate * transObj, MsgStream &log)
{
 fillTransFromPStore( &m_vxCandidateConverter, persObj->m_vxCandidate, transObj, log);
}

void  MVFVxCandidateCnv_p1::transToPers(const Trk::MVFVxCandidate * transObj, Trk::MVFVxCandidate_p1 * persObj, MsgStream &log)
{
 persObj->m_vxCandidate = baseToPersistent(&m_vxCandidateConverter,  transObj, log );
}
