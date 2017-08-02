/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
// Filename : TileGeoG4SDCalc.hh
// Author   : Sergey Karpov <Sergey.Karpov@cern.ch>
// Created  : July, 2013
//
// DESCRIPTION:
//   Sensitive Detector initialisation for TileCal G4 simulations
//   of both ordinary Hits and CalibHits
//
// HISTORY:
//   Nov 2013 - Work with U-shape was added (Sasha Solodkov)
//
//************************************************************

#ifndef TILEG4INTERFACES_ITILECALCULATOR_H
#define TILEG4INTERFACES_ITILECALCULATOR_H

// Geant4 headers
#include "G4Types.hh"

#include "GaudiKernel/IService.h"
#include "Identifier/Identifier.h"

class G4Step;
class TileGeoG4Section;
class TileGeoG4Cell;
class TileGeoG4LookupBuilder;

struct TileMicroHit {
  Identifier pmt_up;
  Identifier pmt_down;
  G4double e_up;
  G4double e_down;
  double time_up;
  double time_down;
  //int         period,     tilerow; // prepared for future use
};

/// Variables to identify Hit objects
struct TileHitData {
  TileGeoG4Section *section = nullptr;
  TileGeoG4Cell *cell = nullptr;
  int nModule = 0;
  int nDetector = 0;
  int nTower = 0;
  int nSample = 0;
  int nSide = 0;
  int nrOfPMT = 0;
  int tileSize = 0;
  int tilePeriod = 0;
  double totalTimeUp = 0.0;
  double totalTimeDown = 0.0;
  bool isNegative = false;
  Identifier pmtID_up;
  Identifier pmtID_down;
  G4double edep_up = 0.0;
  G4double edep_down = 0.0;
  double scin_Time_up = 0.0;
  double scin_Time_down = 0.0;
};

class ITileCalculator : virtual public IService {
  public:
  ///
  ITileCalculator() {}
  ///
  virtual ~ITileCalculator() {}
  ///
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ITileCalculator( "ITileCalculator" , 1 , 0 ) ;
    return IID_ITileCalculator;
  }

  /// Search for the tilecal sub-section, its module and some identifiers
  virtual G4bool FindTileScinSection(const G4Step*, TileHitData& hitData) const = 0;
  /// Calculation of pmtID, edep and scin_Time with aStep (Sergey)
  virtual G4bool MakePmtEdepTime(const G4Step*, TileHitData& hitData) const = 0;
  /// Calculation of pmtID, edep and scin_Time with aStep (Sergey)
  virtual G4bool ManageScintHit(TileHitData& hitData) const = 0;
  /// Used by FastCaloSimParamAction
  virtual TileMicroHit GetTileMicroHit(const G4Step*, TileHitData& hitData) const = 0;
  ///
  virtual TileGeoG4LookupBuilder* GetLookupBuilder() const = 0;
  /// Method used by TileFastCaloSim/TileFCSmStepToTileHitVec
  virtual void pmtEdepFromFCS_StepInfo(TileHitData& hitData, double ene, double yLocal, double halfYLocal, double zLocal, int Ushape) const = 0;

};
//class ITileCalculator

#endif // not TILEG4INTERFACES_ITILECALCULATOR_H
