/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the TruthInfo persistent/transient classes
///

#define private public
#include "JetTagInfo/TruthInfo.h"
#undef private
#include "JetTagInfoTPCnv/TruthInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

namespace Analysis {

  void TruthInfoCnv_p1::transToPers (const TruthInfo *transObj,
				     TruthInfo_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_jetTruthLabel = transObj->m_jetTruthLabel;
    persObj->m_distanceToBQuark = transObj->m_distanceToBQuark;
    persObj->m_distanceToCQuark = transObj->m_distanceToCQuark;
    persObj->m_distanceToTLepton = transObj->m_distanceToTLepton;
    persObj->m_BHadronDecayVertex = transObj->m_BHadronDecayVertex;
    persObj->m_BHadronPdg = transObj->m_BHadronPdg;
  }

  void TruthInfoCnv_p1::persToTrans (const TruthInfo_p1 *persObj,
				     TruthInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_jetTruthLabel = persObj->m_jetTruthLabel;
    transObj->m_distanceToBQuark = persObj->m_distanceToBQuark;
    transObj->m_distanceToCQuark = persObj->m_distanceToCQuark;
    transObj->m_distanceToTLepton = persObj->m_distanceToTLepton;
    transObj->m_BHadronDecayVertex = persObj->m_BHadronDecayVertex;
    transObj->m_BHadronPdg = persObj->m_BHadronPdg;
  }

}
