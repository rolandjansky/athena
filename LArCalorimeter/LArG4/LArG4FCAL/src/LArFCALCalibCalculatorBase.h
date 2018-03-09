/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::FCAL::LArFCALCalibCalculator
// Prepared Oct./2004 Mohsen Khakzad

// This class calculates the values needed for calibration hits in the
// simulation.

// A "calculator" is used in much the same way as a hand-held
// calculator might be.  The user supplies a value and hits 'Enter'
// Forward declaractions:
// (i.e., invokes the Process() method).  Then they read off whatever
// values are of interest.

#ifndef LArFCALCalibCalculatorBase_H
#define LArFCALCalibCalculatorBase_H

#include "LArG4Code/LArCalibCalculatorSvcImp.h"
#include "LArG4Code/LArG4Identifier.h"

//#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"
#include <vector>
// Forward declaration for namespace CaloG4.
class G4Step;
class FCAL_ChannelMap;

namespace LArG4 {

  namespace FCAL {

    class LArFCALCalibCalculatorBase : public LArCalibCalculatorSvcImp {
    public:
      LArFCALCalibCalculatorBase(const std::string& name, ISvcLocator *pSvcLocator);
      StatusCode initialize();
      StatusCode finalize() {return StatusCode::SUCCESS;}
      virtual ~LArFCALCalibCalculatorBase(){};


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
                              const eCalculatorProcessing process = kEnergyAndID) const override final;

      /////////////////////////////////////////////

      virtual G4double GetdeltaX(){return m_deltaX;} //FIXME public but not part of interface class
      virtual G4double GetdeltaY(){return m_deltaY;} //FIXME public but not part of interface class

    protected:

      G4double m_deltaX;
      G4double m_deltaY;

      G4int m_FCalSampling;

    private:

      // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;

      G4float m_zShift;

      FCAL_ChannelMap   *m_ChannelMap;

    };
  }  // namespace FCAL

}  // namespace LArG4

#endif    // LArG4_HEC_CalibrationCalculator_H
