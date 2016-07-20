/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTProcessingOfBarrelHits.h"

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

TRTProcessingOfBarrelHits::TRTProcessingOfBarrelHits
(TRTSensitiveDetector* pSensitiveDet):
  printMessages(0), //FIXME obsolete?
  barrelIdentifier(0),
  testLocalCoordinatesOfHits(0),
  numberOfStrawLayersA(0),
  numberOfStrawLayersB(0),
  numberOfStrawLayersC(0),
  numberOfStrawsA(0),
  numberOfStrawsB(0),
  numberOfStrawsC(0),
  integralDistributionOfStrawsA(nullptr),
  integralDistributionOfStrawsB(nullptr),
  integralDistributionOfStrawsC(nullptr),
  strawIDToLayerIDA(nullptr),
  strawIDToLayerIDB(nullptr),
  strawIDToLayerIDC(nullptr),
  pParameters(nullptr),
  pSensitiveDetector(pSensitiveDet),
  verboseLevel(pSensitiveDet->verboseLevel)
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages"); //FIXME obsolete?

  if (verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfBarrelHits" << G4endl; }

  Initialize();

  pParameters = nullptr;

  if (verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfBarrelHits done" << G4endl; }
}


// Called by TRTSensitiveDetector::DeleteObjects

TRTProcessingOfBarrelHits::~TRTProcessingOfBarrelHits()
{

  if (verboseLevel>5) { G4cout << "##### Destructor TRTProcessingOfBarrelHits" << G4endl; }

  DeleteArrays();

  if (verboseLevel>5) { G4cout << "##### Destructor TRTProcessingOfBarrelHits done" << G4endl; }

}


// Called by TRTProcessingOfBarrelHits

void TRTProcessingOfBarrelHits::Initialize()
{
  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::Initialize" << G4endl; }


  barrelIdentifier = pParameters->GetInteger("BarrelIdentifier");
  testLocalCoordinatesOfHits =
    pParameters->GetInteger("TestLocalCoordinatesOfHits");

  if (barrelIdentifier == 1)
    {
      strawIDToLayerIDA = nullptr;
      strawIDToLayerIDB = nullptr;
      strawIDToLayerIDC = nullptr;
    }


  if (barrelIdentifier == 2)
    {
      numberOfStrawsA = pParameters->GetInteger("NumberOfStrawsA");
      numberOfStrawsB = pParameters->GetInteger("NumberOfStrawsB");
      numberOfStrawsC = pParameters->GetInteger("NumberOfStrawsC");

      strawIDToLayerIDA = new int[numberOfStrawsA];
      strawIDToLayerIDB = new int[numberOfStrawsB];
      strawIDToLayerIDC = new int[numberOfStrawsC];
    }

  numberOfStrawLayersA = pParameters->GetInteger("NumberOfStrawLayersA");
  numberOfStrawLayersB = pParameters->GetInteger("NumberOfStrawLayersB");
  numberOfStrawLayersC = pParameters->GetInteger("NumberOfStrawLayersC");

  integralDistributionOfStrawsA = new int[numberOfStrawLayersA];
  integralDistributionOfStrawsB = new int[numberOfStrawLayersB];
  integralDistributionOfStrawsC = new int[numberOfStrawLayersC];

  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::Initialize done" << G4endl; }
}


// Called by Geant4

bool TRTProcessingOfBarrelHits::ProcessHit(G4Step* pStep)
{
  G4StepPoint* pPreStepPoint = pStep->GetPreStepPoint();

  G4TouchableHandle pTouchableHandle = pPreStepPoint->GetTouchableHandle();
  int depth = 2;

  int gasID = pTouchableHandle->GetReplicaNumber();
  int strawID = pTouchableHandle->GetReplicaNumber(depth++);
  int ringID = pTouchableHandle->GetReplicaNumber(depth++);
  int moduleID = pTouchableHandle->GetReplicaNumber(depth);

  int hitID = gasID % 2;

  G4Track* pTrack = pStep->GetTrack();
  // get the HepMC barcode using the track helper
  TrackHelper trHelp(pTrack);

  G4ThreeVector globalPreStepPoint = pPreStepPoint->GetPosition();

  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();
  G4ThreeVector globalPostStepPoint = pPostStepPoint->GetPosition();


  G4TouchableHistory* pTouchableHistory =
    (G4TouchableHistory*) pPreStepPoint->GetTouchable();

  const G4AffineTransform& topTransform = pTouchableHistory->GetHistory()->
    GetTopTransform();

  G4ThreeVector localPreStepPoint = topTransform.TransformPoint(globalPreStepPoint);
  G4ThreeVector localPostStepPoint = topTransform.TransformPoint(globalPostStepPoint);

  double preStepZ = localPreStepPoint.z();
  double postStepZ = localPostStepPoint.z();


  double preStepX = localPreStepPoint.x();
  double preStepY = localPreStepPoint.y();

  double postStepX = localPostStepPoint.x();
  double postStepY = localPostStepPoint.y();

  if (testLocalCoordinatesOfHits)
    {
      double preStepR = std::sqrt(preStepX * preStepX + preStepY * preStepY);
      double postStepR = std::sqrt(postStepX * postStepX + postStepY * postStepY);

      if (preStepR > 2.0000001 || postStepR > 2.0000001)
        {
          G4int particleEncoding = pSensitiveDetector->m_particleEncoding;
          G4double kineticEnergy = pSensitiveDetector->m_kineticEnergy;
          G4double energyDeposit = pSensitiveDetector->m_energyDeposit;

          std::cout << "!!!!! Barrel. Error in local coordinates of hits!" << std::endl;
          std::cout << "  barrelID=" << hitID << "  ringID=" << ringID
                    << "  moduleID=" << moduleID << "  strawID=" << strawID
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

  int layerID;

  if (barrelIdentifier == 1)
    layerID = GetLayerID1(ringID, strawID);
  else
    layerID = GetLayerID2(ringID, strawID);

  hitID <<= 20;
  hitID += (ringID << 15);
  hitID += (moduleID << 10);
  hitID += (layerID << 5);
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


// Called by ProcessHit

int TRTProcessingOfBarrelHits::GetLayerID1(const int& ringID,
                                           int& strawID) const
{
  ++strawID;
  int layerID = 15;

  if (ringID == 0)
    {
      if (strawID <= integralDistributionOfStrawsA[layerID])
        {
          layerID -= 8;
          if (strawID > integralDistributionOfStrawsA[layerID])
            layerID += 4;
          else
            layerID -= 4;
        }
      if (strawID > integralDistributionOfStrawsA[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > integralDistributionOfStrawsA[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > integralDistributionOfStrawsA[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= integralDistributionOfStrawsA[layerID - 1];
    }
  else if (ringID == 1)
    {
      if (strawID > integralDistributionOfStrawsB[layerID])
        layerID += 8;
      else
        layerID -= 8;
      if (strawID > integralDistributionOfStrawsB[layerID])
        layerID += 4;
      else
        layerID -= 4;
      if (strawID > integralDistributionOfStrawsB[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > integralDistributionOfStrawsB[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > integralDistributionOfStrawsB[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= integralDistributionOfStrawsB[layerID - 1];
    }
  else
    {
      if (strawID > integralDistributionOfStrawsC[layerID])
        layerID += 8;
      else
        layerID -= 8;
      if (strawID > integralDistributionOfStrawsC[layerID])
        layerID += 4;
      else
        layerID -= 4;
      if (strawID > integralDistributionOfStrawsC[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > integralDistributionOfStrawsC[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > integralDistributionOfStrawsC[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= integralDistributionOfStrawsC[layerID - 1];
    }

  --strawID;
  return layerID;
}


// Called by ProcessHit

int TRTProcessingOfBarrelHits::GetLayerID2(const int& ringID,
                                           int& strawID) const
{
  int layerID;
  if (ringID == 0)
    {
      layerID = strawIDToLayerIDA[strawID];
      if (layerID > 0)
        strawID -= integralDistributionOfStrawsA[layerID - 1];
    }
  else if (ringID == 1)
    {
      layerID = strawIDToLayerIDB[strawID];
      if (layerID > 0)
        strawID -= integralDistributionOfStrawsB[layerID - 1];
    }
  else
    {
      layerID = strawIDToLayerIDC[strawID];
      if (layerID > 0)
        strawID -= integralDistributionOfStrawsC[layerID - 1];
    }

  return layerID;
}


// Called by ~TRTProcessingOfBarrelHits

void TRTProcessingOfBarrelHits::DeleteArrays()
{

  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::DeleteArrays" << G4endl; }

  delete [] integralDistributionOfStrawsA;
  delete [] integralDistributionOfStrawsB;
  delete [] integralDistributionOfStrawsC;

  if (barrelIdentifier == 2)
    {
      delete [] strawIDToLayerIDA;
      delete [] strawIDToLayerIDB;
      delete [] strawIDToLayerIDC;
    }

  if (verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::DeleteArrays done" << G4endl; }
}
