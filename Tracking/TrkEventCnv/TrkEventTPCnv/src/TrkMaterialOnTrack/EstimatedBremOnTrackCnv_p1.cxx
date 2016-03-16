/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrackCnv_p1.h"

void EstimatedBremOnTrackCnv_p1 :: persToTrans(
   const Trk::EstimatedBremOnTrack_p1 *persObj,
   Trk::EstimatedBremOnTrack          *transObj,
   MsgStream& log) 
{
   fillTransFromPStore( &m_mefBaseCnv, persObj->m_mefBase, transObj, log );
   *transObj = Trk::EstimatedBremOnTrack (transObj->thicknessInX0(),
                                          persObj->m_retainedEnFraction,
                                          persObj->m_sigmaRetEnFraction,
                                          persObj->m_sigmaQoverPsquared,
                                          transObj->associatedSurface(),
                                          static_cast<Trk::SearchDirection>(persObj->m_direction));
}

void EstimatedBremOnTrackCnv_p1 :: transToPers(
   const Trk::EstimatedBremOnTrack *transObj,
   Trk::EstimatedBremOnTrack_p1  *persObj,
   MsgStream& log) 
{
  persObj->m_mefBase = baseToPersistent( &m_mefBaseCnv, transObj, log );
  persObj->m_retainedEnFraction = (float)transObj->retainedEnFraction();
  persObj->m_sigmaRetEnFraction = (float)transObj->sigmaRetainedEnFraction();
  persObj->m_sigmaQoverPsquared = (float)transObj->sigmaQoverPsquared();
  persObj->m_direction = static_cast<signed char>(transObj->searchDirection());
}



