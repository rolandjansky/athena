/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileFCS_StepInfoSD
// Sensitive detector for TileCal G4 simulations using TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

#ifndef ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H
#define ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H

// Base class header
#include "FCS_StepInfoSD.h"

// Athena headers
#include "CaloIdentifier/TileID.h"

class G4Step;
class G4TouchableHistory;
class ITileCalculator;

class TileFCS_StepInfoSD: public FCS_StepInfoSD {
public:
  TileFCS_StepInfoSD(G4String name, const FCS_Param::Config& config);
  ~TileFCS_StepInfoSD();

  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  // Don't leave copy constructors or assignment operators around
  TileFCS_StepInfoSD(const TileFCS_StepInfoSD&) = delete;
  TileFCS_StepInfoSD& operator=(const TileFCS_StepInfoSD&) = delete;

private:
  /// A copy of the method in TileCablingService.cxx
  bool C10_connected(int module) const;
  /// Some Tile Cells are only read out by one PMT rather than two.
  bool cellReadOutByOnePMT(const Identifier& id) const;

  ITileCalculator* m_calculator;
};

inline bool TileFCS_StepInfoSD::C10_connected(int module) const {
  const bool C = true;
  const bool NC = false;
  const bool cell_present[64] = {
    // counting from 1 in comments
    C, C, C, C, C, C, C, C, // 1-8
    C, C, C, C, C, C, C, C, // 9-16
    C, C, C, C, C, C, C, C, // 17-24
    C, C, C, C, C, C, C, C, // 25-32
    C, C, C, C, C, C,NC,NC, // 33-40  39-40 do not exist
    NC,NC, C, C, C, C, C, C, // 41-48  41-42 do not exist
    C, C, C, C, C, C,NC,NC, // 49-56  55-56 do not exist
    NC,NC, C, C, C, C, C, C  // 57-64  57-58 do not exist
  };
  return cell_present[module];
}

inline bool TileFCS_StepInfoSD::cellReadOutByOnePMT(const Identifier& id) const {
  return m_tileID->is_tile_gapscin(id) || // gap/crack cells
    ((m_tileID->section(id) == TileID::GAPDET && m_tileID->sample(id) == TileID::SAMP_C && !(this->C10_connected(m_tileID->module(id))) )); //Special C10 cells

}

#endif // ISF_FASTCALOSIMSD_TILEFCS_STEPINFOSD_H

