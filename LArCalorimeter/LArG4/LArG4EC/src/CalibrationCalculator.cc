/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::CalibrationCalculator

#undef DEBUG_HITS

#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4EC/EnergyCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"

namespace LArG4 {

  namespace EC {

    CalibrationCalculator::CalibrationCalculator(LArWheelCalculator::LArWheelCalculator_t t, int zside)
    {
      // Make sure there are no uninitialized variables.
      m_geometryCalculator = new EnergyCalculator(t, EnergyCalculator::EMEC_ECOR_OFF, zside);
    }


    CalibrationCalculator::~CalibrationCalculator()
    {
      // Clean up pointers.
      delete m_geometryCalculator;
    }


    G4bool CalibrationCalculator::Process( const G4Step* a_step,
					   const eCalculatorProcessing a_process )
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      m_energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
	{
#ifdef DEBUG_HITS
	  std::cout << "LArG4::EC::CalibrationCalculator::Process"
		    << " calling SimulationEnergies"
		    << " at m_energyCalculator="
		    << m_energyCalculator
		    << std::endl;
#endif
	  m_energyCalculator.Energies( a_step, m_energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );


      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
	{
	  // Calculate the identifier.
	  (void)m_geometryCalculator->Process( a_step );
	   m_identifier =  m_geometryCalculator->identifier();
	}
      else
	m_identifier = LArG4Identifier();


#ifdef DEBUG_HITS
      G4double energy = accumulate(m_energies.begin(),m_energies.end(),0.);
      std::cout << "LArG4::EC::CalibrationCalculator::Process"
		<< " ID=" << std::string(m_identifier)
		<< " energy=" << energy
		<< " energies=(" << m_energies[0]
		<< "," << m_energies[1]
		<< "," << m_energies[2]
		<< "," << m_energies[3] << ")"
		<< std::endl;
#endif

      // Check for bad result.
      if ( m_identifier == LArG4Identifier() ) {
        std::cout << "return invalid from EmecCalibrationCalculator" << std::endl;
	return false;
      }

      return true;
    }

  } // namespace EC

} // namespace LArG4
