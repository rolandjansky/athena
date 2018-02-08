/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// H8CalibrationDefaultCalculator
// prepared 22 s ep  2004 by G.Unal

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
//#define DEBUG_HITS

#undef DEBUG_VOLUMES

#include "H8CalibrationDefaultCalculator.h"
#include "LArG4Code/LArG4Identifier.h"

#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4LogicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "globals.hh"

#include <cmath>
#include <string>
#include <climits>
#include <algorithm>
#include <set>

H8CalibrationDefaultCalculator::H8CalibrationDefaultCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator)
  // , m_geometry("LArBarrelGeometry", name)
{
}

// StatusCode H8CalibrationDefaultCalculator::initialize()
// {
//   // to check identifiers
//   m_geometry = LArG4::Barrel::Geometry::GetInstance();
//   return StatusCode::SUCCESS;
// }


H8CalibrationDefaultCalculator::~H8CalibrationDefaultCalculator()
{
}


G4bool H8CalibrationDefaultCalculator::Process( const G4Step* a_step, LArG4Identifier & identifier,
                                                std::vector<G4double> & energies,
                                                const LArG4::eCalculatorProcessing a_process ) const
{
  // Use the calculators to determine the energies and the
  // identifier associated with this G4Step.  Note that the
  // default is to process both the energy and the ID.

  energies.clear();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy )
    {
      m_energyCalculator.Energies( a_step, energies );
    }
  else
    for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


  identifier.clear();
  G4String volName = "";
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyID )
    {

      G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      G4VPhysicalVolume* vol = pre_step_point->GetPhysicalVolume();
      volName = vol->GetName();

      G4int detector = 10;        // calorimeter "dead" materials

      // Initialize identifier variables with (invalid) default
      // values (INT_MIN is defined in <climits>).
      G4int subdet   = 4;                 // LAr
      //GU 26-Sep-05, type=3 does not work with offline calodm identifier
      //	G4int type     = 3;                 // test beam specific
      G4int type     = 1;
      G4int sampling = INT_MIN;
      G4int region   = 0;
      G4int etaBin   = 0;
      G4int phiBin   = 0;

      G4double x_table=5733.*CLHEP::mm;  //FIXME

      G4ThreeVector startPoint = pre_step_point->GetPosition();

      // directly in world volume, use x position to decide before/after calorimeters
      if (volName=="CTB::CTB")  {
        if (startPoint.x() < x_table) sampling=0;
        else
          {
            //GU 26-Sep-05, to work for leakage after the calorimeter
            sampling=3;
            subdet=5;
          }
        // find detector envelop in which we are
      } else {
        const G4NavigationHistory* g4navigation = pre_step_point->GetTouchable()->GetHistory();
        G4int ndep = g4navigation->GetDepth();
        for (G4int ii=0;ii<=ndep;ii++) {
          G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
          G4String vName = v1->GetName();
          //GU 10dec04  change in envelopes for tracker in ctb setup : IDET=mother envelope
          if ( vName=="IDET::IDET") {
            sampling=0;
            break;
          }
          if ( vName=="CALO::CALO") {
            // compute local radius in CALO::CALO frame (<=> atlas frame for calorimeter)
            const G4AffineTransform transformation = g4navigation->GetTransform(ii);
            G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
            double rloc = sqrt(startPointinLocal.x()*startPointinLocal.x() +
                               startPointinLocal.y()*startPointinLocal.y());
            double etaloc = fabs(startPointinLocal.pseudoRapidity());
            double philoc = startPointinLocal.phi()+M_PI/16.;
            if (philoc<0. ) { philoc+= 2.*M_PI; }
            //static double rend_lar  =  1960.;
            const static double rbeg_lar  =  1500.;
            const static double rend_tile =  3835.;
            const static double rps= 0.5*(1410.+1447.);
            if (rloc<rbeg_lar) {
              sampling=1;
              if (rloc<rps) region=2;
              else region=3;
            }
            else if (rloc<rend_tile) {
              sampling=2;
              region=0;
            }
            else {
              subdet=5;
              sampling=3;
              region=0;
            }
            if (sampling==2 && etaloc>1.0) {
              region=2;
              etaBin = (int) ((etaloc-1.0)/0.1);
              if (etaBin>4) etaBin=4;
            } else {
              etaBin = (int) (etaloc/0.1);
              if (sampling==2 && etaBin>10) etaBin=10;
              if (sampling==1 && etaBin>14) etaBin=14;
              if (sampling==3 && etaBin>16) etaBin=16;
            }
            phiBin = (int) (philoc*32./M_PI);
            if (phiBin>63) phiBin=63;
            break;
          }
          if ( vName=="MUON::MUON") {
            subdet=5;
            sampling=3;
            break;
          }
        }
      }
      if (sampling==INT_MIN)  {

        //G0 10 dec04  to recover anciliary detectors
        G4ThreeVector startPoint = pre_step_point->GetPosition();
        if (startPoint.x() < x_table) sampling=0;
        else
          {
            sampling=3;
            subdet=5;
          }
      }

#ifdef DEBUG_HITS
      double rr=sqrt(startPoint.y()*startPoint.y()+startPoint.z()*startPoint.z());
      double rloc=0.;
      const G4NavigationHistory* g4navigation = pre_step_point->GetTouchable()->GetHistory();
      G4int ndep = g4navigation->GetDepth();
      for (G4int ii=0;ii<=ndep;ii++) {
        G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
        G4String vName = v1->GetName();
        if ( vName=="CALO::CALO") {
          // compute local radius in CALO::CALO frame (<=> atlas frame for calorimeter)
          const G4AffineTransform transformation = g4navigation->GetTransform(ii);
          G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
          rloc = sqrt(startPointinLocal.x()*startPointinLocal.x() +
                      startPointinLocal.y()*startPointinLocal.y());
          break;
        }
      }

      if (subdet==4 && type==1 && sampling==0)
        std::cout << "H8HitPos D4T1S0 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==1 && region==0)
        std::cout << "H8HitPos D4T1S1R0 " <<  startPoint.x() << " " << rr  << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==1 && region==1)
        std::cout << "H8HitPos D4T1S1R1 " <<  startPoint.x()  << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==1 && region==2)
        std::cout << "H8HitPos D4T1S1R2 " <<  startPoint.x()  << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==1 && region==3)
        std::cout << "H8HitPos D4T1S1R3 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==1 && region>3)
        std::cout << "H8HitPos D4T1S1R4 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==2 && region==0)
        std::cout << "H8HitPos D4T1S2R0 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==2 && region==1)
        std::cout << "H8HitPos D4T1S2R1 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==2 && region==2)
        std::cout << "H8HitPos D4T1S2R2 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==2 && region>=3)
        std::cout << "H8HitPos D4T1S2R3 " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==1 && sampling==3 )
        std::cout << "H8HitPos D4T1S3   " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==4 && type==2  )
        std::cout << "H8HitPos D4T2     " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
      if (subdet==5)
        std::cout << "H8HitPos D5       " <<  startPoint.x() << " " << rr << " " << rloc << std::endl;
#endif

      // Create the LArG4Identifier.
      identifier << detector
                 << subdet
                 << type
                 << sampling
                 << region
                 << etaBin
                 << phiBin;
      //        if (subdet==4) {
      //        if (!m_geometry->CheckDMIdentifier(type,sampling,region,etaBin,phiBin))
      //        {
      //           std::cout << "H8CalibrationDefaultCalculator: invalid DM identifier "
      //             << std::string(identifier) << std::endl;
      //        }
      //        }

    }


#ifdef DEBUG_HITS
  G4double energy = accumulate(energies.begin(),energies.end(),0.);
  std::cout << "H8CalibrationDefaultCalculator::Process"
            << " volName " << volName
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
