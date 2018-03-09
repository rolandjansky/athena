/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::BarrelCryostat::CalibrationLArCalculator
// Prepared 06-Jul-2004 Bill Seligman
// 16-Jul-2004 M.Leltchouk - extended for LAr::Barrel::MotherVolume

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS
#define DEBUG_VOLUMES

#include "CryostatCalibrationLArCalculator.h"

#include "LArG4Code/LArG4Identifier.h"
//#include "LArG4Code/CalibrationDefaultCalculator.h"

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

    CalibrationLArCalculator::CalibrationLArCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_parameters(nullptr)
      , m_defaultCalculator("CalibrationDefaultCalculator",name)
    {
       declareProperty("DefaultCalculator",m_defaultCalculator);
    }

    StatusCode CalibrationLArCalculator::initialize(){
      // Get the default calculator (hopefully a temporary piece of code):
      ATH_CHECK(m_defaultCalculator.retrieve());

      // Access source of detector parameters.
      m_parameters = LArVG4DetectorParameters::GetInstance();
      return StatusCode::SUCCESS;
    }


    CalibrationLArCalculator::~CalibrationLArCalculator()
    {
      // Cleanup pointers.
      //delete m_defaultCalculator;
      //m_defaultCalculator = 0;
    }

    G4bool CalibrationLArCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
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

          // static ?
          static const double oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
          static const double oneOverDphi = 32./M_PI;  //   1/Dphi

          static const double rhoMinPresamplerMother = 1385.*CLHEP::mm;
          static const double rhoMiddlePresampler = (1385.*CLHEP::mm + 1447.*CLHEP::mm)/2.;
          // from PresParameterDef.icc
          //   rMinPresamplerMother = 1385*CLHEP::mm;
          //   rMaxPresamplerMother = 1447*CLHEP::mm - 0.001*CLHEP::mm;

          static const double rhoAlignmentSafety = 10.*CLHEP::mm;
          static const double rhoInFrontOfColdWall = rhoMinPresamplerMother - rhoAlignmentSafety;
          static double RIN_AC  = INT_MIN;
          if ( RIN_AC < 0.)
            RIN_AC = m_parameters->GetValue("LArEMBRadiusInnerAccordion"); // 1500.024*CLHEP::mm; from ACCG
          static const double RCUT12  = 1593.9*CLHEP::mm;
          static const double RCUT23  = 1866.1*CLHEP::mm;
          static double ROUT_AC = INT_MIN;
          if ( ROUT_AC < 0. )
            ROUT_AC = m_parameters->GetValue("LArEMBRadiusOuterAccordion"); // 1960.*CLHEP::mm;
          static const double rhoOuterAccordionWithSafety = ROUT_AC - rhoAlignmentSafety;
          static double LArEMBZmax = INT_MIN;
          if ( LArEMBZmax < 0. )
            LArEMBZmax = m_parameters->GetValue("LArEMBZmax"); // 3165.*CLHEP::mm

          const double zMaxAccordionWithSafety = LArEMBZmax - 10.*CLHEP::mm;


          // Calculate the mid-point of the step, and the simple geometry variables.

          G4StepPoint* pre_step_point = step->GetPreStepPoint();
          G4StepPoint* post_step_point = step->GetPostStepPoint();

          G4ThreeVector startPoint = pre_step_point->GetPosition();
          G4ThreeVector endPoint   = post_step_point->GetPosition();
          G4ThreeVector p = (startPoint + endPoint) * 0.5;

          G4double rho = p.perp();
          G4double eta = fabs( p.pseudoRapidity() );
          G4double phi = p.phi();
          if ( phi < 0. ) phi += 2.*M_PI; // Normalize for phiBin calculation

          // subdet = +/-4    "+" or " -" according to sign of Z in World coorinate
          G4int subdet = ( p.z() > 0.) ? 4 : -4;
          G4int phiBin = (int) ( phi * oneOverDphi );
          if (phiBin>63) phiBin=0;

          // Initialize identifier variables with (invalid) default
          // values (INT_MIN is defined in <climits>).
          G4int type = INT_MIN;
          G4int sampling = INT_MIN;
          G4int region = INT_MIN;
          G4int etaBin = INT_MIN;


          if(std::fabs(p.z())< 10 &&  eta < 0.1 && rho > rhoMinPresamplerMother && rho < ROUT_AC )
            {
              // type = 2  dead materials in "internal" cracks:
              // real crack for nominal geometry at this level is +-3mm
              // add some margin to allow for misalignement

              type = 2;

              // sampling = 0,1,2,3 (follow the corresponding calorimeter structure)
              if ( rho < RIN_AC )      { sampling = 0; }
              else if ( rho < RCUT12 ) { sampling = 1; }
              else if ( rho < RCUT23 ) { sampling = 2; }
              else                     { sampling = 3; }

              // region = 0 - between two halves of EMB,
              region = 0;
              etaBin = 0;
            }
          else
            {
              // type = 1 dead materials outside accordion and active presampler layers
              type = 1;

              if( eta < 1.5 )
                {
                  if ( rho < rhoInFrontOfColdWall ) // and in LAr::Barrel::MotherVolume

                    // The region is defined correctly for a radial mis-alignments
                    // up to 10mm because ColdWallThickness >= 20mm and
                    // rhoAlignmentSafety = 10.*mm;
                    // The E-deposit in the Cold Wall itself is handled by
                    // another calculator.
                    {
                      sampling = 1;
                      region = 0;
                      etaBin = (int) ( eta * oneOverDeta );
                    }
                  else if ( rho < rhoMiddlePresampler )
                    // The region is defined correctly for a radial mis-alignments
                    // up to (1447.*mm - 1385.*mm)/2
                    // The E-deposit in the Presampler itself is handled by
                    // other calculator.
                    {
                      sampling = 1;
                      region = 2;
                      etaBin = (int) ( eta * oneOverDeta );
                    }
                  else if ( rho < rhoOuterAccordionWithSafety &&
                            fabs( p.z()) < zMaxAccordionWithSafety )
                    // The region is defined correctly for a radial mis-alignments
                    // up to 10mm because rhoAlignmentSafety = 10.*mm;
                    // The E-deposit in the Barrel Accordion itself is handled by
                    // other calculator.
                    {
                      sampling = 1;
                      region = 3;
                      etaBin = (int) ( eta * oneOverDeta );
                    }
                  else  // rho > ROUT_AC or |z| >= zMaxAccordionWithSafety
                    {
                      sampling = 2;

                      if( eta < 1.0 )
                        {
                          region = 0;
                          etaBin = (int) ( eta * oneOverDeta );
                        }
                      else // 1.0 <= eta < 1.5
                        {
                          region = 2;
                          etaBin = (int) ( (eta-1.) * oneOverDeta );
                        }
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
                  etaBin = (int) ( (eta-1.6) * oneOverDeta );
                }
              else if ( eta < 3.2 )
                {
                  sampling = 1;
                  region = 6;
                  etaBin = (int) ( (eta-1.8) * oneOverDeta );
                }
            }

          // This is a "quick fix" for a complex issue: We're still
          // developing code for the cryostat.  What if, somehow, we have
          // a G4Step in a LAr volume that isn't handled by the above
          // code?  Answer: Use the default calibration calculator (the
          // same one used for volumes without sensitive detectors) to get
          // the identifier.

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
                  std::cout << "LArG4::BarrelCryostat::CalibrationLArCalculator::Process"
                            << " (error " << messageCount << " of " << messageMax << " max displayed)"
                            << std::endl
                            << "   G4Step in LAr at unexpected place: (x,y,z) [mm] = ("
                            << p.x()/CLHEP::mm << ","
                            << p.y()/CLHEP::mm << ","
                            << p.z()/CLHEP::mm
                            << "); eta=" << eta
                            << ", phi=" << phi << std::endl
                            << "   using default calculator"
                            << std::endl;
                }
#endif
              m_defaultCalculator->Process(step, identifier, energies, process);
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
      std::cout << "LArG4::BarrelCryostat::CalibrationLArCalculator::Process"
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
