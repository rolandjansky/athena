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

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"

#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "CaloG4Sim/SimulationEnergies.h"

#include "LArG4Code/LArVCalculator.h"
#include "globals.hh"
#include <vector>
// Forward declaration for namespace CaloG4.
class G4Step;

namespace LArG4 {

  namespace FCAL {
    
    class LArFCALCalibCalculatorBase : public VCalibrationCalculator {
    public:
      LArFCALCalibCalculatorBase();
      virtual ~LArFCALCalibCalculatorBase();
      

	// The Process method returns a boolean value.  If it's true, the
	// hit can be used by Geant4; if it's false, there's something wrong
	// with the energy deposit and it should be ignored.

	// For calibration work, most of the time we want the calculator
	// to determine both the energy and the identifier.  However,
	// sometimes we want it calculate only the identifier (for
	// escaped energy), or only the energy (no known application
	// yet, but you can never tell).  Use the enum (defined in
	// VCalibrationCalculator.h) to control any special processing.

      virtual G4bool Process (const G4Step* step,
			      const eCalculatorProcessing p = kEnergyAndID);
	
	// The cell identifier determined by the Process method.
      virtual LArG4Identifier identifier() const { return m_identifier; }


      virtual G4float OOTcut() const { return m_OOTcut; }

      virtual G4double time() const { return m_time; }
      virtual const std::vector<G4double>& energies() const { return m_energies; }
      virtual G4bool isInTime() const { return     m_isInTime; }
      virtual G4bool isOutOfTime() const { return ( ! m_isInTime ); }
      /////////////////////////////////////////////

      virtual G4double GetdeltaX(){return m_deltaX;}
      virtual G4double GetdeltaY(){return m_deltaY;}
      
    protected:

      G4double m_deltaX;
      G4double m_deltaY;

      G4int m_FCalSampling;

    private:

  // The values calculated by Process()
      LArG4Identifier m_identifier;
      std::vector<G4double> m_energies;
      
      
  // Energy calculator
      CaloG4::SimulationEnergies m_energyCalculator;
      
        ////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$CalculatorBase
	// Store the endcap shift and out-of-time cut from the description:
      G4float m_OOTcut;
      G4float m_zShift;
      
	// The results of the Process calculation:
      G4double m_time;
      G4bool m_isInTime;

      FCAL_ChannelMap   *m_ChannelMap;
      
    };
  }  // namespace FCAL
  
}  // namespace LArG4

#endif    // LArG4_HEC_CalibrationCalculator_H
