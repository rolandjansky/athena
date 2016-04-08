/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Convert the sl truth info
///

#include "JetTagInfo/SLTrueInfo.h"
#include "JetTagInfoTPCnv/SLTrueInfoCnv_p1.h"

namespace Analysis {

  void SLTrueInfoCnv_p1::transToPers (const SLTrueInfo *transObj,
				       SLTrueInfo_p1 *persObj,
				       MsgStream &)
  {
    persObj->m_barcode = transObj->barcode();
    persObj->m_pdgCode = transObj->pdgId();
    persObj->m_pdgCodeMother = transObj->pdgIdMother();
    persObj->m_isFromBhadron = transObj->FromB();
    persObj->m_isFromDhadron = transObj->FromD();
    persObj->m_isFromGHboson = transObj->FromGH();
    persObj->m_Momentum = transObj->momentum();
    persObj->m_ProductionVertex = transObj->prodvtx();
  }

  void SLTrueInfoCnv_p1::persToTrans (const SLTrueInfo_p1 *persObj,
				       SLTrueInfo *transObj,
				       MsgStream &)
  {

    *transObj = SLTrueInfo (persObj->m_barcode,
                            persObj->m_pdgCode,
                            persObj->m_pdgCodeMother,
                            persObj->m_isFromBhadron,
                            persObj->m_isFromDhadron,
                            persObj->m_isFromGHboson,
                            persObj->m_Momentum,
                            persObj->m_ProductionVertex);
  }
}
