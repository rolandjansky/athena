/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TRT_G4_SD/TRTProcessingOfBarrelHits.h"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4_SD/TRTSensitiveDetector.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TouchableHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4NavigationHistory.hh" 
#include "G4AffineTransform.hh"    
#include "G4ThreeVector.hh"
#include "MCTruth/TrackHelper.h"
#include <cmath>

  // Called by TRTSensitiveDetector::InitializeHitProcessing

TRTProcessingOfBarrelHits::TRTProcessingOfBarrelHits
  (TRTSensitiveDetector* pSensitiveDet): 
    printMessages(0),
    barrelIdentifier(0),
    testLocalCoordinatesOfHits(0),
    numberOfStrawLayersA(0),
    numberOfStrawLayersB(0),
    numberOfStrawLayersC(0),
    numberOfStrawsA(0),
    numberOfStrawsB(0),
    numberOfStrawsC(0),
    integralDistributionOfStrawsA(NULL),
    integralDistributionOfStrawsB(NULL),
    integralDistributionOfStrawsC(NULL),
    strawIDToLayerIDA(NULL),
    strawIDToLayerIDB(NULL),
    strawIDToLayerIDC(NULL),
    pParameters(NULL),
    pSensitiveDetector(pSensitiveDet), 
    m_msg("TRTProcessingOfBarrelHits")
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTProcessingOfBarrelHits" << endreq;

  Initialize();

  pParameters = NULL;

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTProcessingOfBarrelHits done" << endreq;
}


  // Called by TRTSensitiveDetector::DeleteObjects

TRTProcessingOfBarrelHits::~TRTProcessingOfBarrelHits()
{

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTProcessingOfBarrelHits" << endreq;

  DeleteArrays();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Destructor TRTProcessingOfBarrelHits done" << endreq;

}


  // Called by TRTProcessingOfBarrelHits

void TRTProcessingOfBarrelHits::Initialize()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTProcessingOfBarrelHits::Initialize" << endreq;


  barrelIdentifier = pParameters->GetInteger("BarrelIdentifier");
  testLocalCoordinatesOfHits =
    pParameters->GetInteger("TestLocalCoordinatesOfHits");

  if (barrelIdentifier == 1)
  {
    strawIDToLayerIDA = NULL;
    strawIDToLayerIDB = NULL;
    strawIDToLayerIDC = NULL;
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

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTProcessingOfBarrelHits::Initialize done" << endreq;
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
  int trackID = trHelp.GetBarcode();

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
      G4int particleEncoding = pSensitiveDetector->particleEncoding;
      G4double kineticEnergy = pSensitiveDetector->kineticEnergy;
      G4double energyDeposit = pSensitiveDetector->energyDeposit;

      std::cout << "!!!!! Barrel. Error in local coordinates of hits!" << std::endl;
      std::cout << "  barrelID=" << hitID << "  ringID=" << ringID
             << "  moduleID=" << moduleID << "  strawID=" << strawID
             << "  trackID=" << trackID << std::endl;
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

  pSensitiveDetector->hitID = hitID;
  pSensitiveDetector->trackID = trackID;
  pSensitiveDetector->preStepX = preStepX;
  pSensitiveDetector->preStepY = preStepY;
  pSensitiveDetector->preStepZ = preStepZ;
  pSensitiveDetector->postStepX = postStepX;
  pSensitiveDetector->postStepY = postStepY;
  pSensitiveDetector->postStepZ = postStepZ;
  pSensitiveDetector->globalTime = globalTime;

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

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTProcessingOfBarrelHits::DeleteArrays" << endreq;

  delete [] integralDistributionOfStrawsA;
  delete [] integralDistributionOfStrawsB;
  delete [] integralDistributionOfStrawsC;

  if (barrelIdentifier == 2)
  {
    delete [] strawIDToLayerIDA;
    delete [] strawIDToLayerIDB;
    delete [] strawIDToLayerIDC;
  }

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "######### Method TRTProcessingOfBarrelHits::DeleteArrays done" << endreq;
}
