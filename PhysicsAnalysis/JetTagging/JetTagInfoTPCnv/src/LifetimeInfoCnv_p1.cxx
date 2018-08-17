/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the LifetimeInfo persistent/transient classes
///

#include "JetTagInfo/LifetimeInfo.h"
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
    persObj->m_trackSIP.assign (transObj->signedIP().begin(),
                                transObj->signedIP().end());
    persObj->m_trackSignificance.assign (transObj->significance().begin(),
                                         transObj->significance().end());
    persObj->m_vectorOfTrackProb.assign (transObj->vectorTrackProb().begin(),
                                         transObj->vectorTrackProb().end());
    persObj->m_nTrackProb = transObj->nTrackProb();
  }

  void LifetimeInfoCnv_p1::persToTrans (const LifetimeInfo_p1 *persObj,
				     LifetimeInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    std::vector<double> v;
    v.assign (persObj->m_trackSIP.begin(), persObj->m_trackSIP.end());
    transObj->setIP(std::move(v));
    v.assign (persObj->m_trackSignificance.begin(), persObj->m_trackSignificance.end());
    transObj->setSignificance(std::move(v));
    v.assign (persObj->m_vectorOfTrackProb.begin(), persObj->m_vectorOfTrackProb.end());
    transObj->setTrackProb (std::move(v));
    transObj->setNTrackProb (persObj->m_nTrackProb);
  }

}
