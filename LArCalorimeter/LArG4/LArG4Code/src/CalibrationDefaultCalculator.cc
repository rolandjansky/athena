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

#include "LArG4Code/CalibrationDefaultCalculator.h"
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

  CalibrationDefaultCalculator::CalibrationDefaultCalculator() 
  {
  }


  CalibrationDefaultCalculator::~CalibrationDefaultCalculator() 
  {
  }


  G4bool CalibrationDefaultCalculator::Process( const G4Step* a_step,
						const eCalculatorProcessing a_process )
  {
    // Use the calculators to determine the energies and the
    // identifier associated with this G4Step.  Note that the
    // default is to process both the energy and the ID.

    m_energies.clear();
    if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
      {
	m_energyCalculator.Energies( a_step, m_energies );
      }
    else {
      for (unsigned int i=0; i != 4; i++) m_energies.push_back(0.);
    }

    m_identifier.clear();
    if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
      {
	// Call after volume-by-volume calls only if none of these calls assigned
	// the SimulationEnergies::Energies of current step to identifier and one
	// of the existing hit collections.

	static const double oneOverDeta = 10.;       //   1/Deta = 1./0.1 = 10.
	static const double oneOverDphi = 32./M_PI;  //   1/Dphi
        static const int    phiBinMax = 63;
	static const double rhoAlignmentSafety = 50.*CLHEP::mm;
	static const double zAlignmentSafety = 100.*CLHEP::mm;
	static const double rhoCalorOut = 3885.*CLHEP::mm - rhoAlignmentSafety;
	static const double zCalorOut = 6100.*CLHEP::mm - zAlignmentSafety;
	static const double rhoInDetOut = 1150.*CLHEP::mm + rhoAlignmentSafety;
	static const double zInDetOut = 3511.*CLHEP::mm + zAlignmentSafety;
        static const double startZFCal1 = 4668.5 - zAlignmentSafety;
        static const double z1BeforeFCal = 4225.5 + zAlignmentSafety; // 
        static const double z2BeforeFCal = 4557.5 + zAlignmentSafety; // 
	// Calculate the mid-point of the step, and the simple geometry variables.

	G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
	G4StepPoint* post_step_point = a_step->GetPostStepPoint();

	G4ThreeVector startPoint = pre_step_point->GetPosition();
	G4ThreeVector endPoint   = post_step_point->GetPosition();
	G4ThreeVector p = (startPoint + endPoint) * 0.5;

	G4double rho = p.perp();
	G4double eta = fabs( p.pseudoRapidity() );
	G4double phi = p.phi(); 
	if ( phi<0. ) { phi += 2.*M_PI; } // Normalize for phiBin calculation

	G4int detector = 10;        // calorimeter "dead" materials

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
		etaBin = (int) ( ( eta - 5. ) * 0.5 * oneOverDeta );
		phiBin = (int) ( phi * oneOverDphi );
                if (phiBin>phiBinMax) phiBin=phiBinMax;
	      }
	    else                // "very forward" leakage  eta >= 8.
	      {
		region = 2;
		etaBin = 0;     // no eta-phi subdivision
		phiBin = 0;
	      }
	  }
	else if ( rho > rhoCalorOut || fabs( p.z() ) > zCalorOut ) 
	  {
	    // type = 1, sampling = 3  leakage outside calorimeters:
	    type = 1;
	    sampling = 3;
	    region = 0;
	    phiBin = (int) ( phi * oneOverDphi );
            if (phiBin>phiBinMax) phiBin=phiBinMax;

	    if ( eta < 1.7)      // leakage outside Tile  
	      { 
		// subdet = +/-5, "+" or " -" according to sign of Z in World coorinate
		subdet = ( p.z() > 0.) ? 5 : -5;
		etaBin = (int) ( eta * oneOverDeta );
	      } 
	    else             // leakage outside LAr HEC 
	      { 
		// subdet = +/-4, "+" or " -" according to sign of Z in World coorinate
		subdet = ( p.z() > 0.) ? 4 : -4;
		etaBin = (int) ( ( eta - 1.7 ) * oneOverDeta );
	      } // outside at eta covered by Tile or by LAr HEC

	  }     // leakage outside calorimeters:
	    
	else if ( rho < rhoInDetOut && fabs( p.z() ) < zInDetOut )

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
	    etaBin = (int) ( eta * oneOverDeta );
	    phiBin = (int) ( phi * oneOverDphi );
            if (phiBin>phiBinMax) phiBin=phiBinMax;

          // g.p. 23.05.2011 beam pipe material outside of inner detector area close to FCal
	  } else if( fabs( p.z() ) >= zInDetOut) {
            subdet = ( p.z() > 0.) ? 4 : -4;
            phiBin = (int) ( phi * oneOverDphi );
            if (phiBin>phiBinMax) phiBin=phiBinMax;
            if(eta>=2.9 && eta<5.0) {
              if(fabs(p.z()) < z1BeforeFCal) {
                type = 1;
                sampling = 1;
                region = 7;
                etaBin = (int) ( (eta-3.2) * oneOverDeta );
              } else if (fabs(p.z()) < z2BeforeFCal){
                type = 2;
                sampling = 0;
                region = 5;
                etaBin = (int) ( (eta-3.0) * oneOverDeta );
              } else if (fabs(p.z()) < startZFCal1) {
                type = 2;
                sampling = 1;
                region = 5;
                etaBin = (int) ( (eta-3.0) * oneOverDeta );
              }
          // g.p. back leakages in beam pipe
          } else if ( eta >= 5.  && eta < 8.0) {
            type = 1;
            sampling = 3;
            region = 1;
            etaBin = (int) ( (eta-5.)* 0.5 * oneOverDeta );
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
	    G4String namePhys = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
	    G4String nameLog  = a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();

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
	    etaBin = (int) ( eta * oneOverDeta );
            if ( etaBin > 49 ) etaBin = 49;
	    phiBin = (int) ( phi * oneOverDphi );
            if (phiBin>phiBinMax) phiBin=phiBinMax;
#ifdef DEBUG_DMXYZ
            G4double energy = std::accumulate(m_energies.begin(),m_energies.end(), 0.);
            if(energy > 1e-15) LArG4::CalibrationDefaultCalculator::Print("UNEXP DefaultCalculator",m_identifier,a_step,m_energies);
#endif
	  }

	// Create the LArG4Identifier.
       	m_identifier << detector
		     << subdet
		     << type
		     << sampling
		     << region
		     << etaBin
		     << phiBin;
      }
  
#ifdef DEBUG_HITS
    G4double energy = accumulate(m_energies.begin(),m_energies.end(),0.);
    std::cout << "LArG4::CalibrationDefaultCalculator::Process"
	      << " ID=" << std::string(m_identifier)
	      << " energy=" << energy
	      << " energies=(" << m_energies[0]
	      << "," << m_energies[1]
	      << "," << m_energies[2]
	      << "," << m_energies[3] << ")"
	      << std::endl;
#endif
#ifdef DEBUG_DMXYZ
//    LArG4::CalibrationDefaultCalculator::Print("DMXYZ DefaultCalculator",m_identifier,a_step,m_energies);
#endif

    // Check for bad result.
    if ( m_identifier == LArG4Identifier() )
      return false;

    return true;
  }

} // namespace LArG4
