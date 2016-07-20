/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTProcessingOfEndCapHits.h"

// Package headers
#include "TRTSensitiveDetector.h"

// Athena headers
#include "MCTruth/TrackHelper.h"
#include "TRT_G4Utilities/TRTParameters.hh"

// Geant4 headers
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"

// STL headers
#include <cmath>

// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTProcessingOfEndCapHits::TRTProcessingOfEndCapHits
(TRTSensitiveDetector* pSensitiveDet):
  printMessages(0), //FIXME obsolete?
  sectorsABC(0),
  testLocalCoordinatesOfHits(0),
  numberOfStrawsInPlanesAB(0),
  numberOfStrawsInPlaneC(0),
  numberOfWheelsAB(0),
  numberOfStrawsInSectorsAB(0),
  numberOfStrawsInSectorC(0),
  initialStrawIDInSectorsAB(0),
  initialStrawIDInSectorC(0),
  numberOfStrawsInIDSectorsAB(0),
  numberOfStrawsInIDSectorC(0),
  pParameters(nullptr),
  pSensitiveDetector(pSensitiveDet),
  verboseLevel(pSensitiveDet->verboseLevel)
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages"); //FIXME Obsolete?

  if (verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfEndCapHits" << G4endl; }

  Initialize();

  pParameters = nullptr;

  if (verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfEndCapHits done" << G4endl; }
}


// Called by TRTSensitiveDetector::DeleteObjects

TRTProcessingOfEndCapHits::~TRTProcessingOfEndCapHits()
{
}


// Called by TRTProcessingOfEndCapHits

void TRTProcessingOfEndCapHits::Initialize()
{
  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfEndCapHits::Initialize" << G4endl; }

  sectorsABC = pParameters->GetInteger("SectorsABC");

  testLocalCoordinatesOfHits =
    pParameters->GetInteger("TestLocalCoordinatesOfHits");

  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfEndCapHits::Initialize done" << G4endl; }

}


// Called by Geant4

bool TRTProcessingOfEndCapHits::ProcessHit(G4Step* pStep)
{
  G4Track* pTrack = pStep->GetTrack();
  // get the HepMC barcode using the track helper
  TrackHelper trHelp(pTrack);

  G4StepPoint* pPreStepPoint = pStep->GetPreStepPoint();
  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();

  G4TouchableHandle pTouchableHandle = pPreStepPoint->GetTouchableHandle();
  int depth = 1;

  int strawID = pTouchableHandle->GetReplicaNumber(depth++);

  int sectorID = 0;

  if (sectorsABC)
    sectorID = pTouchableHandle->GetReplicaNumber(depth++);

  int planeID = pTouchableHandle->GetReplicaNumber(depth++);

  int wheelID = pTouchableHandle->GetReplicaNumber(depth);

  G4ThreeVector globalPreStepPoint = pPreStepPoint->GetPosition();

  int endCapID;

  if (globalPreStepPoint.z() > 0.)
    endCapID = 0;
  else
    endCapID = 1;

  if (sectorsABC)
    {
      if (wheelID < numberOfWheelsAB)
        {
          strawID += numberOfStrawsInSectorsAB * sectorID -
            initialStrawIDInSectorsAB;
          if (strawID < 0)
            strawID += numberOfStrawsInPlanesAB;
        }
      else
        {
          strawID += numberOfStrawsInSectorC * sectorID - initialStrawIDInSectorC;
          if (strawID < 0)
            strawID += numberOfStrawsInPlaneC;
        }
    }

  G4ThreeVector globalPostStepPoint = pPostStepPoint->GetPosition();

  G4TouchableHistory* pTouchableHistory =
    (G4TouchableHistory*) pPreStepPoint->GetTouchable();

  const G4AffineTransform& topTransform = pTouchableHistory->GetHistory()->
    GetTopTransform();

  G4ThreeVector localPreStepPoint = topTransform.TransformPoint(globalPreStepPoint);
  G4ThreeVector localPostStepPoint = topTransform.TransformPoint(globalPostStepPoint);

  double preStepX = localPreStepPoint.x();
  double preStepY = localPreStepPoint.y();
  double preStepZ = localPreStepPoint.z();
  double postStepX = localPostStepPoint.x();
  double postStepY = localPostStepPoint.y();
  double postStepZ = localPostStepPoint.z();


  if (testLocalCoordinatesOfHits)
    {
      double preStepR = std::sqrt(preStepX * preStepX + preStepY * preStepY);
      double postStepR = std::sqrt(postStepX * postStepX + postStepY * postStepY);

      if (preStepR > 2.0000001 || postStepR > 2.0000001)
        {
          G4int particleEncoding = pSensitiveDetector->m_particleEncoding;
          G4double kineticEnergy = pSensitiveDetector->m_kineticEnergy;
          G4double energyDeposit = pSensitiveDetector->m_energyDeposit;

          std::cout << "!!!!! End-caps. Error in local coordinates of hits!" << std::endl;
          std::cout << "  endCapID=" << endCapID << "  wheelID=" << wheelID
                    << "  planeID=" << planeID << "  strawID=" << strawID
                    << "  trackID=" << trHelp.GetBarcode() << std::endl;
          std::cout << "  particleEncoding=" << particleEncoding;

          if (kineticEnergy < 0.0001)
            std::cout << "  kineticEnergy=" << kineticEnergy * 1000000. << " eV";
          else if (kineticEnergy < 0.1)
            std::cout << "  kineticEnergy=" << kineticEnergy * 1000. << " keV";
          else if (kineticEnergy >= 100.)
            std::cout << "  kineticEnergy=" << kineticEnergy / 1000. << " GeV";
          else if (kineticEnergy >= 100000.)
            std::cout << "  kineticEnergy=" << kineticEnergy / 1000000. << " TeV";
          else
            std::cout << "  kineticEnergy=" << kineticEnergy << " MeV";

          if (energyDeposit < 0.1)
            std::cout << "  energyDeposit=" << energyDeposit * 1000. << " eV"
                      << std::endl;
          else if (energyDeposit >= 100.)
            std::cout << "  energyDeposit=" << energyDeposit / 1000. << " MeV"
                      << std::endl;
          else
            std::cout << "  energyDeposit=" << energyDeposit << " keV" << std::endl;

          std::cout << "  preStepX=" << preStepX << " mm  preStepY=" << preStepY
                    << " mm  preStepR=" << preStepR << " mm" << std::endl;
          std::cout << "  postStepX=" << postStepX << " mm  postStepY=" << postStepY
                    << " mm  postStepR=" << postStepR << " mm" << std::endl << std::endl;
        }
    }

  double preStepGlobalTime = pPreStepPoint->GetGlobalTime();
  double postStepGlobalTime = pPostStepPoint->GetGlobalTime();
  double globalTime = (preStepGlobalTime + postStepGlobalTime) / 2.;


  int numberOfStrawsInPlane;
  int numberOfStrawsInIDSector;

  if (wheelID < numberOfWheelsAB) {
    numberOfStrawsInPlane = numberOfStrawsInPlanesAB;
    numberOfStrawsInIDSector = numberOfStrawsInIDSectorsAB;
  } else {
    numberOfStrawsInPlane = numberOfStrawsInPlaneC;
    numberOfStrawsInIDSector = numberOfStrawsInIDSectorC;
  }

  // Mapping for negative endcap is
  //    nSectors = 32
  //    nStraws = num straws in sector
  //    straw =  straw number within sector
  //    sector -> (nSectors + nSectors/2 - sector - 1) % nSectors
  //    straw  -> nStraws - 1 - straw
  //
  // Since we calculate sector number below from strawID (where
  // strawID is straw number in plane), its equivalent to map strawID
  // first and then calculate sector number and straw number within sector.

  // Mapping straw number in plane for negative endcap
  if (endCapID > 0) {
    strawID = (3*numberOfStrawsInPlane/2 - strawID - 1) % numberOfStrawsInPlane;
  }

  // Calculate sector number and straw number within sector.
  sectorID = strawID / numberOfStrawsInIDSector;
  strawID %= numberOfStrawsInIDSector;


  int hitID;
  if (endCapID == 0)
    hitID = 0x00200000;
  else
    hitID = 0x00300000;

  hitID += (wheelID << 15);
  hitID += (sectorID << 10);
  hitID += (planeID << 5);
  hitID += strawID;

  pSensitiveDetector->m_hitID = hitID;
  pSensitiveDetector->m_partLink = trHelp.GetParticleLink();
  pSensitiveDetector->m_preStepX = preStepX;
  pSensitiveDetector->m_preStepY = preStepY;
  pSensitiveDetector->m_preStepZ = preStepZ;
  pSensitiveDetector->m_postStepX = postStepX;
  pSensitiveDetector->m_postStepY = postStepY;
  pSensitiveDetector->m_postStepZ = postStepZ;
  pSensitiveDetector->m_globalTime = globalTime;

  return true;
}
