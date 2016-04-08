/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the TruthInfo persistent/transient classes
///

#include "JetTagInfo/TruthInfo.h"
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

    static const std::string s_B("B");
    static const std::string s_C("C");
    static const std::string s_T("T");

    /// Now, our particular members.
    persObj->m_jetTruthLabel = transObj->jetTruthLabel();
    persObj->m_distanceToBQuark = transObj->deltaRMinTo(s_B);
    persObj->m_distanceToCQuark = transObj->deltaRMinTo(s_C);
    persObj->m_distanceToTLepton = transObj->deltaRMinTo(s_T);
    persObj->m_BHadronDecayVertex = transObj->BDecayVertex();
    persObj->m_BHadronPdg = transObj->BHadronPdg();
  }

  void TruthInfoCnv_p1::persToTrans (const TruthInfo_p1 *persObj,
				     TruthInfo *transObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    static const std::string s_B("B");
    static const std::string s_C("C");
    static const std::string s_T("T");

    /// Now, our particular members.
    transObj->setJetTruthLabel (persObj->m_jetTruthLabel);
    transObj->deltaRMinTo (s_B, persObj->m_distanceToBQuark);
    transObj->deltaRMinTo (s_C, persObj->m_distanceToCQuark);
    transObj->deltaRMinTo (s_T, persObj->m_distanceToTLepton);
    transObj->BDecayVertex (persObj->m_BHadronDecayVertex);
    transObj->BHadronPdg (persObj->m_BHadronPdg);
  }

}
