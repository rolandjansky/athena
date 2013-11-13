/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Convert the sl truth info
///

#define private public
#include "JetTagInfo/SLTrueInfo.h"
#undef private

#include "JetTagInfoTPCnv/SLTrueInfoCnv_p1.h"

namespace Analysis {

  void SLTrueInfoCnv_p1::transToPers (const SLTrueInfo *transObj,
				       SLTrueInfo_p1 *persObj,
				       MsgStream &)
  {
    persObj->m_barcode = transObj->m_barcode;
    persObj->m_pdgCode = transObj->m_pdgCode;
    persObj->m_pdgCodeMother = transObj->m_pdgCodeMother;
    persObj->m_isFromBhadron = transObj->m_isFromBhadron;
    persObj->m_isFromDhadron = transObj->m_isFromDhadron;
    persObj->m_isFromGHboson = transObj->m_isFromGHboson;
    persObj->m_Momentum = transObj->m_Momentum;
    persObj->m_ProductionVertex = transObj->m_ProductionVertex;
  }

  void SLTrueInfoCnv_p1::persToTrans (const SLTrueInfo_p1 *persObj,
				       SLTrueInfo *transObj,
				       MsgStream &)
  {

    transObj->m_barcode = persObj->m_barcode;
    transObj->m_pdgCode = persObj->m_pdgCode;
    transObj->m_pdgCodeMother = persObj->m_pdgCodeMother;
    transObj->m_isFromBhadron = persObj->m_isFromBhadron;
    transObj->m_isFromDhadron = persObj->m_isFromDhadron;
    transObj->m_isFromGHboson = persObj->m_isFromGHboson;
    transObj->m_Momentum = persObj->m_Momentum;
    transObj->m_ProductionVertex = persObj->m_ProductionVertex;
  }
}
