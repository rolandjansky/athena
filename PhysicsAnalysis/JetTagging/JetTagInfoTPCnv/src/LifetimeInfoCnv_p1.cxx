/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the LifetimeInfo persistent/transient classes
///

#define private public
#include "JetTagInfo/LifetimeInfo.h"
#undef private
#include "JetTagInfoTPCnv/LifetimeInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

namespace Analysis {

  void LifetimeInfoCnv_p1::transToPers (const LifetimeInfo *transObj,
				     LifetimeInfo_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_trackSIP.assign(transObj->m_trackSIP.begin(), transObj->m_trackSIP.end());
    persObj->m_trackSignificance.assign(transObj->m_trackSignificance.begin(), transObj->m_trackSignificance.end());
    persObj->m_vectorOfTrackProb.assign(transObj->m_vectorOfTrackProb.begin(), transObj->m_vectorOfTrackProb.end());
    persObj->m_nTrackProb = transObj->m_nTrackProb;
  }

  void LifetimeInfoCnv_p1::persToTrans (const LifetimeInfo_p1 *persObj,
				     LifetimeInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_trackSIP.assign(persObj->m_trackSIP.begin(), persObj->m_trackSIP.end());
    transObj->m_trackSignificance.assign(persObj->m_trackSignificance.begin(), persObj->m_trackSignificance.end());
    transObj->m_vectorOfTrackProb.assign(persObj->m_vectorOfTrackProb.begin(), persObj->m_vectorOfTrackProb.end());
    transObj->m_nTrackProb = persObj->m_nTrackProb;
  }

}
