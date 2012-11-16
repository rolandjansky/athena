/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HEC/LocalCalibrationCalculator.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include <algorithm>

#undef DEBUG_HITS

namespace LArG4 {

  namespace HEC {

    LocalCalibrationCalculator::LocalCalibrationCalculator(const eLocalGeometryType type) {
#ifdef DEBUG_HITS
	  std::cout << "LArG4::HEC::LocalCalibrationCalculator constructed" << std::endl;
#endif
      // Initialize the geometry calculator.
      m_geometryCalculator = LocalGeometry::GetInstance();
      m_geometryType = type;
    }


    LocalCalibrationCalculator::~LocalCalibrationCalculator() {
    }


    G4bool LocalCalibrationCalculator::Process( const G4Step* a_step,
					   const eCalculatorProcessing a_process ) {

#ifdef DEBUG_HITS
	  std::cout << "LArG4::HEC::LocalCalibrationCalculator::Process" << std::endl;
#endif
      m_energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
	{
#ifdef DEBUG_HITS
          std::cout << " calling SimulationEnergies" << std::endl;
#endif
	  m_energyCalculator.Energies( a_step, m_energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );


      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
	{
	   // Calculate the identifier.
           G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
           G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
           // Volume name 
           G4String hitVolume = theTouchable->GetVolume(0)->GetName();
#ifdef DEBUG_HITS
          std::cout<<"LArHECLocalCalibrationCalculator::Process Volume: "<<hitVolume<<std::endl;
#endif
          if(hitVolume.contains("::") ) {
             int last = hitVolume.last(':');
             hitVolume.remove(0,last+1);
          } 
	  if(m_geometryType != kLocDead) {
             if(hitVolume=="Slice" ) {
                 m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, kLocInactive, 0, 4.*CLHEP::mm);
	     } else if(hitVolume=="Absorber" ) {
                 m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, kLocInactive, 0, 2.*CLHEP::mm, 1.02*CLHEP::mm);
             } else if(hitVolume=="Electrode") { 
                 m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, kLocInactive, 1, 4.*CLHEP::mm);
             } else if(hitVolume=="Copper") {
                 m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, kLocInactive, 2, 4.*CLHEP::mm);
             } else if(hitVolume=="TieRod"){ // We should call another functions for TieRods 
                 m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, kLocActive, -1);
	     } else m_identifier = LArG4Identifier();
	  } else m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, m_geometryType, 0, 4.*CLHEP::mm);

#ifdef DEBUG_HITS
	  G4double energy = accumulate(m_energies.begin(),m_energies.end(),0.);
	  std::cout << "LArG4::HEC::LocalCalibrationCalculator::Process"
		    << " ID=" << std::string(m_identifier)
		    << " energy=" << energy
		    << " energies=(" << m_energies[0]
		    << "," << m_energies[1]
		    << "," << m_energies[2]
		    << "," << m_energies[3] << ")"
		    << std::endl;
#endif

          // Check for bad result.
          if ( m_identifier == LArG4Identifier() ) return false;

          return true;
        }
      return true;
    }

  } // namespace HEC

} // namespace LArG4
