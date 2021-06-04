/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/MuonAlignmentErrorData.h"

MuonAlignmentErrorData::MuonAlignmentErrorData() : m_test(0), m_vec(0) {}

MuonAlignmentErrorData::~MuonAlignmentErrorData() {}

deviationStr::deviationStr() : traslation(0.), rotation(0.), stationName(""), multilayer("") {}

deviationStr::~deviationStr() {}

void MuonAlignmentErrorData::setVec(const std::vector<deviationStr> vec) { m_vec = vec; }

void MuonAlignmentErrorData::getVec(std::vector<deviationStr>& vec) const { vec = m_vec; }

void MuonAlignmentErrorData::clearVec() { m_vec.clear(); }
