/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileGeoG4Calib_page TileGeoG4Calib Package

This package is used in Geant4 simulations with calibration hits. Hit energy is collected in both sensitive (scintillators) and non-sensitive (absorber, dead material) material in the package. Escaped energy is also calculated here. Energy in the event is divided in electromagnetic, non-electromagnetic, invisible and escaped components. CaloCalibrationHitContainer object is used to store the calibration hits in StoreGate, also TileHitVector object with 'normal' hits is stored. Birk's saturation law in organic scintillator is applied.

TileGeoG4CalibSD - Main class of this package, collects hit energy, divides energy in different components, store CaloCalibrationHits and TileHits in StoreGate

TileGeoG4DMLookup.h  - contains calibration look-up tables

TileGeoG4DMLookupBuilder - class building look-up tables for TileGeoG4CalibSD, it initializes all calibration and also ordinary (for 'normal' hits) lookup tables

TileCalibDddbManager - provides the access to the Tile Calibration DB, it is called by the TileGeoG4DMLookupBuilder class

TileEscapedEnergyProcessing  -  manages processing of those particles that were created in Tile region and escaped from the detector, calculates the escaped energy

@author Gia Khoriauli <gia.khoriauli@cern.ch>



*/
