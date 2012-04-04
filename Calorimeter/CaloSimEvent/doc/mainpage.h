/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage CaloSimEvent Package

The package CaloSimEvent contains data model classes for calorimeter
simulation that are not specific to LAr or Tile. They therefore complement
LArSimEvent and TileSimEvent.

@section CaloCalibrationHit Calibration Hit related classes

This package includes the CaloCalibrationHit class that describes the
calorimeter calibration hit. This class 
includes the energy deposited by type: EM, hadronic, invisible or escaped.
CaloCalibrationHit objects appear as two per cell (active and inactive parts) and in few samplings of
typically eta*phi = 0.1 x 0.1 for 
energy deposited in passive material before and after the calorimeter. They are recorded
in StoreGate in a CaloCalibrationHitContainer class.

*/
