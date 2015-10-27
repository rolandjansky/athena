/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESDOPTIONS_h
#define TILESDOPTIONS_h

/* This is a very simple struct for holding sensitive detector
configuration options; formerly this was in
TileCalorimeter/TileSimUtils/TileSimUtils/TileG4SimOptions.h
But now that there are real tools, that setup is not necessary
*/

#include <string>
#include <vector>

struct TileSDOptions {
  /** Setup defaults */
  TileSDOptions()
   : timeCut ( 350.5 )
   , tileTB ( false )
   , plateToCell ( true )
   , Ushape ( -1 )
   , doBirk ( true )
   , doTileRow ( false )
   , doTOFCorrection ( true )
   , doCalibHitParticleID ( false )
   , verboseLevel(0)
   , rDBAccessSvcName ( "RDBAccessSvc" )
   , geoModelSvcName ( "GeoModelSvc" )
  { deltaTHit = { 0.5 , -75.25 , 75.25 , 5. }; }

  /** A time granularity for G4 hits in TileHit
   it can be just one number, e.g. 5.0 which means that the same
   granularity is used everywhere or it can be a vector which contains
   3*N+1 elements, e.g. 0.1,-5,5, 0.5,-75,75, 5.0 which means that 
   for [-5,5] ns interval granularity 0.1ns will be used
   for [-75,75] ns interval granularity 0.5ns will be used
   and 5ns granularity will be used for all other hits  */
  std::vector<double> deltaTHit;
  /** Time cut for hits, all hits go to one single time bin if time is above this cut */
  double timeCut;
  /** Flag to indicate TileCal TestBeam configuration */
  bool tileTB;
  /** Special flag for Calibration Hits. If true then Tile
      Plates are the parts of the adjacent Tile cells.
      If false then they are Dead Materials */
  bool plateToCell;
  /** Flag to enable simulation of light attenuation in tiles */
  int Ushape;
  /** Flag to enable Birk's law */
  bool doBirk;
  /** Flag to enable energy per tile row in TileHit */
  bool doTileRow;
  /** Flag to apply TOF correction (subtract Time Of Flight from ATLAS center) */
  bool   doTOFCorrection;
  /** Flag to produce calibration hits signed with primary particle ID */
  bool   doCalibHitParticleID;
  /** Helper to keep the same verbosity everywhere */
  int verboseLevel;
  //** Name of the RDBAccessSvc instance to use */
  std::string rDBAccessSvcName;
  //** Name of the GeoModelSvc instance to use */
  std::string geoModelSvcName;
};

#endif
