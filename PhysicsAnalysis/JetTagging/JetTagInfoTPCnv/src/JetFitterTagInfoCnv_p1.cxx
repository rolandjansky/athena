/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#define private public
#include "JetTagInfo/JetFitterTagInfo.h"
#undef private
#include "JetTagInfoTPCnv/JetFitterTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void JetFitterTagInfoCnv_p1::transToPers(const JetFitterTagInfo* pa, JetFitterTagInfo_p1* pb, MsgStream & msg) {
    pb->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    pb->m_nVTX = pa->m_nVTX;
    pb->m_nSingleTracks = pa->m_nSingleTracks;
    pb->m_nTracksAtVtx = pa->m_nTracksAtVtx;
    pb->m_mass = pa->m_mass;
    pb->m_energyFraction = pa->m_energyFraction;
    pb->m_significance3d = pa->m_significance3d;
    pb->m_deltaeta = pa->m_deltaeta;
    pb->m_deltaphi = pa->m_deltaphi;
  }

  void JetFitterTagInfoCnv_p1::persToTrans(const JetFitterTagInfo_p1* pa, JetFitterTagInfo* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    pb->m_nVTX = pa->m_nVTX;
    pb->m_nSingleTracks = pa->m_nSingleTracks;
    pb->m_nTracksAtVtx = pa->m_nTracksAtVtx;
    pb->m_mass = pa->m_mass;
    pb->m_energyFraction = pa->m_energyFraction;
    pb->m_significance3d = pa->m_significance3d;
    pb->m_deltaeta = pa->m_deltaeta;
    pb->m_deltaphi = pa->m_deltaphi;
  }

}
