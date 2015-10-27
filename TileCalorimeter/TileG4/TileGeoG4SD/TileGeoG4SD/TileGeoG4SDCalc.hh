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

#ifndef TILEGEOG4SD_TILEGEOG4SDCALC_H
#define TILEGEOG4SD_TILEGEOG4SDCALC_H

// package headers
#include "TileGeoG4SD/TileSDOptions.h"

// athena headers
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"

// Geant4 headers
#include "G4Types.hh"

// STL headers
#include <vector>
#include <memory>

class TileGeoG4LookupBuilder;

class G4Step;

class TileRow;
class TileGeoG4Section;
class TileGeoG4Cell;

class StoreGateSvc;


struct TileMicroHit
{
  Identifier  pmt_up,     pmt_down;
  G4double    e_up,       e_down;
  double      time_up,    time_down;
  //int         period,     tilerow; // prepared for future use
};


class TileGeoG4SDCalc {
public:
  TileGeoG4SDCalc (TileSDOptions opts);
  ~TileGeoG4SDCalc ();

  G4bool       FindTileScinSection (const G4Step*);
  G4bool       MakePmtEdepTime     (const G4Step*);
  TileMicroHit GetTileMicroHit     (const G4Step*);

  G4bool ManageScintHit ();
  void   CreateScintHit (int);
  void   UpdateScintHit (int);

  /** @brief function to give PMT responce as a function of distance
      from tile center in mm (along phi direction) */
  static G4double Tile_1D_profileAsym    (int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //asymmetric U-shape
  static G4double Tile_1D_profileSym     (int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //Single Lookup table is included; average of central 17 bins is normalized to 0.5
  static G4double Tile_1D_profileFunc    (int row, G4double x, G4double y, int PMT, int nDetector, int nSide); //Single Lookup table is included; average of all bins is normalized to 0.5
  static G4double Tile_1D_profileRescaled(int row, G4double x, G4double y, int PMT, int nDetector, int nSide);
  static G4double Tile_1D_profileRescaled_zeroBins(int row, G4double x, G4double y, int PMT, int nDetector, int nSide, int nZeroBins=0);

  // the default function which is used in simulation in the case Ushape=1
#define Tile_1D_profile Tile_1D_profileRescaled

  StoreGateSvc* m_detStore; // used by TileGeoG4CalibSD.cc

  TileGeoG4LookupBuilder* m_lookup;
  TileGeoG4Section*       _section;
  TileGeoG4Cell*          _cell;

  //variables to identify Hit objects
  int nModule;
  int nDetector;
  int nTower;
  int nSample;
  int nSide;

  Identifier m_invalid_id;
  Identifier pmtID_up,        pmtID_down;
  G4double   edep_up,         edep_down;
  double     scin_Time_up,    scin_Time_down;

  const TileSDOptions GetOptions() const { return m_options; }

private:
  // Private copy-constructor
  TileGeoG4SDCalc& operator=( const TileGeoG4SDCalc& ) = delete;
  TileGeoG4SDCalc ( const TileGeoG4SDCalc& ) = delete;

  TileSDOptions m_options;

  int     nrOfPMT;
  int     tilesize;
  int     tileperiod;
  bool    is_negative;
  double  totalTime_up;
  double  totalTime_down;

  /** @brief granularity in time for hits */
  double m_deltaT;

  /** @brief function to calculate Birks correction */
  G4double BirkLaw(const G4Step* aStep) const;

  /** @brief Structure holding the attenuation lengths */
  std::unique_ptr<TileRow> m_row;

  /** @brief pointer to TileID helper class */
  const  TileID* m_tileID;

  /** @brief pointer to TileTBID helper class */
  const  TileTBID* m_tileTBID;

  /** @brief hits in different tile rows are shifted in time by this amount of ns */
  double m_tilesize_deltaT;

  /** @brief time for hits which are above m_timeCut threshold
      it is equal to m_tilesize_deltaT - m_deltaT */
  double m_lateHitTime;

  /** @brief function to provide correct deltaT bin width for given time */
  inline double deltaT(double time) const {
    unsigned int i=0;
    double delta = m_options.deltaTHit[i++];
    while (i<m_options.deltaTHit.size()) {
      if (time>m_options.deltaTHit[i++] && time<m_options.deltaTHit[i++]) break;
      delta = m_options.deltaTHit[i++];
    }
    return delta;
  }

};  //class TileGeoG4SDCalc

#endif // not TILEGEOG4SD_TILEGEOG4SDCALC_H
