/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBCryostatCalibrationCalculator
//  22 sep 2004 G.Unal

// This class calculates the values needed for calibration hits in the
// simulation.

#undef DEBUG_HITS
//#define DEBUG_HITS

#include "LArTBCryostatCalibrationCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"


#include <map>
#include <algorithm>
#include <cmath>

LArTBCryostatCalibrationCalculator::LArTBCryostatCalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator),
    // improvement would be to fetch this from database (for accordion)
    m_radiusPS (0.5*(1385.+1447.)*CLHEP::mm),
    m_rminAcc (1500.24*CLHEP::mm),
    m_rmaxAcc (1960.00*CLHEP::mm)
    // , m_geometry(nullptr)
{
}

// StatusCode LArTBCryostatCalibrationCalculator::initialize()
// {
//   m_geometry = LArG4::Barrel::Geometry::GetInstance();
//   return StatusCode::SUCCESS;
// }


LArTBCryostatCalibrationCalculator::~LArTBCryostatCalibrationCalculator()
{
}


G4bool LArTBCryostatCalibrationCalculator::Process( const G4Step* a_step, LArG4Identifier & identifier,
                                                    std::vector<G4double> & energies,
                                                    const LArG4::eCalculatorProcessing a_process ) const
{
  // Use the calculators to determine the energies and the
  // identifier associated with this G4Step.  Note that the
  // default is to process both the energy and the ID.

  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy )
    {
      m_energyCalculator.Energies( a_step, energies );
    }
  else
    for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );

  identifier.clear();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyID )
    {

      G4StepPoint *thisStepPoint = a_step->GetPreStepPoint();
      const G4NavigationHistory* g4navigation = thisStepPoint->GetTouchable()->GetHistory();
      G4int ndep = g4navigation->GetDepth();
      G4int ind=-1;

      // Now navigate through the volumes hierarchy

      for (G4int ii=0;ii<=ndep;ii++) {
        G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
        if ( (v1->GetName()).find("LAr::TBBarrel::Cryostat::Envelope") != std::string::npos ) {
          ind = ii;
          break;
        }
      }

      if (ind < 0)
        {
          std::cout << "LArTBCryostatCalibrationCalculator:  Not in Cryostat !!!! " << std::endl;
          std::cout << " ndep " << ndep << std::endl;
          for (G4int ii=0;ii<=ndep;ii++) {
            G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
            std::cout << " volname " << v1->GetName() << std::endl;
          }
          return false;
        }

      G4StepPoint *thisStepBackPoint = a_step->GetPostStepPoint();
      G4ThreeVector startPoint = thisStepPoint->GetPosition();
      G4ThreeVector endPoint = thisStepBackPoint->GetPosition();
      const G4AffineTransform transformation = g4navigation->GetTransform(ind);
      G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
      G4ThreeVector   endPointinLocal = transformation.TransformPoint(startPoint);
      G4ThreeVector midinLocal = (startPointinLocal+endPointinLocal)*0.5;
      G4double xZpos   = midinLocal.x();
      G4double yZpos   = midinLocal.y();
      //G4double zZpos   = midinLocal.z();
#ifdef DEBUG_HITS
      std::cout << " LArTBCryostatCalibrationCalculator x,y,z       " <<
        startPoint.x() << " " << startPoint.y() << " " << startPoint.z() << std::endl;
      std::cout << " LArTBCryostatCalibrationCalculator x,y,z local mid" << xZpos <<
        " " << yZpos << " " << zZpos << std::endl;
#endif
      G4double etaZpos = midinLocal.pseudoRapidity();
      G4double phiZpos = midinLocal.phi();
      if(phiZpos<0.) phiZpos = phiZpos + 2.*M_PI;
      G4double radius2Zpos = xZpos*xZpos + yZpos*yZpos;
      G4double radiusZpos = sqrt(radius2Zpos);

      G4int zSide = 1;
      G4int numDeadPhiBins = 64;
      G4double DM1EtaWidth = 0.1 ;
      G4double DM1PhiWidth = 2.*M_PI / numDeadPhiBins ;
      G4int etaBin = (G4int) ( std::fabs(etaZpos) / DM1EtaWidth ) ;
      G4int phiBin = (G4int) (phiZpos/ DM1PhiWidth );
      if (phiBin>63) phiBin=63;
      G4int sampling;
      G4int region;
      G4int type=1;

      if (radiusZpos < m_radiusPS)
        {
          // before PSactive
          sampling=1;
          G4VPhysicalVolume* vol = thisStepPoint->GetPhysicalVolume();
          G4String volName = vol->GetName();
          if (volName.find("LAr::TBBarrel::Cryostat::WarmTube") != std::string::npos) region=0;
          else if (volName.find("LAr::TBBarrel::Cryostat::ColdTube") != std::string::npos ) region=1;
          else region=2;
          if (etaBin>14) etaBin=14;
        }
      else if (radiusZpos < m_rminAcc)
        {
          // between PS and Accordion
          sampling=1;
          region=3;
          if (etaBin>14) etaBin=14;
        }
      else if (radiusZpos < m_rmaxAcc)
        {
          // we will use this for the leakage on the side of the test beam module...
          sampling=2;
          if (fabs(etaZpos)<1.0) {
            region=0;
            etaBin=0;
          } else {
            region=2;
            etaBin=0;
          }
        }
      else
        {
          // between LAr and Tile
          sampling=2;
          region=0;
          if (std::fabs(etaZpos) > 1.0) {
            region=2;
            etaBin = (G4int) ( std::fabs(etaZpos-1.0) / DM1EtaWidth ) ;
            if (etaBin>4) etaBin=4;
          }
        }

      identifier << 10             // LArCalorimeter
                 << zSide * 4      // LArBarrel
                 << type
                 << sampling
                 << region
                 << etaBin
                 << phiBin;

      //   if (!m_geometry->CheckDMIdentifier(type,sampling,region,etaBin,phiBin))
      //   {
      //     std::cout << " LArTBCryostatCalibrationCalculator: Invalid DM identifier "
      //          << std::string(identifier) << std::endl;
      //   }


    } // calculate identifier

#ifdef DEBUG_HITS
  G4double energy = accumulate(energies.begin(),energies.end(),0.);
  std::cout << "LArTBBarrelCryostatCalibrationCalculator::Process"
            << " ID=" << std::string(identifier)
            << " energy=" << energy
            << " energies=(" << energies[0]
            << "," << energies[1]
            << "," << energies[2]
            << "," << energies[3] << ")"
            << std::endl;
#endif

  return true;
}
