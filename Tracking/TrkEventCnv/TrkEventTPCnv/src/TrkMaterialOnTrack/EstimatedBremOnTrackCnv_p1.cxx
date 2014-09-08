/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#undef private

#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrackCnv_p1.h"

void EstimatedBremOnTrackCnv_p1 :: persToTrans(
   const Trk::EstimatedBremOnTrack_p1 *persObj,
   Trk::EstimatedBremOnTrack          *transObj,
   MsgStream& log) 
{
   fillTransFromPStore( &m_mefBaseCnv, persObj->m_mefBase, transObj, log );
   transObj->m_retainedEnFraction = persObj->m_retainedEnFraction;
   transObj->m_sigmaRetainedEnFraction = persObj->m_sigmaRetEnFraction;
   transObj->m_sigmaQoverPsquared = persObj->m_sigmaQoverPsquared;
   transObj->m_bremSearchDirection = static_cast<Trk::SearchDirection>(persObj->m_direction);
}

void EstimatedBremOnTrackCnv_p1 :: transToPers(
   const Trk::EstimatedBremOnTrack *transObj,
   Trk::EstimatedBremOnTrack_p1  *persObj,
   MsgStream& log) 
{
  persObj->m_mefBase = baseToPersistent( &m_mefBaseCnv, transObj, log );
  persObj->m_retainedEnFraction = (float)transObj->m_retainedEnFraction;
  persObj->m_sigmaRetEnFraction = (float)transObj->m_sigmaRetainedEnFraction;
  persObj->m_sigmaQoverPsquared = (float)transObj->m_sigmaQoverPsquared;
  persObj->m_direction = static_cast<signed char>(transObj->m_bremSearchDirection);
}



