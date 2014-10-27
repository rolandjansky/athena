/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_G4_SD/TRTSensitiveDetector.h"
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4_SD/TRTParametersForBarrelHits.h"
#include "TRT_G4_SD/TRTParametersForEndCapHits.h"
#include "TRT_G4_SD/TRTProcessingOfBarrelHits.h"
#include "TRT_G4_SD/TRTProcessingOfEndCapHits.h"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
//Geant4 includes
#include "Randomize.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
//stl includes
#include <fstream>
#include <cmath>
#include <utility>

static FADS::SensitiveDetectorEntryT<TRTSensitiveDetector> sd("TRTSensitiveDetector");


// Called by FADS/Goofy

TRTSensitiveDetector::TRTSensitiveDetector(const std::string name):
  FADS::FadsSensitiveDetector(name),
  //Variables properly set during InitializeHitProcessing() method
  hitsWithZeroEnergyDeposit(0), phot(NULL), energyThreshold(0.0),
  probabilityThreshold(0.0), energyDepositCorrection(0.0), boundaryZ(0.0),
  //End of variables properly set during InitializeHitProcessing() method
  //Properties of current TRTUncompressedHit
  hitID(0), trackID(0), particleEncoding(0), kineticEnergy(0.0),
  energyDeposit(0.0), energyDepositInKeV(0.0), preStepX(0.0),
  preStepY(0.0), preStepZ(0.0), postStepX(0.0), postStepY(0.0),
  postStepZ(0.0), globalTime(0.0),
  //End of Properties of current TRTUncompressedHit
  pUncompressedHitCollection(NULL), pParameters(NULL),
  pProcessingOfBarrelHits(NULL), pProcessingOfEndCapHits(NULL),
  m_msg("TRTSensitiveDetector")
{
  pParameters = TRTParameters::GetPointer();

  ATH_MSG_VERBOSE ( "Constructor" );

  printMessages = pParameters->GetInteger("PrintMessages");

  InitializeHitProcessing();

  pParameters = NULL;

  ATH_MSG_VERBOSE ( "Constructor done" );
}


// Called by TRTSensitiveDetector
// Once per run

void TRTSensitiveDetector::InitializeHitProcessing()
{
  ATH_MSG_VERBOSE ( "InitializeHitProcessing()" );

  hitsWithZeroEnergyDeposit =
    pParameters->GetInteger("HitsWithZeroEnergyDeposit");

  phot = NULL;

  // Parameters describing flourecense in Xe gas mixture:
  // Units of these numbers in management file are in keV; change to default units

  energyThreshold         = pParameters->GetDouble("EnergyThreshold") * CLHEP::keV;
  probabilityThreshold    = pParameters->GetDouble("ProbabilityThreshold");
  energyDepositCorrection = pParameters->GetDouble("EnergyDepositCorrection") * CLHEP::keV;

  boundaryZ = pParameters->GetDouble("LengthOfBarrelVolume") / 2.;

  TRTParametersForBarrelHits* pParametersForBarrelHits = NULL;
  TRTParametersForEndCapHits* pParametersForEndCapHits = NULL;

  pProcessingOfBarrelHits = new TRTProcessingOfBarrelHits(this);
  pProcessingOfEndCapHits = new TRTProcessingOfEndCapHits(this);

  pParametersForBarrelHits =
    new TRTParametersForBarrelHits(pProcessingOfBarrelHits);
  pParametersForEndCapHits =
    new TRTParametersForEndCapHits(pProcessingOfEndCapHits);

  delete pParametersForBarrelHits;
  delete pParametersForEndCapHits;

  ATH_MSG_VERBOSE ( "InitializeHitProcessing() done" );

}


// Called by Geant4
// For each event

void TRTSensitiveDetector::Initialize(G4HCofThisEvent* /*pHCofThisEvent*/)
{

  ATH_MSG_VERBOSE ( "Initialize()" );

  //pUncompressedHitCollection =
  //new TRTUncompressedHitCollection("TRTUncompressedHits");
  pUncompressedHitCollection =
    m_hitCollHelp.RetrieveNonconstCollection<TRTUncompressedHitCollection>("TRTUncompressedHits");

  // The following code etablishes a pointer to the "phot" process.
  // This pointer is used in ProcessHits.
  // For performance reasons we test directly on the pointer address rather
  // than doing the string comparison for each hit in ProcessHits.

  if(!phot) {  // Only for the first event
    const G4ProcessVector* pVec =
      G4Gamma::Definition()->GetProcessManager()->GetProcessList();
    for(G4int ip=0;ip<pVec->entries();ip++) {
      if((*pVec)[ip]->GetProcessName()=="phot") {
        phot = (*pVec)[ip];
        break;
      }
    }
  }

  ATH_MSG_VERBOSE ( "Initialize() done" );
}



// Called by Geant4

bool TRTSensitiveDetector::ProcessHits(G4Step* pStep,
                                       G4TouchableHistory* /*pTouchableHistory*/)
{
  energyDeposit = pStep->GetTotalEnergyDeposit();

  G4Track* pTrack = pStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = pTrack->GetDefinition();

  // Skip particles which deposit no energy
  if ( energyDeposit == 0. && !hitsWithZeroEnergyDeposit ) {
    if ( pParticleDefinition != G4Geantino::Definition() &&
         pParticleDefinition != G4ChargedGeantino::Definition() )
      return false;
  }

  // Skip electrons which originate from a photoelectric process.
  // These electrons have very low energy and even the PAI model will
  // give the wrong energy deposit.
  // Instead we count for a photon which undergoes a "phot" reaction
  // the full energy as deposited in the interaction point (see below)
  if(pTrack->GetCreatorProcess()==phot) {
    return false;
  }

  // Get kinetic energy of depositing particle
  kineticEnergy = pStep->GetPreStepPoint()->GetKineticEnergy();

  // If we are dealing with a photon undergoing a "phot" reaction, count
  // the photon kinetic energy as deposited energy in the point of
  // the reaction

  if ( pParticleDefinition==G4Gamma::GammaDefinition() &&
       pStep->GetPostStepPoint()->GetProcessDefinedStep()==phot ) {
    energyDeposit = kineticEnergy;

    // Correct for flourescence (a la Nevski):
    //  - For some percentage (15%) of photons with
    //    energy above threshold (4.9 keV) reduce deposited energy
    //    by correction (4.0 keV)
    if ( energyDeposit > energyThreshold &&
         CLHEP::RandFlat::shoot() > probabilityThreshold )
      energyDeposit -= energyDepositCorrection;
  }


  bool trackerHit = false;

  if ( std::fabs(pStep->GetPreStepPoint()->GetPosition().z()) < boundaryZ )
    trackerHit = pProcessingOfBarrelHits->ProcessHit(pStep);
  else
    trackerHit = pProcessingOfEndCapHits->ProcessHit(pStep);


  // Particle code of depositing particle
  int particleEncoding = pParticleDefinition->GetPDGEncoding();

  // TRTUncompressedHit appearently expects energyDeposit in keV.
  // Convert:
  energyDepositInKeV = energyDeposit/CLHEP::keV;

  // Create UncompressedHit

  if (trackerHit) {
    // Build the hit straight onto the vector
    pUncompressedHitCollection->Emplace(hitID, trackID, particleEncoding,
                                        (float) kineticEnergy, (float) energyDepositInKeV,
                                        (float) preStepX, (float) preStepY, (float) preStepZ,
                                        (float) postStepX, (float) postStepY, (float) postStepZ,
                                        (float) globalTime );
  }

  return true;
}


// Called by Geant4

void TRTSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*pHCofThisEvent*/)
{
  ATH_MSG_VERBOSE ( "EndOfEvent()" );

  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection<TRTUncompressedHitCollection>(pUncompressedHitCollection);

  //m_hitCollHelp.ExportCollection<TRTUncompressedHitCollection>(pUncompressedHitCollection);

  ATH_MSG_VERBOSE ( "EndOfEvent() done" );

}


// It was called by TRTRunAction::EndOfRunAction ...

void TRTSensitiveDetector::DeleteObjects()
{

  ATH_MSG_VERBOSE ( "DeleteObjects()" );

  delete pProcessingOfBarrelHits;
  delete pProcessingOfEndCapHits;

  ATH_MSG_VERBOSE ( "DeleteObjects() done" );

}
