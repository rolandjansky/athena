/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include <algorithm>

#undef DEBUG_HITS

namespace LArG4 {

  namespace HEC {

    LArHECCalibrationWheelCalculator::LArHECCalibrationWheelCalculator(const eHECGeometryType type) {

      StoreGateSvc* detStore;
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      StatusCode status = svcLocator->service("DetectorStore", detStore);
      MsgStream log(Athena::getMessageSvc(),"LArHECWheelCalculator" );
      if(status != StatusCode::SUCCESS ) {
         log << MSG::ERROR << "No DetStore available !!" << endmsg;
      }

      log << MSG::INFO << "Use the LArHECCalibrationWheelCalculator for the HEC" << endmsg;

      // Initialize the geometry calculator.
      m_geometryCalculator = HECGeometry::GetInstance();
      m_geometryType       = type;
    }


    LArHECCalibrationWheelCalculator::~LArHECCalibrationWheelCalculator() {
    }


    G4bool LArHECCalibrationWheelCalculator::Process( const G4Step* a_step,
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

  } // namespace HEC

} // namespace LArG4
