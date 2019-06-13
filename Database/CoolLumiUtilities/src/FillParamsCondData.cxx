/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/FillParamsCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds fill parameters data.
 */


#include "CoolLumiUtilities/FillParamsCondData.h"


const std::vector<unsigned int>& FillParamsCondData::beam1Bunches() const
{
  return m_beam1Bunches;
}


const std::vector<unsigned int>& FillParamsCondData::beam2Bunches() const
{
  return m_beam2Bunches;
}


const std::vector<unsigned int>& FillParamsCondData::luminousBunches() const
{
  return m_luminousBunches;
}


void FillParamsCondData::setBeam1Bunches (const uint16_t* beg,
                                          const uint16_t* end)
{
  m_beam1Bunches.assign (beg, end);
}


void FillParamsCondData::setBeam2Bunches (const uint16_t* beg,
                                          const uint16_t* end)
{
  m_beam2Bunches.assign (beg, end);
}


void FillParamsCondData::setLuminousBunches (const uint16_t* beg,
                                             const uint16_t* end)
{
  m_luminousBunches.assign (beg, end);
}
