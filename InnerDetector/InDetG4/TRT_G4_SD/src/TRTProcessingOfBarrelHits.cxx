/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  m_printMessages(0), //FIXME obsolete?
  m_barrelIdentifier(0),
  m_testLocalCoordinatesOfHits(0),
  m_numberOfStrawLayersA(0),
  m_numberOfStrawLayersB(0),
  m_numberOfStrawLayersC(0),
  m_numberOfStrawsA(0),
  m_numberOfStrawsB(0),
  m_numberOfStrawsC(0),
  m_integralDistributionOfStrawsA(nullptr),
  m_integralDistributionOfStrawsB(nullptr),
  m_integralDistributionOfStrawsC(nullptr),
  m_strawIDToLayerIDA(nullptr),
  m_strawIDToLayerIDB(nullptr),
  m_strawIDToLayerIDC(nullptr),
  m_pParameters(nullptr),
  m_pSensitiveDetector(pSensitiveDet),
  m_verboseLevel(pSensitiveDet->verboseLevel)
{
  m_pParameters = TRTParameters::GetPointer();

  m_printMessages = m_pParameters->GetInteger("PrintMessages"); //FIXME obsolete?

  if (m_verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfBarrelHits" << G4endl; }

  this->Initialize();

  m_pParameters = nullptr;

  if (m_verboseLevel>5) { G4cout << "##### Constructor TRTProcessingOfBarrelHits done" << G4endl; }
}


// Called by TRTSensitiveDetector::DeleteObjects

TRTProcessingOfBarrelHits::~TRTProcessingOfBarrelHits()
{

  if (m_verboseLevel>5) { G4cout << "##### Destructor TRTProcessingOfBarrelHits" << G4endl; }

  this->DeleteArrays();

  if (m_verboseLevel>5) { G4cout << "##### Destructor TRTProcessingOfBarrelHits done" << G4endl; }

}


// Called by TRTProcessingOfBarrelHits

void TRTProcessingOfBarrelHits::Initialize()
{
  if (m_verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::Initialize" << G4endl; }


  m_barrelIdentifier = m_pParameters->GetInteger("BarrelIdentifier");
  m_testLocalCoordinatesOfHits =
    m_pParameters->GetInteger("TestLocalCoordinatesOfHits");

  if (m_barrelIdentifier == 1)
    {
      m_strawIDToLayerIDA = nullptr;
      m_strawIDToLayerIDB = nullptr;
      m_strawIDToLayerIDC = nullptr;
    }


  if (m_barrelIdentifier == 2)
    {
      m_numberOfStrawsA = m_pParameters->GetInteger("NumberOfStrawsA");
      m_numberOfStrawsB = m_pParameters->GetInteger("NumberOfStrawsB");
      m_numberOfStrawsC = m_pParameters->GetInteger("NumberOfStrawsC");

      m_strawIDToLayerIDA = new int[m_numberOfStrawsA];
      m_strawIDToLayerIDB = new int[m_numberOfStrawsB];
      m_strawIDToLayerIDC = new int[m_numberOfStrawsC];
    }

  m_numberOfStrawLayersA = m_pParameters->GetInteger("NumberOfStrawLayersA");
  m_numberOfStrawLayersB = m_pParameters->GetInteger("NumberOfStrawLayersB");
  m_numberOfStrawLayersC = m_pParameters->GetInteger("NumberOfStrawLayersC");

  m_integralDistributionOfStrawsA = new int[m_numberOfStrawLayersA];
  m_integralDistributionOfStrawsB = new int[m_numberOfStrawLayersB];
  m_integralDistributionOfStrawsC = new int[m_numberOfStrawLayersC];

  if (m_verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::Initialize done" << G4endl; }
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


  const G4TouchableHistory* pTouchableHistory =
    dynamic_cast<const G4TouchableHistory*>(pPreStepPoint->GetTouchable());

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

  if (m_testLocalCoordinatesOfHits)
    {
      double preStepR = std::sqrt(preStepX * preStepX + preStepY * preStepY);
      double postStepR = std::sqrt(postStepX * postStepX + postStepY * postStepY);

      if (preStepR > 2.0000001 || postStepR > 2.0000001)
        {
          G4int particleEncoding = m_pSensitiveDetector->m_particleEncoding;
          G4double kineticEnergy = m_pSensitiveDetector->m_kineticEnergy;
          G4double energyDeposit = m_pSensitiveDetector->m_energyDeposit;

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

  if (m_barrelIdentifier == 1)
    layerID = GetLayerID1(ringID, strawID);
  else
    layerID = GetLayerID2(ringID, strawID);

  hitID <<= 20;
  hitID += (ringID << 15);
  hitID += (moduleID << 10);
  hitID += (layerID << 5);
  hitID += strawID;

  m_pSensitiveDetector->m_hitID = hitID;
  m_pSensitiveDetector->m_partLink = trHelp.GetParticleLink();
  m_pSensitiveDetector->m_preStepX = preStepX;
  m_pSensitiveDetector->m_preStepY = preStepY;
  m_pSensitiveDetector->m_preStepZ = preStepZ;
  m_pSensitiveDetector->m_postStepX = postStepX;
  m_pSensitiveDetector->m_postStepY = postStepY;
  m_pSensitiveDetector->m_postStepZ = postStepZ;
  m_pSensitiveDetector->m_globalTime = globalTime;

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
      if (strawID <= m_integralDistributionOfStrawsA[layerID])
        {
          layerID -= 8;
          if (strawID > m_integralDistributionOfStrawsA[layerID])
            layerID += 4;
          else
            layerID -= 4;
        }
      if (strawID > m_integralDistributionOfStrawsA[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > m_integralDistributionOfStrawsA[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > m_integralDistributionOfStrawsA[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsA[layerID - 1];
    }
  else if (ringID == 1)
    {
      if (strawID > m_integralDistributionOfStrawsB[layerID])
        layerID += 8;
      else
        layerID -= 8;
      if (strawID > m_integralDistributionOfStrawsB[layerID])
        layerID += 4;
      else
        layerID -= 4;
      if (strawID > m_integralDistributionOfStrawsB[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > m_integralDistributionOfStrawsB[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > m_integralDistributionOfStrawsB[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsB[layerID - 1];
    }
  else
    {
      if (strawID > m_integralDistributionOfStrawsC[layerID])
        layerID += 8;
      else
        layerID -= 8;
      if (strawID > m_integralDistributionOfStrawsC[layerID])
        layerID += 4;
      else
        layerID -= 4;
      if (strawID > m_integralDistributionOfStrawsC[layerID])
        layerID += 2;
      else
        layerID -= 2;
      if (strawID > m_integralDistributionOfStrawsC[layerID])
        ++layerID;
      else
        --layerID;
      if (strawID > m_integralDistributionOfStrawsC[layerID])
        ++layerID;

      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsC[layerID - 1];
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
      layerID = m_strawIDToLayerIDA[strawID];
      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsA[layerID - 1];
    }
  else if (ringID == 1)
    {
      layerID = m_strawIDToLayerIDB[strawID];
      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsB[layerID - 1];
    }
  else
    {
      layerID = m_strawIDToLayerIDC[strawID];
      if (layerID > 0)
        strawID -= m_integralDistributionOfStrawsC[layerID - 1];
    }

  return layerID;
}


// Called by ~TRTProcessingOfBarrelHits

void TRTProcessingOfBarrelHits::DeleteArrays()
{

  if (m_verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::DeleteArrays" << G4endl; }

  delete [] m_integralDistributionOfStrawsA;
  delete [] m_integralDistributionOfStrawsB;
  delete [] m_integralDistributionOfStrawsC;

  if (m_barrelIdentifier == 2)
    {
      delete [] m_strawIDToLayerIDA;
      delete [] m_strawIDToLayerIDB;
      delete [] m_strawIDToLayerIDC;
    }

  if (m_verboseLevel>5) { G4cout << "######### Method TRTProcessingOfBarrelHits::DeleteArrays done" << G4endl; }
}
