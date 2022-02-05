/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/MdtRtRelation.h"

#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/TrRelation.h"

namespace MuonCalib {

    MdtRtRelation::~MdtRtRelation() {
        delete m_rt;
        delete m_rtRes;
        if (m_tr != nullptr) { delete m_tr; }
    }

    MdtRtRelation::MdtRtRelation(IRtRelation* rt, IRtResolution* reso, float t0) : m_rt(rt), m_rtRes(reso), m_t0(t0) {
        m_tr = nullptr;
        if (rt != nullptr) { m_tr = new TrRelation(*m_rt); }
    }

}  // end namespace MuonCalib
