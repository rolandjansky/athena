// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/FillParamsCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds fill parameters data.
 */


#ifndef COOLLUMIUTILITIES_FILLPARAMSCONDDATA_H
#define COOLLUMIUTILITIES_FILLPARAMSCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <stdint.h>


class FillParamsCondData
{
public:
  // Arrays with BCID numbers for filled bunches
  const std::vector<unsigned int>& beam1Bunches() const;
  const std::vector<unsigned int>& beam2Bunches() const;
  const std::vector<unsigned int>& luminousBunches() const;

  // Set BCID arrays.
  void setBeam1Bunches (const uint16_t* beg, const uint16_t* end);
  void setBeam2Bunches (const uint16_t* beg, const uint16_t* end);
  void setLuminousBunches (const uint16_t* beg, const uint16_t* end);


private:
  // Data
  std::vector<unsigned int> m_beam1Bunches;
  std::vector<unsigned int> m_beam2Bunches;
  std::vector<unsigned int> m_luminousBunches;
};


CLASS_DEF (FillParamsCondData, 46982539, 0)
CONDCONT_DEF (FillParamsCondData, 245762757);


#endif // not COOLLUMIUTILITIES_FILLPARAMSCONDDATA_H
