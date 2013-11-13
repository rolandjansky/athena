/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the TruthInfo persistent/transient classes
///

#define private public
#include "JetTagInfo/AtlfInfo.h"
#undef private

#include "JetTagInfoTPCnv/AtlfInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

namespace Analysis {

  void AtlfInfoCnv_p1::transToPers (const AtlfInfo *transObj,
				    AtlfInfo_p1 *persObj,
				    MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, transObj, msg);

    /// Now, our particular members.
    persObj->m_isBTagged = transObj->m_isBTagged;
    persObj->m_isTauTagged = transObj->m_isTauTagged;
    persObj->m_isTau1PTagged = transObj->m_isTau1PTagged;
    persObj->m_isTau3PTagged = transObj->m_isTau3PTagged;
    
    persObj->m_calFactLight = transObj->m_calFactLight;
    persObj->m_calFactTau = transObj->m_calFactTau;
    persObj->m_calFactTau1P3P = transObj->m_calFactTau1P3P;
    persObj->m_calFactB = transObj->m_calFactB;

    persObj->m_distanceToBQuark = transObj->m_distanceToBQuark;
    persObj->m_distanceToCQuark = transObj->m_distanceToCQuark;
    persObj->m_distanceToTLepton = transObj->m_distanceToTLepton;
  }

  void AtlfInfoCnv_p1::persToTrans (const AtlfInfo_p1 *persObj,
				     AtlfInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);
    //m_baseTagCnv.persToTrans(&persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_isBTagged = persObj->m_isBTagged;
    transObj->m_isTauTagged = persObj->m_isTauTagged;
    transObj->m_isTau1PTagged = persObj->m_isTau1PTagged;
    transObj->m_isTau3PTagged = persObj->m_isTau3PTagged;
    
    transObj->m_calFactLight = persObj->m_calFactLight;
    transObj->m_calFactTau = persObj->m_calFactTau;
    transObj->m_calFactTau1P3P = persObj->m_calFactTau1P3P;
    transObj->m_calFactB = persObj->m_calFactB;

    transObj->m_distanceToBQuark = persObj->m_distanceToBQuark;
    transObj->m_distanceToCQuark = persObj->m_distanceToCQuark;
    transObj->m_distanceToTLepton = persObj->m_distanceToTLepton;

  }

}
