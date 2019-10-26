// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/LuminosityCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Hold luminosity data produced by LuminosityCondAlg.
 */


#ifndef LUMIBLOCKDATA_LUMINOSITYCONDDATA_H
#define LUMIBLOCKDATA_LUMINOSITYCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


class LuminosityCondData
{
public:
  // Total number of BCIDs in one turn - this must match value used
  // in /TDAQ/OLC/BUNCHLUMIS for storage mode 1
  static constexpr unsigned int TOTAL_LHC_BCIDS = 3564;

  // Instantaneous luminosity information from /TRIGGER/LUMI/LBLESTONL or LBLESTOFL
    // In ub-1 s-1 => 10^30 cm-2 s-1
  float lbAverageLuminosity() const;
  float lbAverageInteractionsPerCrossing() const;
  unsigned int lbAverageValid() const; // Validity flag from COOL

  // Instantaneous luminosity per BCID
  // Non-zero values only filled for BCIDs in the physics bunch group, or found to have collisions by the bunch currents

  // Return vector with all BCIDs indexed by bcid number
  const std::vector<float>& lbLuminosityPerBCIDVector() const;  // In ub-1 s-1 => 10^30 cm-2 s-1

  // Conversion factor from OLC.  lumiPerBCID/muToLumi = interactionsPerCrossingPerBCID
  float muToLumi() const;


  // Setter methods.
  void setLbAverageLuminosity (float val);
  void setLbAverageInteractionsPerCrossing (float val);
  void setLbAverageValid (uint32_t val);
  void setLbLuminosityPerBCIDVector (std::vector<float>&& val);
  void setMuToLumi (float val);


private:
  // Data from LBLESTOFL
  float m_LBAvInstLumi = 0; // in ub-1 s-1 => 10^30 cm-2 s-1
  float m_LBAvEvtsPerBX = 0;
  uint32_t m_Valid = 0xffffffff;

  // Luminosity per beam crossing (for a specific BCID)
  std::vector<float> m_LBInstLumi = std::vector<float> (TOTAL_LHC_BCIDS);

  // Conversion factor from calibrations
  float m_MuToLumi = 0;
};


CLASS_DEF (LuminosityCondData, 222148049, 0)
CONDCONT_MIXED_DEF (LuminosityCondData, 180584023);


#endif // not LUMIBLOCKDATA_LUMINOSITYCONDDATA_H
