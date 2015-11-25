/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     CalibrationCalculator.cxx
PACKAGE:  offline/LArCalorimeter/LArG4/LArG4Barrel

AUTHORS:  G. Unal, L. Carminati (on a template from Bill Selingman)
CREATED:  September, 2004

PURPOSE:  This class calculates the values needed for calibration hits 
          in the barrel LAr calorimeter. This calculator is called
          in calibration runs (see LArBarrelSDConsultant) for calibration 
          hits in the accordion (no presampler).

UPDATES:  

********************************************************************/

//#define DEBUG_HITS

#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"

#include "LArG4Barrel/LArBarrelGeometry.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"
#include <algorithm>

namespace LArG4 {

namespace Barrel {

    CalibrationCalculator::CalibrationCalculator() 
       : m_detectorName("LArMgr")
    {
      // Initialize the geometry calculator.
      m_geometryCalculator = Geometry::GetInstance();
    }

    CalibrationCalculator::~CalibrationCalculator()
    {
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
	  std::cout << "LArG4::Barrel::CalibrationCalculator::Process" 
		    << " calling SimulationEnergies" << std::endl;
#endif
	  m_energyCalculator.Energies( a_step, m_energies );

	  // First, get the energy.
	  //	  m_energy = a_step->GetTotalEnergyDeposit();	   
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );
      
      
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
	{
	  // Calculate the identifier.
	  m_identifier = m_geometryCalculator->CalculateIdentifier( a_step, m_detectorName );
	}
      else
	m_identifier = LArG4Identifier();
      
      
#ifdef DEBUG_HITS
      G4double energy = accumulate(m_energies.begin(),m_energies.end(),0.);
      std::cout << "LArG4::Barrel::CalibrationCalculator::Process"
      		<< " ID=" << std::string(m_identifier)
		<< " energy=" << energy
      		<< " energies=(" << m_energies[0]
      		<< "," << m_energies[1]
      		<< "," << m_energies[2]
      		<< "," << m_energies[3] << ")"
      		<< std::endl;
#endif
      
      // Check for bad result.
      if ( m_identifier == LArG4Identifier() )
	return false;
      
      return true;
    }
    
} // namespace Barrel
  
} // namespace LAr
