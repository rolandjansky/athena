/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     PresamplerCalibrationCalculator.cxx
PACKAGE:  offline/LArCalorimeter/LArG4/LArG4Barrel

AUTHORS:  G. Unal, L. Carminati (on a template from Bill Selingman)
CREATED:  September, 2004

PURPOSE:  This class calculates the values needed for calibration hits
          in the barrel presampler of LAr calorimeter. This calculator is
          called in calibration runs (see LArBarrelPresamplerSDConsultant)
          for calibration hits in the presampler volume.

UPDATES:

********************************************************************/

// #define DEBUG_HITS

#include "PresamplerCalibrationCalculator.h"

#include "LArBarrelPresamplerGeometry.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "globals.hh"
#include <algorithm>

namespace LArG4 {

  namespace BarrelPresampler {

    CalibrationCalculator::CalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_geometryCalculator("LArBarrelPresamplerGeometry", name)
    {
      declareProperty("GeometryCalculator", m_geometryCalculator);
    }

    StatusCode CalibrationCalculator::initialize() {
      // Initialize the geometry calculator
      ATH_CHECK(m_geometryCalculator.retrieve());
      return StatusCode::SUCCESS;
    }

    CalibrationCalculator::~CalibrationCalculator()
    {
    }

    G4bool CalibrationCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                          std::vector<G4double> & energies,
                                          const eCalculatorProcessing process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
        {
#ifdef DEBUG_HITS
          std::cout << "LArG4::Barrel::CalibrationCalculator::Process"
                    << " calling SimulationEnergies" << std::endl;
#endif
          m_energyCalculator.Energies( step, energies );

          // First, get the energy.
          //m_energy = step->GetTotalEnergyDeposit();
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


      if ( process == kEnergyAndID  ||  process == kOnlyID )
        {
          // Calculate the identifier.
          identifier = m_geometryCalculator->CalculateIdentifier( step );
        }
      else
        identifier = LArG4Identifier();


#ifdef DEBUG_HITS
      G4double energy = accumulate(energies.begin(),energies.end(),0.);
      std::cout << "LArG4::Barrel::CalibrationCalculator::Process"
                << " ID=" << std::string(identifier)
                << " energy=" << energy
                << " energies=(" << energies[0]
                << "," << energies[1]
                << "," << energies[2]
                << "," << energies[3] << ")"
                << std::endl;
#endif

      // Check for bad result.
      if ( identifier == LArG4Identifier() )
        return false;

      return true;
    }

  } // namespace Barrel

} // namespace LAr
