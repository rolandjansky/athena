/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6COLDTCMod0CalibCalculator.h"

#include "G4Step.hh"

LArG4H6COLDTCMod0CalibCalculator::LArG4H6COLDTCMod0CalibCalculator() {

   // Initialize the geometry calculator.
   m_Calculator = LArG4H6COLDTCMod0Calculator::GetInstance();

}
    
LArG4H6COLDTCMod0CalibCalculator::~LArG4H6COLDTCMod0CalibCalculator(){}   


G4bool LArG4H6COLDTCMod0CalibCalculator::Process( const G4Step* a_step, const eCalculatorProcessing a_process )
{

      m_energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
	{
	  m_energyCalculator.Energies( a_step, m_energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );


      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
	{
	   m_identifier = LArG4Identifier();
	  // Calculate the identifier.
           if(m_Calculator->Process(a_step)) m_identifier = m_Calculator->identifier(0);
	      else m_identifier = LArG4Identifier();
	}
      else
	m_identifier = LArG4Identifier();

  
      // Check for bad result.
      if ( m_identifier == LArG4Identifier() ) {
         std::cout<<"LArG4H6COLDTCMod0CalibCalculator::Process return false !!!"<<std::endl;	 
	 return false;
      }

      return true;
}

