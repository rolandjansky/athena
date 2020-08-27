/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::CalibrationCalculator

#undef DEBUG_HITS

#include "CalibrationCalculator.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/ILArCalculatorSvc.h"

#include "G4Step.hh"
#include "globals.hh"

namespace LArG4 {

  namespace EC {

    //CalibrationCalculator::CalibrationCalculator(LArG4::LArWheelCalculator_t t, int zside)
    CalibrationCalculator::CalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_wcalc_tProp(0)
      , m_wcalc_t(LArG4::InnerAbsorberWheel)
      , m_geometryCalculator("",name)
    {
      declareProperty("WheelType", m_wcalc_tProp);
      m_wcalc_tProp.declareUpdateHandler(&CalibrationCalculator::WheelTypeHandler, this);
      declareProperty("zSide", m_zside=0);
      declareProperty("GeometryCalculator",m_geometryCalculator);
    }

    void CalibrationCalculator::WheelTypeHandler(Gaudi::Details::PropertyBase&)
    {
      switch(m_wcalc_tProp.value())
        {
        case  0: m_wcalc_t = LArG4::InnerAbsorberWheel; break;
        case  1: m_wcalc_t = LArG4::OuterAbsorberWheel; break;
        case  2: m_wcalc_t = LArG4::InnerElectrodWheel; break;
        case  3: m_wcalc_t = LArG4::OuterElectrodWheel; break;
        case  4: m_wcalc_t = LArG4::InnerAbsorberModule; break;
        case  5: m_wcalc_t = LArG4::OuterAbsorberModule; break;
        case  6: m_wcalc_t = LArG4::InnerElectrodModule; break;
        case  7: m_wcalc_t = LArG4::OuterElectrodModule; break;
        case  8: m_wcalc_t = LArG4::BackInnerBarretteWheel; break;
        case  9: m_wcalc_t = LArG4::BackOuterBarretteWheel; break;
        case 10: m_wcalc_t = LArG4::BackInnerBarretteWheelCalib; break;
        case 11: m_wcalc_t = LArG4::BackOuterBarretteWheelCalib; break;
        case 12: m_wcalc_t = LArG4::BackInnerBarretteModule; break;
        case 13: m_wcalc_t = LArG4::BackOuterBarretteModule; break;
        case 14: m_wcalc_t = LArG4::BackInnerBarretteModuleCalib; break;
        case 15: m_wcalc_t = LArG4::BackOuterBarretteModuleCalib; break;
        case 16: m_wcalc_t = LArG4::InnerGlueWheel; break;
        case 17: m_wcalc_t = LArG4::OuterGlueWheel; break;
        case 18: m_wcalc_t = LArG4::InnerLeadWheel; break;
        case 19: m_wcalc_t = LArG4::OuterLeadWheel; break;
        default:
          {
            std::ostringstream merr;
            merr <<
              "CalibrationCalculator::WheelTypeHandler FATAL: invalid LArWheelCalculator_t specified "
                 << m_wcalc_tProp.value();
            std::cerr << merr.str() << std::endl;
            throw GaudiException(merr.str(), "CalibrationCalculator::WheelTypeHandler", StatusCode::FAILURE);
          }
        }
    }

    StatusCode CalibrationCalculator::initialize() {
      ATH_CHECK(m_geometryCalculator.retrieve());

      return StatusCode::SUCCESS;
    }


    CalibrationCalculator::~CalibrationCalculator()
    {
      // Clean up pointers.
      //delete m_geometryCalculator;
    }

    G4bool CalibrationCalculator::Process (const G4Step* a_step,
                                           LArG4Identifier & identifier,
                                           std::vector<G4double> & energies,
                                           const eCalculatorProcessing a_process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
        {
#ifdef DEBUG_HITS
          std::cout << "LArG4::EC::CalibrationCalculator::Process"
                    << " calling SimulationEnergies"
                    << " at m_energyCalculator="
                    << m_energyCalculator
                    << std::endl;
#endif
          m_energyCalculator.Energies( a_step, energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID ) {
        // Calculate the identifier.
        //(void)m_geometryCalculator->Process( a_step );
        // identifier =  m_geometryCalculator->identifier();
        std::vector<LArHitData> hdata;
        m_geometryCalculator->Process( a_step, hdata );
        identifier =  hdata[0].id;
      } else
        identifier = LArG4Identifier();


#ifdef DEBUG_HITS
      G4double energy = accumulate(energies.begin(),energies.end(),0.);
      std::cout << "LArG4::EC::CalibrationCalculator::Process"
                << " ID=" << std::string(identifier)
                << " energy=" << energy
                << " energies=(" << energies[0]
                << "," << energies[1]
                << "," << energies[2]
                << "," << energies[3] << ")"
                << std::endl;
#endif

      // Check for bad result.
      if ( identifier == LArG4Identifier() ) {
        std::cout << "return invalid from EmecCalibrationCalculator" << std::endl;
        return false;
      }

      return true;
    }

  } // namespace EC

} // namespace LArG4
