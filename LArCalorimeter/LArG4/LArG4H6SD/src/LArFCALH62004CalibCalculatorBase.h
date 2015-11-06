/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArFCALH62004CalibCalculatorBase_H
#define LArFCALH62004CalibCalculatorBase_H

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArG4Identifier.h"

#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "LArG4Code/LArVCalculator.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "globals.hh"
#include <vector>
// Forward declaration for namespace CaloG4.
class G4Step;

class LArFCALH62004CalibCalculatorBase : public LArG4::VCalibrationCalculator {

    public:
      LArFCALH62004CalibCalculatorBase();
      virtual ~LArFCALH62004CalibCalculatorBase();

      virtual G4bool Process (const G4Step* step,
                              const LArG4::VCalibrationCalculator::eCalculatorProcessing p = LArG4::VCalibrationCalculator::kEnergyAndID);

        // The cell identifier determined by the Process method.
      virtual const LArG4Identifier& identifier() const { return m_identifier; }


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

      G4float m_OOTcut;

        // The results of the Process calculation:
      G4double m_time;
      G4bool m_isInTime;

      FCAL_ChannelMap   *m_ChannelMap;

      const IRDBRecordset* m_fcalMod;
      float m_Zshift;

};


#endif    // LArG4_HEC_CalibrationCalculator_H
