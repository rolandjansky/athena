/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::CalibrationDefaultCalculator
// Prepared 04-Mar-2004 Bill Seligman
// Modified 22-Sep-2004 Mikhail Leltchouk

// The calibration studies rely on every volume in the simulation
// being made into a sensitive detector.  There is a practical
// problem: What if we're still in the middle of developing code, and
// not every volume has been made sensitive yet?  What if we've
// overlooked a volume?  Or (the most common case), what if we have an
// energy deposit in a volume that is not being directly calibrated?

// This class provides a "default behavior" for all energy deposits
// that are not made in a volume that's been made sensitive for
// calibration studies.

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS
#undef DEBUG_VOLUMES

#include "CalibrationDefaultCalculator.h"
#include "LArG4Code/LArG4Identifier.h"

#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <string>
#include <climits>
#include <algorithm>
#include <set>
#include <numeric>

#undef DEBUG_DMXYZ

namespace LArG4 {

  CalibrationDefaultCalculator::CalibrationDefaultCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator)
  {
  }


  CalibrationDefaultCalculator::~CalibrationDefaultCalculator()
  {
  }

  G4bool CalibrationDefaultCalculator::Process (const G4Step* a_step,
                  LArG4Identifier & _identifier,
                  std::vector<G4double> & _energies,
                  const eCalculatorProcessing a_process) const
  {
    // Use the calculators to determine the energies and the
    // identifier associated with this G4Step.  Note that the
    // default is to process both the energy and the ID.

    _energies.clear();
    if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
      {
        m_energyCalculator.Energies( a_step, _energies );
      }
    else {
      for (unsigned int i=0; i != 4; i++) _energies.push_back(0.);
    }

    _identifier.clear();
    if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
      {
        // Call after volume-by-volume calls only if none of these calls assigned
        // the SimulationEnergies::Energies of current step to identifier and one
        // of the existing hit collections.

        // Calculate the mid-point of the step, and the simple geometry variables.

        const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
        const G4StepPoint* post_step_point = a_step->GetPostStepPoint();

        const G4ThreeVector startPoint = pre_step_point->GetPosition();
        const G4ThreeVector endPoint   = post_step_point->GetPosition();
        const G4ThreeVector p = (startPoint + endPoint) * 0.5;

        const G4double rho = p.perp();
        const G4double eta = fabs( p.pseudoRapidity() );
        G4double phi = p.phi();
        if ( phi<0. ) { phi += 2.*M_PI; } // Normalize for phiBin calculation

        const G4int detector = 10;        // calorimeter "dead" materials

        // Initialize identifier variables with (invalid) default
        // values (INT_MIN is defined in <climits>).
        G4int subdet   = INT_MIN;
        G4int type     = INT_MIN;
        G4int sampling = INT_MIN;
        G4int region   = INT_MIN;
        G4int etaBin   = INT_MIN;
        G4int phiBin   = INT_MIN;

        if ( eta > 5. ) // "forward" leakage
          {
            // subdet = +/-4, "+" or " -" according to sign of Z in World coorinate
            subdet = ( p.z() > 0.) ? 4 : -4;
            // type = 1, sampling = 3  leakage outside calorimeters:
            type = 1;
            sampling = 3;
            if ( eta < 8.)      // leakage between eta = 5 and 8
              {
                region = 1;
                etaBin = (int) ( ( eta - 5. ) * 0.5 * m_oneOverDeta );
                phiBin = (int) ( phi * m_oneOverDphi );
                if (phiBin>m_phiBinMax) phiBin=m_phiBinMax;
              }
            else                // "very forward" leakage  eta >= 8.
              {
                region = 2;
                etaBin = 0;     // no eta-phi subdivision
                phiBin = 0;
              }
          }
        else if ( rho > m_rhoCalorOut || fabs( p.z() ) > m_zCalorOut )
          {
            // type = 1, sampling = 3  leakage outside calorimeters:
            type = 1;
            sampling = 3;
            region = 0;
            phiBin = (int) ( phi * m_oneOverDphi );
            if (phiBin>m_phiBinMax) phiBin=m_phiBinMax;

            if ( eta < 1.7)      // leakage outside Tile
              {
                // subdet = +/-5, "+" or " -" according to sign of Z in World coorinate
                subdet = ( p.z() > 0.) ? 5 : -5;
                etaBin = (int) ( eta * m_oneOverDeta );
              }
            else             // leakage outside LAr HEC
              {
                // subdet = +/-4, "+" or " -" according to sign of Z in World coorinate
                subdet = ( p.z() > 0.) ? 4 : -4;
                etaBin = (int) ( ( eta - 1.7 ) * m_oneOverDeta );
              } // outside at eta covered by Tile or by LAr HEC

          }     // leakage outside calorimeters:

        else if ( rho < m_rhoInDetOut && fabs( p.z() ) < m_zInDetOut )

          {
            subdet = ( p.z() > 0.) ? 4 : -4;
            // type = 1, sampling = 0, region = 1-5 => Inner Detector
            type = 1;
            sampling = 0;
            region = 5; // TRT support, cables, services
            if ( rho < 980.*CLHEP::mm ) region = 4;  // TRT
            if ( rho < 650.*CLHEP::mm ) region = 3;  // support
            if ( rho < 540.*CLHEP::mm ) region = 2;  // SCT
            if ( rho < 270.*CLHEP::mm ) region = 1;  // Pixels with support, beam pipe
            etaBin = (int) ( eta * m_oneOverDeta );
            phiBin = (int) ( phi * m_oneOverDphi );
            if (phiBin>m_phiBinMax) phiBin=m_phiBinMax;

          // g.p. 23.05.2011 beam pipe material outside of inner detector area close to FCal
          } else if( fabs( p.z() ) >= m_zInDetOut) {
            subdet = ( p.z() > 0.) ? 4 : -4;
            phiBin = (int) ( phi * m_oneOverDphi );
            if (phiBin>m_phiBinMax) phiBin=m_phiBinMax;
            if(eta>=2.9 && eta<5.0) {
              if(fabs(p.z()) < m_z1BeforeFCal) {
                type = 1;
                sampling = 1;
                region = 7;
                etaBin = (int) ( (eta-3.2) * m_oneOverDeta );
              } else if (fabs(p.z()) < m_z2BeforeFCal){
                type = 2;
                sampling = 0;
                region = 5;
                etaBin = (int) ( (eta-3.0) * m_oneOverDeta );
              } else if (fabs(p.z()) < m_startZFCal1) {
                type = 2;
                sampling = 1;
                region = 5;
                etaBin = (int) ( (eta-3.0) * m_oneOverDeta );
              }
          // g.p. back leakages in beam pipe
          } else if ( eta >= 5.  && eta < 8.0) {
            type = 1;
            sampling = 3;
            region = 1;
            etaBin = (int) ( (eta-5.)* 0.5 * m_oneOverDeta );
          } else if (eta>8.) {
            type = 1;
            sampling = 3;
            region = 2;
            etaBin = 0;
            phiBin = 0;
          }
          if(etaBin<0) etaBin=0;
        }


        // If the point falls outside any of the above "if"
        // statements, use a "none-of-the-above" identifier.

        if
          ( subdet   == INT_MIN  ||
            type     == INT_MIN  ||
            sampling == INT_MIN  ||
            region   == INT_MIN  ||
            etaBin   == INT_MIN  ||
            phiBin   == INT_MIN )
          {
#if defined (DEBUG_VOLUMES) || defined (DEBUG_HITS)
            static std::set<G4String> volumeList;
            const G4String namePhys = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
            const G4String nameLog  = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();

            std::set<G4String>::iterator i = volumeList.find( nameLog );
            if ( i == volumeList.end() )
              {
                volumeList.insert( nameLog );
                std::cout << "LArG4::CalibrationDefaultCalculator::Process"
                          << std::endl
                          << "   default ID used in logical volume '"
                          << nameLog
                          << "'"
                          << std::endl
                          << "   (each such volume is only listed once)"
                          << std::endl;
              }

            static const G4int messageMax = 10;
            static G4int messageCount = 0;
            if ( messageCount++ < messageMax )
              {
                std::cout << "   detailed error (" << messageCount
                          << " of " << messageMax << " max displayed):"
                          << std::endl
                          << "   G4Step at unexpected place: (x,y,z) [mm] = ("
                          << p.x()/mm << ","
                          << p.y()/mm << ","
                          << p.z()/mm
                          << "),  eta=" << eta
                          << ", phi=" << phi
                          << ", rho=" << rho
                          << std::endl
                          << "   in physical volume '" << namePhys << "'"
                          << std::endl
                          << "   (subdet,type,sampling,region,etaBin,phiBin)=("
                          << subdet << ","
                          << type << ","
                          << sampling << ","
                          << region << ","
                          << etaBin << ","
                          << phiBin
                          << "); using default ID"
                          << std::endl;
              }
#endif
            subdet = ( p.z() > 0.) ? 4 : -4;
            type = 1;
            sampling = 0;
            region   = 0;
            etaBin = (int) ( eta * m_oneOverDeta );
            if ( etaBin > 49 ) etaBin = 49;
            phiBin = (int) ( phi * m_oneOverDphi );
            if (phiBin>m_phiBinMax) phiBin=m_phiBinMax;
#ifdef DEBUG_DMXYZ
            G4double energy = std::accumulate(_energies.begin(),_energies.end(), 0.);
            if(energy > 1e-15) LArG4::CalibrationDefaultCalculator::Print("UNEXP DefaultCalculator",_identifier,a_step,_energies);
#endif
          }

        // Create the LArG4Identifier.
                _identifier << detector
                     << subdet
                     << type
                     << sampling
                     << region
                     << etaBin
                     << phiBin;
      }

#ifdef DEBUG_HITS
    G4double energy = accumulate(_energies.begin(),_energies.end(),0.);
    std::cout << "LArG4::CalibrationDefaultCalculator::Process"
              << " ID=" << std::string(_identifier)
              << " energy=" << energy
              << " energies=(" << _energies[0]
              << "," << _energies[1]
              << "," << _energies[2]
              << "," << _energies[3] << ")"
              << std::endl;
#endif
#ifdef DEBUG_DMXYZ
//    LArG4::CalibrationDefaultCalculator::Print("DMXYZ DefaultCalculator",_identifier,a_step,_energies);
#endif

    // Check for bad result.
    if ( _identifier == LArG4Identifier() )
      return false;

    return true;
  }

} // namespace LArG4
