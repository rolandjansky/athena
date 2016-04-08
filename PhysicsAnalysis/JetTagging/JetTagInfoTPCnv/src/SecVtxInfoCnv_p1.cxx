/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SecVtxInfo persistent/transient classes
///

#include "JetTagInfo/SecVtxInfo.h"
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
    persObj->m_dist = transObj->distance();
    persObj->m_rphidist = transObj->rphidistance();
    persObj->m_prob = transObj->probability();
    persObj->m_mass = transObj->mass();
    persObj->m_energyFraction = transObj->energyFraction();
    persObj->m_mult = transObj->mult();
    persObj->m_NGood2TrackVertices = transObj->NumberOfG2TrackVertices();
    persObj->m_fitType = transObj->fitType();

    persObj->m_secVtxPos = toPersistent (&m_recoVertexCnv,
					 &transObj->secVertexPos(),
					 msg);

  }

  void SecVtxInfoCnv_p1::persToTrans (const SecVtxInfo_p1 *persObj,
				      SecVtxInfo *transObj,
				      MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->setDist (persObj->m_dist);
    transObj->setRPhiDist (persObj->m_rphidist);
    transObj->setMass (persObj->m_mass);
    transObj->setEnergyFraction (persObj->m_energyFraction);
    transObj->setMult (persObj->m_mult);
    transObj->setNumberOfG2TrackVertices (persObj->m_NGood2TrackVertices);
    transObj->setFitType ((SecVtxInfo::FitType) persObj->m_fitType);

    Trk::RecVertex secVtx;
    fillTransFromPStore(&m_recoVertexCnv,
			persObj->m_secVtxPos,
			&secVtx,
			msg);
    transObj->setSecVtx (secVtx, persObj->m_prob, TrackVec());
  }

}
