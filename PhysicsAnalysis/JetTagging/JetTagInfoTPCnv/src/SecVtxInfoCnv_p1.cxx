/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SecVtxInfo persistent/transient classes
///

#define private public
#include "JetTagInfo/SecVtxInfo.h"
#undef private
#include "JetTagInfoTPCnv/SecVtxInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"

namespace Analysis {

  void SecVtxInfoCnv_p1::transToPers (const SecVtxInfo *transObj,
				     SecVtxInfo_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_dist = transObj->m_dist;
    persObj->m_rphidist = transObj->m_rphidist;
    persObj->m_prob = transObj->m_prob;
    persObj->m_mass = transObj->m_mass;
    persObj->m_energyFraction = transObj->m_energyFraction;
    persObj->m_mult = transObj->m_mult;
    persObj->m_NGood2TrackVertices = transObj->m_NGood2TrackVertices;
    persObj->m_fitType = transObj->m_fitType;

    persObj->m_secVtxPos = toPersistent (&m_recoVertexCnv,
					 &transObj->m_secVtxPos,
					 msg);

  }

  void SecVtxInfoCnv_p1::persToTrans (const SecVtxInfo_p1 *persObj,
				      SecVtxInfo *transObj,
				      MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_dist = persObj->m_dist;
    transObj->m_rphidist = persObj->m_rphidist;
    transObj->m_prob = persObj->m_prob;
    transObj->m_mass = persObj->m_mass;
    transObj->m_energyFraction = persObj->m_energyFraction;
    transObj->m_mult = persObj->m_mult;
    transObj->m_NGood2TrackVertices = persObj->m_NGood2TrackVertices;
    transObj->m_fitType = (SecVtxInfo::FitType) persObj->m_fitType;

    fillTransFromPStore(&m_recoVertexCnv,
			persObj->m_secVtxPos,
			&transObj->m_secVtxPos,
			msg);

  }

}
