/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4MiniFCAL/MiniFCALCalibrationCalculator.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include <algorithm>

#undef DEBUG_HITS

namespace LArG4 {

  namespace MiniFCAL {

    MiniFCALCalibrationCalculator::MiniFCALCalibrationCalculator(const eMiniFCALAssignIdentifierType type) {

      StoreGateSvc* detStore;
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      StatusCode status = svcLocator->service("DetectorStore", detStore);
      MsgStream log(Athena::getMessageSvc(),"MiniFCALCalculator" );
      if(status != StatusCode::SUCCESS ) {
         log << MSG::ERROR << "No DetStore available !!" << endmsg;
      }

      log << MSG::INFO << "Use the MiniFCALCalibrationCalculator for the MiniFCAL" << endmsg;

      // Make sure there are no uninitialized variables.
      m_identifier = LArG4Identifier();
      m_energies.clear();

      // Initialize the geometry and energy calculators.
      m_geometryCalculator = MiniFCALAssignIdentifier::GetInstance();
      //m_energyCalculator   = new CaloG4::SimulationEnergies();
      m_geometryType       = type;
    }


    MiniFCALCalibrationCalculator::~MiniFCALCalibrationCalculator() {
      // Clean up pointers.
      //delete m_energyCalculator;
    }


    G4bool MiniFCALCalibrationCalculator::Process( const G4Step* a_step,
					   const eCalculatorProcessing a_process ) {
      
      m_energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
	{
	  m_energyCalculator.Energies( a_step, m_energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) m_energies.push_back( 0. );
      
      

      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
	{
	  // Calculate the identifier.
	  
	  m_identifier = m_geometryCalculator->CalculateIdentifier(a_step, m_geometryType );	  
          // Check for bad result.
          if ( m_identifier == LArG4Identifier() ) return false;

          return true;
        }
      return true;
    }

  } // namespace MiniFCAL

} // namespace LArG4
