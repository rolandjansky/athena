/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////
//
// TileG4SimOptions class
//
//      This class provides methods to pass some options 
//	to G4 simulation of TileCal via Python-script  
//
// author: <gia@mail.cern.ch>
// February, 2006
//
//////////////////////////////////////////////////////////

#ifndef TILESIMUTILS_TILEG4SIMOPTIONS_H
#define TILESIMUTILS_TILEG4SIMOPTIONS_H

#include <string>
#include <vector>


/** 
* @class TileG4SimOptions
* @brief Special options used in G4 simulation
* @author Gia Khoriauli
*
*      This class provides methods to pass some options 
*       to G4 simulation of TileCal via Python-script  
*/

class TileG4SimOptions {

public:
  //constructor
  TileG4SimOptions()
    : m_deltaTHit(4)
    , m_timeCut(350.5)// need such huge value for pileup, otherwise 150.5 is enough
    , m_tileTB(false)
    , m_plateToCell(true)
    , m_Ushape(-1) // -1 means not set - use value from GeoModel DB
    , m_doBirk(true)
    , m_doTileRow(false)
    , m_doTOFCorrection(true)
    , m_doCalibHitParticleID(false)
    {
      m_deltaTHit[0] = 0.5;  // 0.5 ns inside window
      m_deltaTHit[1] = -75.25;
      m_deltaTHit[2] = 75.25;
      m_deltaTHit[3] = 5.;  // 5 ns outside
    }

  //To store an object of this class in StoreGate
  void saveMe() const;
  //To debug all options 
  void printMe() const;
  operator std::string() const;
  std::string whoami() const { return "TileG4SimOptions"; }

  //Set Methods
  void SetDeltaTHit           (double deltaTHit)    { m_deltaTHit.resize(1); m_deltaTHit[0]=deltaTHit; }
  void SetDeltaTHit           (const std::vector<double> & deltaTHit)    { m_deltaTHit=deltaTHit; }
  void SetTimeCut             (double timeCut)      { m_timeCut=timeCut; }
  void SetTileTBFlag          (bool   tileTB)       { m_tileTB=tileTB; }
  void SetPlateToCellFlag     (bool   plateToCell)  { m_plateToCell=plateToCell; }
  void SetUshape              (int    Ushape)       { m_Ushape=Ushape; }
  void SetDoBirk              (bool   doBirk)       { m_doBirk=doBirk; }
  void SetDoTileRow           (bool   doTileRow)    { m_doTileRow=doTileRow; }
  void SetDoTOFCorrection     (bool   doTOFCorr)    { m_doTOFCorrection=doTOFCorr; }
  void SetDoCalibHitParticleID(bool   doParticleID) { m_doCalibHitParticleID=doParticleID; }

  //Get Methods
  const std::vector< double> & GetDeltaTHitVector () const { return m_deltaTHit; }
  double GetDeltaTHit           () const { return m_deltaTHit[m_deltaTHit.size()-1];   }
  double GetTimeCut             () const { return m_timeCut;     }
  bool   GetTileTBFlag          () const { return m_tileTB;      }
  bool   GetPlateToCellFlag     () const { return m_plateToCell; }
  int    GetUshape              () const { return m_Ushape;      }
  bool   GetDoBirk              () const { return m_doBirk;      }
  bool   GetDoTileRow           () const { return m_doTileRow;   }
  bool   GetDoTOFCorrection     () const { return m_doTOFCorrection;}
  bool   GetDoCalibHitParticleID() const { return m_doCalibHitParticleID;}

private:
  /** A time granularity for G4 hits in TileHit
   it can be just one number, e.g. 5.0 which means that the same
   granularity is used everywhere or it can be a vector which contains
   3*N+1 elements, e.g. 0.1,-5,5, 0.5,-75,75, 5.0 which means that 
   for [-5,5] ns interval granularity 0.1ns will be used
   for [-75,75] ns interval granularity 0.5ns will be used
   and 5ns granularity will be used for all other hits  */
    std::vector<double> m_deltaTHit;
  /** Time cut for hits, all hits go to one single time bin if time is above this cut */
  double m_timeCut;
  /** Flag to indicate TileCal TestBeam configuration */
  bool m_tileTB;
  /** Special flag for Calibration Hits. If true then Tile
      Plates are the parts of the adjacent Tile cells.
      If false then they are Dead Materials */
  bool m_plateToCell;
  /** Flag to enable simulation of light attenuation in tiles */
  int m_Ushape;
  /** Flag to enable Birk's law */
  bool m_doBirk;
  /** Flag to enable energy per tile row in TileHit */
  bool m_doTileRow;
  /** Flag to apply TOF correction (subtract Time Of Flight from ATLAS center) */
  bool   m_doTOFCorrection;
  /** Flag to produce calibration hits signed with primary particle ID */
  bool   m_doCalibHitParticleID;
};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(TileG4SimOptions, 2912, 1)
#endif

#endif // TILESIMUTILS_TILEG4SIMOPTIONS_H

