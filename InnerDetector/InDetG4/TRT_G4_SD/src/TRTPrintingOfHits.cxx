/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "TRTPrintingOfHits.h"

// Athena headers
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "InDetSimEvent/TRTUncompressedHit.h"

// STL headers
#include <fstream>

// Called by TRTSensitiveDetector::InitializeHitProcessing

TRTPrintingOfHits::TRTPrintingOfHits(): m_msg("TRTPrintingOfHits")
{
  const TRTParameters* pParameters = TRTParameters::GetPointer();

  m_printMessages = pParameters->GetInteger("PrintMessages");

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTPrintingOfHits" << endmsg;

  m_mask = 0x0000001F;
  m_shift = 5;

  m_pOutputFile = TRTOutputFile::GetPointer();

  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "##### Constructor TRTPrintingOfHits done" << endmsg;
}


// Called by TRTSensitiveDetector::DeleteObjects

TRTPrintingOfHits::~TRTPrintingOfHits()
{
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTPrintingOfHits" << endmsg;
  if (msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "####### Destructor TRTPrintingOfHits done" << endmsg;
}


// Called by TRTSensitiveDetector::ProcessHits

void TRTPrintingOfHits::PrintUncompressedHit
(TRTUncompressedHit* pUncompressedHit)
{
  std::ofstream& output = m_pOutputFile->GetReference();

  int hitID = pUncompressedHit->GetHitID();
  int trackID = pUncompressedHit->GetTrackID();
  int particleEncoding = pUncompressedHit->GetParticleEncoding();
  float kineticEnergy = pUncompressedHit->GetKineticEnergy();
  float energyDeposit = pUncompressedHit->GetEnergyDeposit();
  float preStepX = pUncompressedHit->GetPreStepX();
  float preStepY = pUncompressedHit->GetPreStepY();
  float preStepZ = pUncompressedHit->GetPreStepZ();
  float postStepX = pUncompressedHit->GetPostStepX();
  float postStepY = pUncompressedHit->GetPostStepY();
  float postStepZ = pUncompressedHit->GetPostStepZ();
  float globalTime = pUncompressedHit->GetGlobalTime();

  if (!(hitID & 0x00200000))
    {
      int trtID, ringID, moduleID, layerID, strawID;

      strawID = hitID & m_mask;
      hitID >>= m_shift;
      layerID = hitID & m_mask;
      hitID >>= m_shift;
      moduleID = hitID & m_mask;
      hitID >>= m_shift;
      ringID = hitID & m_mask;
      trtID = hitID >> m_shift;

      output << "Hit: trtID=" << trtID << "  ringID=" << ringID << "  moduleID="
             << moduleID << "  layerID=" << layerID << "  strawID=" << strawID;
    }
  else
    {
      int trtID, wheelID, planeID, sectorID, strawID;

      strawID = hitID & m_mask;
      hitID >>= m_shift;
      planeID = hitID & m_mask;
      hitID >>= m_shift;
      sectorID = hitID & m_mask;
      hitID >>= m_shift;
      wheelID = hitID & m_mask;
      trtID = hitID >> m_shift;

      output << "Hit: trtID=" << trtID << "  wheelID=" << wheelID
             << "  planeID=" << planeID << "  sectorID=" << sectorID
             << "  strawID=" << strawID;
    }

  output << "  trackID=" << trackID << std::endl;

  output << "  particleEncoding=" << particleEncoding;

  if (kineticEnergy < 0.0001)
    output << "  kineticEnergy=" << kineticEnergy * 1000000. << " eV";
  else if (kineticEnergy < 0.1)
    output << "  kineticEnergy=" << kineticEnergy * 1000. << " keV";
  else if (kineticEnergy >= 100000.)
    output << "  kineticEnergy=" << kineticEnergy / 1000000. << " TeV";
  else if (kineticEnergy >= 100.)
    output << "  kineticEnergy=" << kineticEnergy / 1000. << " GeV";
  else
    output << "  kineticEnergy=" << kineticEnergy << " MeV";

  if (energyDeposit < 0.1)
    output << "  energyDeposit=" << energyDeposit * 1000. << " eV" << std::endl;
  else if (energyDeposit >= 100.)
    output << "  energyDeposit=" << energyDeposit / 1000. << " MeV"
           << std::endl;
  else
    output << "  energyDeposit=" << energyDeposit << " keV" << std::endl;

  output << "  preStepX=" << preStepX << " mm  preStepY=" << preStepY
         << " mm  preStepZ=" << preStepZ << " mm" << std::endl;
  output << "  postStepX=" << postStepX << " mm  postStepY=" << postStepY
         << " mm  postStepZ=" << postStepZ << " mm" << std::endl;
  output << "  globalTime=" << globalTime << " ns" << std::endl;

  output << std::endl;
}
