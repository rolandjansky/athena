/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::PresamplerCalibrationCalculator
// Prepared 17-Aug-2004 Bill Seligman

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS

#include "PresamplerCalibrationCalculator.h"
#include "LArG4EC/IECPresamplerGeometry.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"

namespace LArG4 {

  namespace EC {

    PresamplerCalibrationCalculator::PresamplerCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_geometryCalculator("EMECPresamplerGeometry", name) // LArG4::EC::PresamplerGeometry
    {
      declareProperty("GeometryCalculator", m_geometryCalculator);
    }

    StatusCode PresamplerCalibrationCalculator::initialize()
    {
      // Initialize the geometry calculator.
      ATH_CHECK(m_geometryCalculator.retrieve());
      return StatusCode::SUCCESS;
    }

    PresamplerCalibrationCalculator::~PresamplerCalibrationCalculator()
    {
    }


    G4bool PresamplerCalibrationCalculator::Process (const G4Step* a_step,
                                                     LArG4Identifier & _identifier,
                                                     std::vector<G4double> & _energies,
                                                     const eCalculatorProcessing a_process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      _energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
        {
#ifdef DEBUG_HITS
          ATH_MSG_DEBUG("Process(): calling SimulationEnergies");
#endif
          m_energyCalculator.Energies( a_step, _energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) _energies.push_back( 0. );


      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
        {
          // Calculate the identifier.
          _identifier = m_geometryCalculator->CalculateIdentifier( a_step );
        }
      else
        _identifier = LArG4Identifier();


#ifdef DEBUG_HITS
      G4double energy = accumulate(_energies.begin(),_energies.end(),0.);
      ATH_MSG_DEBUG("Process(): "
                    << " ID=" << std::string(_identifier)
                    << " energy=" << energy
                    << " energies=(" << _energies[0]
                    << "," << _energies[1]
                    << "," << _energies[2]
                    << "," << _energies[3] << ")");
#endif

      // Check for bad result.
      if ( _identifier == LArG4Identifier() )
        return false;

      return true;
    }

  } // namespace EC

} // namespace LArG4
