/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::BarrelCryostat::CalibrationMixedCalculator
// Prepared 24-Aug-2004 William Seligman
// from code prepared by Mikhail Leltchouk

// This class calculates the values needed for calibration hits in the
// simulation.

#undef  DEBUG_HITS
#define DEBUG_VOLUMES

#include "CryostatCalibrationMixedCalculator.h"
#include "CryostatCalibrationLArCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <algorithm>
#include <cmath>
#include <climits>

namespace LArG4 {

  namespace BarrelCryostat {

     ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationMixedCalculator::CalibrationMixedCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator)
  , m_backupCalculator("BarrelCryostatCalibrationLArCalculator",name)
    {
       declareProperty("BackupCalculator", m_backupCalculator);
    }

    StatusCode CalibrationMixedCalculator::initialize() {
      // Get a "backup" calculator.
      ATH_CHECK(m_backupCalculator.retrieve());

      return StatusCode::SUCCESS;
    }


    CalibrationMixedCalculator::~CalibrationMixedCalculator()
    {
      // Cleanup pointers.
      //delete m_backupCalculator;
      //m_backupCalculator = 0;
    }

    // This calculator is intended to apply to the following volumes that have "mixed" identifiers:
    //      volumeName   "LAr::Barrel::Cryostat::Cylinder::Mixed"
    //      copy number   2, 47, 49,
    //
    //      volumeName   "LAr::Barrel::Cryostat::Cone::Mixed"
    //      copy number   8, 9,
    //
    //      volumeName   "LAr::Barrel::Cryostat::Half::Cylinder::Mixed"
    //      copy number   21
    //
    // G. Pospelov (8-Fev-2006)
    // Actually, this calculator is intended to apply to the following volumes
    // LArMgr::LAr::Barrel::Cryostat::Mixed::Cylinder::*
    // LArMgr::LAr::Barrel::Cryostat::OuterWall
    // LArMgr::LAr::Barrel::Cryostat::InnerEndWall
    // See also Simulation/G4Atlas/G4AtlasApps/python/atlas_calo.py and
    // LArG4/LArG4Barrel/src/CryostatCalibrationCalculator.cxx

    G4bool CalibrationMixedCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                               std::vector<G4double> & energies,
                                               const eCalculatorProcessing process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
        {
          m_energyCalculator.Energies( step, energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


      identifier.clear();
      if ( process == kEnergyAndID  ||  process == kOnlyID )
        {
          // Calculate the identifier.

          // Note:
          // LArG4::BarrelCryostat::CryostatCalibrationCalculator uses
          // a table-based approach to determine the identifier.  The
          // following code uses an "if-statement" approach.

          // The fixed parameters (only a couple of which are readily
          // accessible from the database):

          static const double oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
          static const double oneOverDphi = 32./M_PI;  //   1/Dphi

          // Calculate the mid-point of the step, and the simple geometry variables.
          G4VPhysicalVolume* physical = step->GetPreStepPoint()->GetPhysicalVolume();
          //G4int copyNumber = physical->GetCopyNo();
          G4String volumeName = physical->GetLogicalVolume()->GetName();

          G4StepPoint* pre_step_point = step->GetPreStepPoint();
          G4StepPoint* post_step_point = step->GetPostStepPoint();

          G4ThreeVector startPoint = pre_step_point->GetPosition();
          G4ThreeVector endPoint   = post_step_point->GetPosition();
          G4ThreeVector p = (startPoint + endPoint) * 0.5;

          G4double rho = p.perp();
          G4double eta = fabs( p.pseudoRapidity() );
          G4double phi = p.phi();
          if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation

          // Initialize identifier variables with (invalid) default
          // values (INT_MIN is defined in <climits>).
          G4int sampling = INT_MIN;
          G4int region = INT_MIN;
          G4int etaBin = INT_MIN;
          // subdet = +/-4    "+" or " -" according to sign of Z in World coorinate
          G4int subdet = ( p.z() > 0.) ? 4 : -4;
          G4int type = 1;
          G4int phiBin = (int) ( phi * oneOverDphi );
          if (phiBin>63) phiBin=0;

          static double rho12 = 1386.+10.; // use old hardcoded 1386 for Sampling 1-2 transition
                                           // and add 10mm for safety (misalignment)

          if ( eta < 1.0 )
            {
              if ( rho < rho12) // LAr::Barrel::Cryostat::OuterWall
                {
                  sampling = 1;
                  region = 1;
                  etaBin = (int) ( eta * oneOverDeta );
                }
              else
                {
                  sampling = 2;
                  region = 0;
                  etaBin = (int) ( eta * oneOverDeta );
                }
            }
          else if ( eta < 1.5 )
            {
              if ( rho < rho12) // LAr::Barrel::Cryostat::OuterWall
                {
                  sampling = 1;
                  region = 1;
                  etaBin = (int) ( eta * oneOverDeta );
                }
              else
                {
                  sampling = 2;
                  region = 2;
                  etaBin = (int) ( (eta-1.) * oneOverDeta );
                }
            }
          else if ( eta < 1.6 )
            {
              sampling = 1;
              region = 4;
              etaBin = (int) ( (eta-1.5) * oneOverDeta );
            }
          else if ( eta < 1.8 )
            {
              sampling = 1;
              region = 5;
              etaBin = (int) ( (eta-1.5) * oneOverDeta );
            }
          else
            {
              sampling = 1;
              region = 6;
              etaBin = (int) ( (eta-1.3) * oneOverDeta );
            }

          // What if we have a G4Step that isn't handled by the above
          // code?  Answer: Use a "backup" calculator to try to
          // process the step.

          if ( type     == INT_MIN  ||
               region   == INT_MIN  ||
               sampling == INT_MIN  ||
               etaBin   == INT_MIN  ||
               phiBin   <  0 )
            {
#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
              static const G4int messageMax = 10;
              static G4int messageCount = 0;
              if ( messageCount++ < messageMax )
                {
                  std::cout << "LArG4::BarrelCryostat::CalibrationMixedCalculator::Process"
                            << " (error " << messageCount << " of " << messageMax << " max displayed)"
                            << std::endl
                            << "   G4Step in '"
                            << step->GetPreStepPoint()->GetPhysicalVolume()->GetName()
                            << "', using backup calculator"
                            << std::endl;
                }
#endif
              m_backupCalculator->Process(step, identifier, energies, process);
            }
          else
            {
              // Append the cell ID to the (empty) identifier.
              identifier << 10         // Calorimeter
                           << subdet     // LAr +/-4 where "+" or " -" according to
                                         // the sign of Z in World coorinate
                           << type
                           << sampling
                           << region
                           << etaBin
                           << phiBin;
            }
        }

#ifdef DEBUG_HITS
      G4double energy = accumulate(energies.begin(),energies.end(),0.);
      std::cout << "LArG4::BarrelCryostat::CalibrationMixedCalculator::Process"
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

  } // namespace BarrelCryostat

} // namespace LArG4
