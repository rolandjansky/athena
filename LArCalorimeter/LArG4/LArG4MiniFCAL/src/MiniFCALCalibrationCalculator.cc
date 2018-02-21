/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MiniFCALCalibrationCalculator.h"

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

    MiniFCALCalibrationCalculator::MiniFCALCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator) : LArCalibCalculatorSvcImp(name, pSvcLocator),
    m_geometryType(kActive) {
       declareProperty("GeometryType",m_strgeometryType="ACTIVE");
    }

    StatusCode MiniFCALCalibrationCalculator::initialize() {

      if(m_strgeometryType.find("DEAD") != std::string::npos){
            m_geometryType=kDead;
     } else if (m_strgeometryType.find("INACTIVE") != std::string::npos){
            m_geometryType=kInactive;
     } else {m_geometryType=kActive;}

      ATH_MSG_INFO("Use the MiniFCALCalibrationCalculator for the MiniFCAL");

      // Initialize the geometry and energy calculators.
      m_geometryCalculator = MiniFCALAssignIdentifier::GetInstance();
      //m_energyCalculator   = new CaloG4::SimulationEnergies();

      return StatusCode::SUCCESS;
    }


    MiniFCALCalibrationCalculator::~MiniFCALCalibrationCalculator() {
      // Clean up pointers.
      //delete m_energyCalculator;
    }

    G4bool MiniFCALCalibrationCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                             std::vector<G4double> & energies,
                                             const eCalculatorProcessing process) const
    {
      if ( process == kEnergyAndID  ||  process == kOnlyEnergy ) {
          m_energyCalculator.Energies( step, energies );
      } else {
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );
      }


      if ( process == kEnergyAndID  ||  process == kOnlyID ) {
        // Calculate the identifier.
          identifier = m_geometryCalculator->CalculateIdentifier(step, m_geometryType );
          // Check for bad result.
          if ( identifier == LArG4Identifier() ) return false;

          return true;
        }
      return true;
    }

  } // namespace MiniFCAL

} // namespace LArG4
