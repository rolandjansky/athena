/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EMECSupport::CalibrationCalculator
// Prepared 9-Sep-2004 Mikhail Leltchouk

// This class calculates the values needed for calibration hits in the
// simulation. 

// This particular class is meant to be be used for particular volumes
// in the endcap.  Some volumes have identifiers that change
// significantly (mixed samplings, types, subdets) within that single
// volume.  This calculator handles those special cases.

#ifndef LArG4_EMECSupport_CalibrationCalculator_H
#define LArG4_EMECSupport_CalibrationCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArGeoCode/VDetectorParameters.h"
#include "CryostatCalibrationLArCalculator.h"
#include "CaloG4Sim/SimulationEnergies.h"
#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

// Note the use of nested namespaces (mainly to prevent long names).
// This class is contained in the namespace LArG4::EMECSupport.

namespace LArG4 {

  //  namespace EMECSupport {

    class EMECSupportCalibrationCalculator : public LArCalibCalculatorSvcImp {
    public:

      EMECSupportCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      StatusCode initialize() override final;
      virtual ~EMECSupportCalibrationCalculator();

      // The Process method returns a boolean value.  If it's true, the
      // hit can be used by Geant4; if it's false, there's something wrong
      // with the energy deposit and it should be ignored.

      // For calibration work, most of the time we want the calculator
      // to determine both the energy and the identifier.  However,
      // sometimes we want it calculate only the identifier (for
      // escaped energy), or only the energy (no known application
      // yet, but you can never tell).  Use the enum (defined in
      // VCalibrationCalculator.h) to control any special processing.

      virtual G4bool Process (const G4Step* step, LArG4Identifier & _identifier,
                  std::vector<G4double> & _energies,
                  const eCalculatorProcessing p = kEnergyAndID) const override final;

    private:

      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

      class Parameters;
      const Parameters *m_par;

      // Access to parameters.
      //LArGeo::VDetectorParameters* m_parameters;

      // Define a "backup" calculator.
      ServiceHandle<ILArCalibCalculatorSvc> m_backupCalculator;

      EMECSupportCalibrationCalculator (const EMECSupportCalibrationCalculator&);
      EMECSupportCalibrationCalculator& operator= (const EMECSupportCalibrationCalculator&);
    };

    //  } // namespace EMECSupport

} // namespace LArG4

#endif // LArG4_EMECSupport_CalibrationCalculator_H
