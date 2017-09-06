/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileGeoG4SD_page TileGeoG4SD Package

This package is used in Geant4 simulation and contains classes to accumulate energy
deposited in TileCal active material (scintillators) in TileHit objects and store
all TileHits in TileHitVector in StoreGate at the end of the event. Birk's saturation law in organic scintillator is applied.

Main class of this package, which is doing all the job  has the same name as 
package itself - TileGeoG4SD

TileGeoG4Lookup.hh contains definition of various classes - tables to convert
from internal Geant4 numbers to PMT Identifier used in TileHit

TileGeoG4LookupBuilder initializes all lookup tables during initialization and also
keeps tables with pointers to all TileHit objects created during event and
resets those tables at the end of every event.

@author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>



*/
