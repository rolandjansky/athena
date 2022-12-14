/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#ifndef TILEGEOG4SD_TILEGEOG4SDCALC_H
#define TILEGEOG4SD_TILEGEOG4SDCALC_H


#include "TileG4Interfaces/ITileCalculator.h"
#include "AthenaBaseComps/AthService.h"

// package headers
#include "TileGeoG4SD/TileSDOptions.h"

// athena headers
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "CxxUtils/checker_macros.h"

// Geant4 headers
#include "G4Types.hh"

//CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

// STL headers
#include <vector>
#include <memory>

ATLAS_CHECK_FILE_THREAD_SAFETY;

class TileGeoG4LookupBuilder;
class G4Step;

class TileRow;
class TileGeoG4Section;
class TileGeoG4Cell;

class StoreGateSvc;
class IGeoModelSvc;

class TileGeoG4SDCalc: public extends<AthService, ITileCalculator> {
public:
  TileGeoG4SDCalc(const std::string& name, ISvcLocator * pSvcLocator);
  virtual ~TileGeoG4SDCalc();

  virtual StatusCode initialize() override final;

  /// Search for the tilecal sub-section, its module and some identifiers
  virtual G4bool FindTileScinSection(const G4Step*, TileHitData& hitData) const override final;
  /// Calculation of pmtID, edep and scin_Time with aStep (Sergey)
  virtual G4bool MakePmtEdepTime(const G4Step*, TileHitData& hitData, double& deltaTime) const override final;
  /// Create or update hit object in the collection
  virtual G4bool ManageScintHit(TileHitData& hitData, double deltaTime) const override final;
  /// Used by FastCaloSimParamAction
  virtual TileMicroHit GetTileMicroHit(const G4Step*, TileHitData& hitData) const override final;
  ///
  virtual TileGeoG4LookupBuilder* GetLookupBuilder() const override final;
  /// pointer to class with all options
  virtual const TileSDOptions* GetOptions() const override final;

  /// Method used by TileFastCaloSim/TileFCSmStepToTileHitVec
  virtual void pmtEdepFromFCS_StepInfo(TileHitData& hitData, double ene, double yLocal, double halfYLocal, double zLocal, int uShape) const override final;
private:
  void CreateScintHit(int pmt, TileHitData& hitData, double deltaTime) const;
  void UpdateScintHit(int pmt, TileHitData& hitData, double deltaTime) const;

  int getUshapeFromGM() const;

  /** @brief function to calculate Birks correction */
  G4double BirkLaw(const G4Step* aStep) const;

  // Private copy-constructor
  TileGeoG4SDCalc& operator=(const TileGeoG4SDCalc&) = delete;
  TileGeoG4SDCalc(const TileGeoG4SDCalc&) = delete;

  /** @brief function to give PMT responce as a function of distance
      from tile center in mm (along phi direction) */
  static G4double Tile_1D_profileAsym(int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //asymmetric U-shape
  static G4double Tile_1D_profileSym(int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //Single Lookup table is included; average of central 17 bins is normalized to 0.5
  static G4double Tile_1D_profileFunc(int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //Single Lookup table is included; average of all bins is normalized to 0.5
  static G4double Tile_1D_profileRescaled(int row, G4double x, G4double y, int PMT, int nDetector, int nSide);
  static G4double Tile_1D_profileRescaled_zeroBins(int row, G4double x, G4double y, int PMT, int nDetector, int nSide,
                                                   int nZeroBins = 0);

  // the default function which is used in simulation in the case Ushape=1
#define Tile_1D_profile Tile_1D_profileRescaled

  ServiceHandle<StoreGateSvc> m_detStore; // used by TileGeoG4CalibSD.cc
  ServiceHandle<IGeoModelSvc> m_geoModSvc;

  Identifier m_invalid_id{}; /// FIXME just a default-constructed Identifier???

  TileMicroHit m_microHit{};

  TileSDOptions m_options{};

  Gaudi::Property<double> m_birk1{this, "birk1",0.02002 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2), "value updated for G4 10.6.p03"};
  Gaudi::Property<double> m_birk2{this, "birk2",0.0 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2), "value updated for G4 10.6.p03"};

  /** @brief Keep hit time */
  bool m_keepHitTime{};

  /** @brief Structure holding the attenuation lengths */
  std::unique_ptr<TileRow> m_row;

  /** @brief pointer to TileID helper class */
  const TileID* m_tileID{};

  /** @brief pointer to TileTBID helper class */
  const TileTBID* m_tileTBID{};

  /** @brief hits in different tile rows are shifted in time by this amount of ns */
  double m_tileSizeDeltaT{};

  /** @brief time for hits which are above m_timeCut threshold
      it is equal to m_tilesize_deltaT - m_deltaT */
  double m_lateHitTime{};

  /** @brief function to provide correct deltaT bin width for given time */
  inline double deltaT(double time) const {
    unsigned int i = 0;
    double delta = m_options.deltaTHit[i++];
    while (i < m_options.deltaTHit.size()) {
      if (time > m_options.deltaTHit[i++] && time < m_options.deltaTHit[i++]) break;
      delta = m_options.deltaTHit[i++];
    }
    return delta;
  }

};
//class TileGeoG4SDCalc

#endif // not TILEGEOG4SD_TILEGEOG4SDCALC_H
