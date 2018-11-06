/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#include "JetTagInfo/JetFitterTagInfo.h"
#include "JetTagInfoTPCnv/JetFitterTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void JetFitterTagInfoCnv_p1::transToPers(const JetFitterTagInfo* pa, JetFitterTagInfo_p1* pb, MsgStream & msg) {
    pb->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    pb->m_nVTX = pa->nVTX();
    pb->m_nSingleTracks = pa->nSingleTracks();
    pb->m_nTracksAtVtx = pa->nTracksAtVtx();
    pb->m_mass = pa->mass();
    pb->m_energyFraction = pa->energyFraction();
    pb->m_significance3d = pa->significance3d();
    pb->m_deltaeta = pa->deltaeta();
    pb->m_deltaphi = pa->deltaphi();
  }

  void JetFitterTagInfoCnv_p1::persToTrans(const JetFitterTagInfo_p1* pa, JetFitterTagInfo* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    pb->setnVTX (pa->m_nVTX);
    pb->setnSingleTracks (pa->m_nSingleTracks);
    pb->setnTracksAtVtx (pa->m_nTracksAtVtx);
    pb->setMass (pa->m_mass);
    pb->setEnergyFraction (pa->m_energyFraction);
    pb->setSignificance3d (pa->m_significance3d);
    pb->setDeltaeta (pa->m_deltaeta);
    pb->setDeltaphi (pa->m_deltaphi);
  }

}
