/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/src/LuminosityCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Hold luminosity data produced by LuminosityCondAlg.
 */


#include "LumiBlockData/LuminosityCondData.h"


float
LuminosityCondData::lbAverageLuminosity() const {
  return m_LBAvInstLumi;
}

void
LuminosityCondData::setLbAverageLuminosity (float val) {
  m_LBAvInstLumi = val;
}

float
LuminosityCondData::lbAverageInteractionsPerCrossing() const {
  return m_LBAvEvtsPerBX;
}

void
LuminosityCondData::setLbAverageInteractionsPerCrossing (float val) {
  m_LBAvEvtsPerBX = val;
}

unsigned int
LuminosityCondData::lbAverageValid() const {
  return m_Valid;
}

void
LuminosityCondData::setLbAverageValid (uint32_t val) {
  m_Valid = val;
}

const std::vector<float>&
LuminosityCondData::lbLuminosityPerBCIDVector() const {
  return m_LBInstLumi;
}

void
LuminosityCondData::setLbLuminosityPerBCIDVector (std::vector<float>&& val) {
  m_LBInstLumi = std::move(val);
}

float
LuminosityCondData::muToLumi() const {
  return m_MuToLumi;
}

void
LuminosityCondData::setMuToLumi (float val) {
  m_MuToLumi = val;
}

