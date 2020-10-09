/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHECCalibrationWheelCalculator.h"

#include "LArG4HEC/IHECGeometry.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include <algorithm>

#undef DEBUG_HITS

namespace LArG4 {

  namespace HEC {

    LArHECCalibrationWheelCalculator::LArHECCalibrationWheelCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_geometryCalculator("HECGeometry",name) //FIXME LArG4::HEC::HECGeometry
      , m_geometryTypeProp(0)
      , m_geometryType(kWheelActive)
    {
      declareProperty("GeometryCalculator",m_geometryCalculator);
      declareProperty("GeometryType",m_geometryTypeProp);
      m_geometryTypeProp.declareUpdateHandler(&LArHECCalibrationWheelCalculator::GeometryTypeUpdateHandler, this);
    }

    void LArHECCalibrationWheelCalculator::GeometryTypeUpdateHandler(Gaudi::Details::PropertyBase&)
    {
      switch(m_geometryTypeProp.value())
        {
        case 0: m_geometryType = kWheelActive; break;
        case 1: m_geometryType = kWheelInactive; break;
        case 2: m_geometryType = kWheelDead; break;
        default:
          std::ostringstream merr;
          merr <<
            "LArHECCalibrationWheelCalculator::GeometryTypeUpdateHandler FATAL: invalid eHECGeometryType specified "
               << m_geometryTypeProp.value();
          std::cerr << merr.str() << std::endl;
          throw GaudiException(merr.str(), "LArHECCalibrationWheelCalculator::GeometryTypeUpdateHandler", StatusCode::FAILURE);
        }

    }

    StatusCode LArHECCalibrationWheelCalculator::initialize() {

      ATH_MSG_INFO("Use the LArHECCalibrationWheelCalculator for the HEC");
      ATH_CHECK(m_geometryCalculator.retrieve());
      return StatusCode::SUCCESS;
    }

    LArHECCalibrationWheelCalculator::~LArHECCalibrationWheelCalculator() {
    }

    G4bool LArHECCalibrationWheelCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                                     std::vector<G4double> & energies,
                                                     const eCalculatorProcessing process) const
    {
      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
        {
          m_energyCalculator.Energies( step, energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );



      if ( process == kEnergyAndID  ||  process == kOnlyID )
        {
          // Calculate the identifier.

          identifier = m_geometryCalculator->CalculateIdentifier(step, m_geometryType );

          // Check for bad result.
          if ( identifier == LArG4Identifier() ) return false;

          return true;
        }
      return true;
    }

  } // namespace HEC

} // namespace LArG4
