/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "TRTSensitiveDetector.h"

// Athena includes
#include "CxxUtils/make_unique.h" // For make unique
#include "TRT_G4Utilities/TRTParameters.hh"
#include "TRT_G4Utilities/TRTOutputFile.hh"
#include "TRTParametersForBarrelHits.h"
#include "TRTParametersForEndCapHits.h"
#include "TRTProcessingOfBarrelHits.h"
#include "TRTProcessingOfEndCapHits.h"

//Geant4 includes
//#include "Randomize.hh"
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
#include "G4NistManager.hh"

//stl includes
#include <cmath>
#include <utility>

TRTSensitiveDetector::TRTSensitiveDetector(const std::string& name, const std::string& hitCollectionName, int setVerboseLevel)
  : G4VSensitiveDetector( name ),
    //Variables properly set during InitializeHitProcessing() method
    hitsWithZeroEnergyDeposit(0), phot(nullptr),
    energyThreshold(0.0), probabilityThreshold(0.0), energyDepositCorrection(0.0),
    energyThresholdKr(0.0), probabilityThresholdKr(0.0), energyDepositCorrectionKr(0.0),
    energyThresholdAr(0.0), probabilityThresholdAr(0.0), energyDepositCorrectionAr(0.0),
    boundaryZ(0.0),
    //End of variables properly set during InitializeHitProcessing() method
    //Properties of current TRTUncompressedHit
    hitID(0), trackID(0), particleEncoding(0), kineticEnergy(0.0),
    energyDeposit(0.0), energyDepositInKeV(0.0), preStepX(0.0),
    preStepY(0.0), preStepZ(0.0), postStepX(0.0), postStepY(0.0),
    postStepZ(0.0), globalTime(0.0),
    //End of Properties of current TRTUncompressedHit
    m_HitColl( hitCollectionName ), pParameters(nullptr),
    pProcessingOfBarrelHits(nullptr), pProcessingOfEndCapHits(nullptr),
    pMaterialXe(nullptr), pMaterialKr(nullptr), pMaterialAr(nullptr)
{
  pParameters = TRTParameters::GetPointer();

  printMessages = pParameters->GetInteger("PrintMessages"); //FIXME not used - remove?
  verboseLevel = setVerboseLevel;

  InitializeHitProcessing();

  pParameters = nullptr;
}


// Called by TRTSensitiveDetector
// Once per run

void TRTSensitiveDetector::InitializeHitProcessing()
{
  if(verboseLevel>4)
    {
      G4cout << "InitializeHitProcessing()" << G4endl;
    }
  hitsWithZeroEnergyDeposit =
    pParameters->GetInteger("HitsWithZeroEnergyDeposit");

  phot = nullptr;

  // Parameters describing flourecense in Xe gas mixture:
  // Units of these numbers in management file are in keV; change to default units

  energyThreshold           = pParameters->GetDouble("EnergyThreshold"  ) * CLHEP::keV;
  energyThresholdKr         = pParameters->GetDouble("EnergyThresholdKr") * CLHEP::keV;
  energyThresholdAr         = pParameters->GetDouble("EnergyThresholdAr") * CLHEP::keV;
  probabilityThreshold      = pParameters->GetDouble("ProbabilityThreshold"  );
  probabilityThresholdKr    = pParameters->GetDouble("ProbabilityThresholdKr");
  probabilityThresholdAr    = pParameters->GetDouble("ProbabilityThresholdAr");
  energyDepositCorrection   = pParameters->GetDouble("EnergyDepositCorrection"  ) * CLHEP::keV;
  energyDepositCorrectionKr = pParameters->GetDouble("EnergyDepositCorrectionKr") * CLHEP::keV;
  energyDepositCorrectionAr = pParameters->GetDouble("EnergyDepositCorrectionAr") * CLHEP::keV;
  if(verboseLevel>9)
    {
      G4cout << "Fluorescence parameters: EnergyThreshold            " << energyThreshold           << G4endl;
      G4cout << "Fluorescence parameters: EnergyThresholdKr          " << energyThresholdKr         << G4endl;
      G4cout << "Fluorescence parameters: EnergyThresholdAr          " << energyThresholdAr         << G4endl;
      G4cout << "Fluorescence parameters: ProbabilityThreshold       " << probabilityThreshold      << G4endl;
      G4cout << "Fluorescence parameters: ProbabilityThresholdKr     " << probabilityThresholdKr    << G4endl;
      G4cout << "Fluorescence parameters: ProbabilityThresholdAr     " << probabilityThresholdAr    << G4endl;
      G4cout << "Fluorescence parameters: EnergyDepositCorrection    " << energyDepositCorrection   << G4endl;
      G4cout << "Fluorescence parameters: EnergyDepositCorrectionKr  " << energyDepositCorrectionKr << G4endl;
      G4cout << "Fluorescence parameters: EnergyDepositCorrectionAr  " << energyDepositCorrectionAr << G4endl;
    }
  boundaryZ = pParameters->GetDouble("LengthOfBarrelVolume") / 2.;

  TRTParametersForBarrelHits* pParametersForBarrelHits = nullptr;
  TRTParametersForEndCapHits* pParametersForEndCapHits = nullptr;

  pProcessingOfBarrelHits = new TRTProcessingOfBarrelHits(this);
  pProcessingOfEndCapHits = new TRTProcessingOfEndCapHits(this);

  pParametersForBarrelHits =
    new TRTParametersForBarrelHits(pProcessingOfBarrelHits);
  pParametersForEndCapHits =
    new TRTParametersForEndCapHits(pProcessingOfEndCapHits);

  delete pParametersForBarrelHits;
  delete pParametersForEndCapHits;

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  pMaterialXe = nist->FindOrBuildMaterial("XeCO2O2");
  if (!pMaterialXe && verboseLevel>4)
    {
      G4cout << "Could not find Xe material (Only OK if no TRT straws are filled with Xenon)" << G4endl;
    }
  pMaterialKr = nist->FindOrBuildMaterial("KrCO2O2");
  if (!pMaterialKr)
    {
      pMaterialKr = nist->FindOrBuildMaterial("trt::KrCO2O2");
      if (!pMaterialKr && verboseLevel>4)
        {
          G4cout << "Could not find Kr material (Only OK if no TRT straws are filled with Krypton)" << G4endl;
        }
    }
  pMaterialAr = nist->FindOrBuildMaterial("ArCO2O2");
  if (!pMaterialAr && verboseLevel>4)
    {
      G4cout << "Could not find Ar material (Only OK if no TRT straws are filled with Argon)" << G4endl;
    }
  if(!pMaterialXe && !pMaterialKr && !pMaterialAr)
    {
      G4ExceptionDescription description;
      description << "InitializeHitProcessing: Could not find Xe, Kr or Ar materials (Not OK!)";
      G4Exception("TRTSensitiveDetector", "NoTRTGasesFound", FatalException, description);
    }

  if(verboseLevel>4)
    {
      G4cout << "InitializeHitProcessing() done" << G4endl;
    }
}


// Called by Geant4
// For each event

void TRTSensitiveDetector::Initialize(G4HCofThisEvent* /*pHCofThisEvent*/)
{
  if(verboseLevel>4)
    {
      G4cout << "Initialize()" << G4endl;
    }

  // The following code etablishes a pointer to the "phot" process.
  // This pointer is used in ProcessHits.
  // For performance reasons we test directly on the pointer address rather
  // than doing the string comparison for each hit in ProcessHits.

  if(!phot) {  // Only for the first event
    const G4ProcessVector* pVec =
      G4Gamma::Definition()->GetProcessManager()->GetProcessList();
    for(G4int ip=0;ip<pVec->entries();ip++)
      {
        if((*pVec)[ip]->GetProcessName()=="phot")
          {
            phot = (*pVec)[ip];
            break;
          }
      }
  }

  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<TRTUncompressedHitCollection>();

  if(verboseLevel>4)
    {
      G4cout << "Initialize() done" << G4endl;
    }
}



// Called by Geant4

bool TRTSensitiveDetector::ProcessHits(G4Step* pStep,
                                       G4TouchableHistory* /*pTouchableHistory*/)
{
  energyDeposit = pStep->GetTotalEnergyDeposit();

  G4Track* pTrack = pStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = pTrack->GetDefinition();

  // Skip particles which deposit no energy
  if ( energyDeposit == 0. && !hitsWithZeroEnergyDeposit )
    {
      if ( pParticleDefinition != G4Geantino::Definition() &&
           pParticleDefinition != G4ChargedGeantino::Definition() )
        {
          return false;
        }
    }

  // Skip electrons which originate from a photoelectric process.
  // These electrons have very low energy and even the PAI model will
  // give the wrong energy deposit.
  // Instead we count for a photon which undergoes a "phot" reaction
  // the full energy as deposited in the interaction point (see below)
  if(pTrack->GetCreatorProcess()==phot)
    {
      return false;
    }

  // Get kinetic energy of depositing particle
  kineticEnergy = pStep->GetPreStepPoint()->GetKineticEnergy();

  // If we are dealing with a photon undergoing a "phot" reaction, count
  // the photon kinetic energy as deposited energy in the point of
  // the reaction

  if ( pParticleDefinition==G4Gamma::GammaDefinition() &&
       pStep->GetPostStepPoint()->GetProcessDefinedStep()==phot )
    {
      energyDeposit = kineticEnergy;
      double current_energyThreshold         = 1E+99;
      double current_probabilityThreshold    = 2.0;
      double current_energyDepositCorrection = 0.0;
      G4Material *pPreStepMaterial = pStep->GetPreStepPoint()->GetMaterial();
      if      (pPreStepMaterial == pMaterialXe)
        {
          current_energyThreshold         = energyThreshold;
          current_probabilityThreshold    = probabilityThreshold;
          current_energyDepositCorrection = energyDepositCorrection;
        }
      else if (pPreStepMaterial == pMaterialKr)
        {
          current_energyThreshold         = energyThresholdKr;
          current_probabilityThreshold    = probabilityThresholdKr;
          current_energyDepositCorrection = energyDepositCorrectionKr;
        }
      else if (pPreStepMaterial == pMaterialAr)
        {
          current_energyThreshold         = energyThresholdAr;
          current_probabilityThreshold    = probabilityThresholdAr;
          current_energyDepositCorrection = energyDepositCorrectionAr;
        }
      else
        {
          G4ExceptionDescription description;
          description << "ProcessHits: Unknown prestep material";
          G4Exception("TRTSensitiveDetector", "UnknownGasFound", FatalException, description);
          return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
        }
      // Correct for flourescence (a la Nevski):
      //  - For some percentage (15%) of photons with
      //    energy above threshold (4.9 keV) reduce deposited energy
      //    by correction (4.0 keV)
      //    (Different numbers for Ar and Kr)
      if ( energyDeposit > current_energyThreshold &&
           CLHEP::RandFlat::shoot() > current_probabilityThreshold )
        {
          energyDeposit -= current_energyDepositCorrection;
        }
    }


  bool trackerHit = false;

  if ( std::fabs(pStep->GetPreStepPoint()->GetPosition().z()) < boundaryZ )
    {
      trackerHit = pProcessingOfBarrelHits->ProcessHit(pStep);
    }
  else
    {
      trackerHit = pProcessingOfEndCapHits->ProcessHit(pStep);
    }

  // Particle code of depositing particle
  int particleEncoding = pParticleDefinition->GetPDGEncoding();

  // TRTUncompressedHit appearently expects energyDeposit in keV.
  // Convert:
  energyDepositInKeV = energyDeposit/CLHEP::keV;

  // Create UncompressedHit

  if (trackerHit)
    {
      // Build the hit straight onto the vector
      m_HitColl->Emplace(hitID, trackID, particleEncoding,
                         (float) kineticEnergy, (float) energyDepositInKeV,
                         (float) preStepX, (float) preStepY, (float) preStepZ,
                         (float) postStepX, (float) postStepY, (float) postStepZ,
                         (float) globalTime );
    }

  return true;
}


// It was called by TRTRunAction::EndOfRunAction ...

void TRTSensitiveDetector::DeleteObjects()
{
  if(verboseLevel>4)
    {
      G4cout << "DeleteObjects()" << G4endl;
    }

  delete pProcessingOfBarrelHits;
  delete pProcessingOfEndCapHits;

  if(verboseLevel>4)
    {
      G4cout << "DeleteObjects() done" << G4endl;
    }
}
