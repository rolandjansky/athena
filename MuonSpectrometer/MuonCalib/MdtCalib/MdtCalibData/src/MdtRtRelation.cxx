/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/TrRelation.h"
#include "MdtCalibData/MdtRtRelation.h"
#include "MdtCalibData/TrRelation.h"

namespace MuonCalib {

MdtRtRelation::~MdtRtRelation() { 
  delete m_rt; delete m_rtRes;
  if (m_tr!=0) {
    delete m_tr;
  }
}

MdtRtRelation::MdtRtRelation( IRtRelation* rt, IRtResolution* reso, float t0 ): m_rt(rt), m_rtRes(reso), m_t0(t0)  {
  m_tr = 0;
  if (rt!=0) {
    m_tr = new TrRelation(*m_rt);
  }
}

}   //end namespace MuonCalib

