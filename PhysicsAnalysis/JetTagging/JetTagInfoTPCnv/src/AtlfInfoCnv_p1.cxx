/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the TruthInfo persistent/transient classes
///

#include "JetTagInfo/AtlfInfo.h"
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
    persObj->m_isBTagged = transObj->isBTagged();
    persObj->m_isTauTagged = transObj->isTauTagged();
    persObj->m_isTau1PTagged = transObj->isTau1PTagged();
    persObj->m_isTau3PTagged = transObj->isTau3PTagged();
    
    persObj->m_calFactLight = transObj->LightHypoCalFactor();
    persObj->m_calFactTau = transObj->TauHypoCalFactor();
    persObj->m_calFactTau1P3P = transObj->Tau1P3PHypoCalFactor();
    persObj->m_calFactB = transObj->BHypoCalFactor();

    persObj->m_distanceToBQuark = transObj->deltaRMinTo ("B");
    persObj->m_distanceToCQuark = transObj->deltaRMinTo ("C");
    persObj->m_distanceToTLepton = transObj->deltaRMinTo ("T");
  }

  void AtlfInfoCnv_p1::persToTrans (const AtlfInfo_p1 *persObj,
				     AtlfInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);
    //m_baseTagCnv.persToTrans(&persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->isBTagged (persObj->m_isBTagged);
    transObj->isTauTagged (persObj->m_isTauTagged);
    transObj->isTau1PTagged (persObj->m_isTau1PTagged);
    transObj->isTau3PTagged (persObj->m_isTau3PTagged);
    
    transObj->LightHypoCalFactor (persObj->m_calFactLight);
    transObj->TauHypoCalFactor (persObj->m_calFactTau);
    transObj->Tau1P3PHypoCalFactor (persObj->m_calFactTau1P3P);
    transObj->BHypoCalFactor (persObj->m_calFactB);

    transObj->deltaRMinTo ("B", persObj->m_distanceToBQuark);
    transObj->deltaRMinTo ("C", persObj->m_distanceToCQuark);
    transObj->deltaRMinTo ("T", persObj->m_distanceToTLepton);

  }

}
