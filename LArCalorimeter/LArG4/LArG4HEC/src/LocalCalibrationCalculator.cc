/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LocalCalibrationCalculator.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include <algorithm>

#undef DEBUG_HITS

namespace LArG4 {

  namespace HEC {

    LocalCalibrationCalculator::LocalCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_geometryCalculator("LocalHECGeometry",name)
      , m_geometryType(kLocActive)
     {
       declareProperty("GeometryCalculator", m_geometryCalculator);
       declareProperty("GeometryType",m_strgeometryType="ACTIVE");
       m_strgeometryType.declareUpdateHandler(&LocalCalibrationCalculator::GeometryTypeUpdateHandler, this);
#ifdef DEBUG_HITS
       std::cout << "LArG4::HEC::LocalCalibrationCalculator constructed" << std::endl;
#endif
     }

    void LocalCalibrationCalculator::GeometryTypeUpdateHandler(Property&)
    {
      std::string geoTypeString = m_strgeometryType.value();
      std::transform(geoTypeString.begin(), geoTypeString.end(),geoTypeString.begin(), ::toupper);
      if(geoTypeString.find("DEAD") != std::string::npos)
        {
          m_geometryType=kLocDead;
        }
      else if (geoTypeString.find("INACTIVE") != std::string::npos)
        {
          m_geometryType=kLocInactive;
        }
      else if (geoTypeString.find("ACTIVE") != std::string::npos)
        {
          m_geometryType=kLocActive;
        }
      else
        {
          std::ostringstream merr;
          merr <<
            "LArG4::HEC::LocalCalibrationCalculator::GeometryTypeUpdateHandler FATAL: invalid eHECGeometryType specified "
               << geoTypeString;
          std::cerr << merr.str() << std::endl;
          throw GaudiException(merr.str(), "LArG4::HEC::LocalCalibrationCalculator::GeometryTypeUpdateHandler", StatusCode::FAILURE);
        }

    }

    StatusCode LocalCalibrationCalculator::initialize()
    {
      // Initialize the geometry calculator.
      ATH_CHECK(m_geometryCalculator.retrieve());

      return StatusCode::SUCCESS;
    }

    LocalCalibrationCalculator::~LocalCalibrationCalculator() {
    }


    G4bool LocalCalibrationCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                          std::vector<G4double> & energies,
                                          const eCalculatorProcessing process) const
    {

#ifdef DEBUG_HITS
	  std::cout << "LArG4::HEC::LocalCalibrationCalculator::Process" << std::endl;
#endif
      energies.clear();
      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
	{
#ifdef DEBUG_HITS
          std::cout << " calling SimulationEnergies" << std::endl;
#endif
	  m_energyCalculator.Energies( step, energies );
	}
      else
	for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


      if ( process == kEnergyAndID  ||  process == kOnlyID )
	{
	   // Calculate the identifier.
           G4StepPoint* pre_step_point = step->GetPreStepPoint();
           const G4TouchableHistory* theTouchable = static_cast<const G4TouchableHistory*>(pre_step_point->GetTouchable());
           // Volume name 
           G4String hitVolume = theTouchable->GetVolume(0)->GetName();
#ifdef DEBUG_HITS
          std::cout<<"LArHECLocalCalibrationCalculator::Process Volume: "<<hitVolume<<std::endl;
#endif
          if(hitVolume.contains("::") ) {
             const int last = hitVolume.last(':');
             hitVolume.remove(0,last+1);
          }
          static const G4String sliceVolume("Slice");
          static const G4String absorberVolume("Absorber");
          static const G4String electrodeVolume("Electrode");
          static const G4String copperVolume("Copper");
          static const G4String tieRodVolume("TieRod");
	  if(m_geometryType != kLocDead) {
             if(hitVolume==sliceVolume) {
                 identifier = m_geometryCalculator->CalculateIdentifier(step, kLocInactive, 0, 4.*CLHEP::mm);
	     } else if(hitVolume==absorberVolume) {
                 identifier = m_geometryCalculator->CalculateIdentifier(step, kLocInactive, 0, 2.*CLHEP::mm, 1.02*CLHEP::mm);
             } else if(hitVolume==electrodeVolume) { 
                 identifier = m_geometryCalculator->CalculateIdentifier(step, kLocInactive, 1, 4.*CLHEP::mm);
             } else if(hitVolume==copperVolume) {
                 identifier = m_geometryCalculator->CalculateIdentifier(step, kLocInactive, 2, 4.*CLHEP::mm);
             } else if(hitVolume==tieRodVolume){ // We should call another functions for TieRods 
                 identifier = m_geometryCalculator->CalculateIdentifier(step, kLocActive, -1);
	     } else identifier = LArG4Identifier();
	  } else identifier = m_geometryCalculator->CalculateIdentifier(step, m_geometryType, 0, 4.*CLHEP::mm);

#ifdef DEBUG_HITS
	  G4double energy = accumulate(energies.begin(),energies.end(),0.);
	  std::cout << "LArG4::HEC::LocalCalibrationCalculator::Process"
		    << " ID=" << std::string(identifier)
		    << " energy=" << energy
		    << " energies=(" << energies[0]
		    << "," << energies[1]
		    << "," << energies[2]
		    << "," << energies[3] << ")"
		    << std::endl;
#endif

          // Check for bad result.
          if ( identifier == LArG4Identifier() ) return false;

          return true;
        }
      return true;
    }

  } // namespace HEC

} // namespace LArG4
