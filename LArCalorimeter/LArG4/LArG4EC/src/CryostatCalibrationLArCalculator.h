/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EndcapCryostat::CalibrationLArCalculator
// Prepared 19-Aug-2004 Bill Seligman

// This class calculates the values needed for calibration hits in the
// simulation.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

// This particular class is meant to be be used for general LAr
// regions within the endcap cryostat.  The liquid-argon volumes
// typically "fill in the cracks" between all other volumes; this
// calculator determines the identifiers for such "cracks".

#ifndef LArG4_EndcapCryostat_CalibrationLArCalculator_H
#define LArG4_EndcapCryostat_CalibrationLArCalculator_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"

#include <vector>

// Forward declaractions:
class G4Step;

// Note the use of nested namespaces (mainly to prevent long names
// like LArG4HECCalibrationCalculator).  This class is contained in
// the namespace LArG4::EndcapCryostat.

namespace LArG4 {

  namespace EndcapCryostat {

    class CalibrationLArCalculator : public LArCalibCalculatorSvcImp {
    public:

      CalibrationLArCalculator(const std::string& name, ISvcLocator *pSvcLocator);
      StatusCode initialize() override final;
      virtual ~CalibrationLArCalculator();

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

      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

      // Access to parameters.
      const LArVG4DetectorParameters* m_parameters{nullptr};

      // For the default calculator (hopefully temporary).
      ServiceHandle<ILArCalibCalculatorSvc> m_defaultCalculator;

      double m_rhoOutOfEmecHec{0.};      // used as const after init
      double m_zInFrontOfPresampler{0.}; // used as const after init
      double m_zEMECRefPoint{0.};        // used as const after init
      double m_zInFrontOfSpanishFan{0.}; // used as const after init
      double m_zInFrontOfHEC{0.};        // used as const after init
      double m_zBehindTile{0.};          // used as const after init
      double m_endZHEC1Wheel{0.};        // used as const after init
      double m_startZHEC2Wheel{0.};      // used as const after init
      inline double rhoOutOfEmecHec() const { return m_rhoOutOfEmecHec; };
      inline double zInFrontOfPresampler() const { return m_zInFrontOfPresampler; };
      inline double zEMECRefPoint() const { return m_zEMECRefPoint; };
      inline double zInFrontOfSpanishFan() const { return m_zInFrontOfSpanishFan; };
      inline double zInFrontOfHEC() const { return m_zInFrontOfHEC; };
      inline double zBehindTile() const { return m_zBehindTile; };
      inline double endZHEC1Wheel() const { return m_endZHEC1Wheel; };
      inline double startZHEC2Wheel() const { return m_startZHEC2Wheel; };
    };

  } // namespace EndcapCryostat

} // namespace LArG4

#endif // LArG4_EndcapCryostat_CalibrationLArCalculator_H
