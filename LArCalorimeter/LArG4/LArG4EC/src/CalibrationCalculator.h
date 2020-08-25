/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::CalibrationCalculator

// based on:
////     LArG4::HEC::CalibrationCalculator
////     Prepared 13-Jan-2004 Bill Seligman

// This class calculates the values needed for calibration hits in the
// simulation.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

#ifndef LArG4_EC_CalibrationCalculator_H
#define LArG4_EC_CalibrationCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;
class ILArCalculatorSvc;

// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECCalibrationCalculator).  This class is contained in
// the namespace LArG4::EC.

namespace LArG4 {

  namespace EC {

    // Forward declaration
    class Geometry;

    class CalibrationCalculator : public LArCalibCalculatorSvcImp {
    public:

      //CalibrationCalculator(LArWheelCalculator::LArWheelCalculator_t, int);
      CalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      virtual ~CalibrationCalculator();
      // Update handler
      void WheelTypeHandler(Gaudi::Details::PropertyBase&);
      StatusCode initialize() override final;

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
                  const eCalculatorProcessing p = kEnergyAndID) const override final;

    private:

      int m_zside;
      UnsignedShortProperty m_wcalc_tProp;
      LArG4::LArWheelCalculator_t m_wcalc_t;

      // Geometry calculator
      ServiceHandle<ILArCalculatorSvc> m_geometryCalculator;

      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

    };

  } // namespace EC

} // namespace LArG4

#endif // LArG4_EC_CalibrationCalculator_H
