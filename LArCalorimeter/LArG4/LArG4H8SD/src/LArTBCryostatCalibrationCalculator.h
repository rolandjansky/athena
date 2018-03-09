/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArTBCryostatCalibrationCalculator
//  22 sep 2004 G.Unal

// This class calculates the values needed for calibration hits in the
// simulation.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

// This particular class is meant to be a "generic" calculator for
// dead materials.  It's assumed that for most dead volumes, the
// detector_system/subdet/type/sampling/region of the identifier will
// remain constant (except that subdet will be +/- depending on z).
// The only portions that will change are the eta and phi identifiers.

#ifndef LArG4H8SD_Cryostat_CalibrationCalculator_H
#define LArG4H8SD_Cryostat_CalibrationCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "CaloG4Sim/SimulationEnergies.h"

// #include "LArG4Barrel/LArBarrelGeometry.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;


// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECCalibrationCalculator).  This class is contained in
// the namespace LArG4::BarrelCryostat.

class LArTBCryostatCalibrationCalculator : public LArCalibCalculatorSvcImp
{
public:

  LArTBCryostatCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator);
  // virtual StatusCode initialize() override final;
  virtual ~LArTBCryostatCalibrationCalculator();

  // The Process method returns a boolean value.  If it's true, the
  // hit can be used by Geant4; if it's false, there's something wrong
  // with the energy deposit and it should be ignored.

  // For calibration work, most of the time we want the calculator
  // to determine both the energy and the identifier.  However,
  // sometimes we want it calculate only the identifier (for
  // escaped energy), or only the energy (no known application
  // yet, but you can never tell).  Use the enum (defined in
  // VCalibrationCalculator.h) to control any special processing.

  virtual G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                          std::vector<G4double> & energies,
                          const LArG4::eCalculatorProcessing p = LArG4::kEnergyAndID) const override final;

private:

  // Energy calculator
  CaloG4::SimulationEnergies m_energyCalculator;

  // LArG4::Barrel::Geometry* m_geometry;

  G4double m_radiusPS;
  G4double m_rminAcc;
  G4double m_rmaxAcc;

};

#endif // LArG4_BarrelCryostat_CalibrationCalculator_H
