/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// Class header
#include "TRTSensitiveDetector.h"

// Athena includes
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
#include <memory> // For make unique
#include <utility>

TRTSensitiveDetector::TRTSensitiveDetector(const std::string& name, const std::string& hitCollectionName, int setVerboseLevel)
  : G4VSensitiveDetector( name ),
    //Variables properly set during InitializeHitProcessing() method
    m_hitsWithZeroEnergyDeposit(0), m_phot(nullptr),
    m_energyThreshold(0.0),   m_probabilityThreshold(0.0),   m_energyDepositCorrection(0.0),
    m_energyThresholdKr(0.0), m_probabilityThresholdKr(0.0), m_energyDepositCorrectionKr(0.0),
    m_energyThresholdAr(0.0), m_probabilityThresholdAr(0.0), m_energyDepositCorrectionAr(0.0),
    m_boundaryZ(0.0),
    //End of variables properly set during InitializeHitProcessing() method
    //Properties of current TRTUncompressedHit
    m_hitID(0), m_particleEncoding(0), m_kineticEnergy(0.0),
    m_energyDeposit(0.0), m_energyDepositInKeV(0.0), m_preStepX(0.0),
    m_preStepY(0.0), m_preStepZ(0.0), m_postStepX(0.0), m_postStepY(0.0),
    m_postStepZ(0.0), m_globalTime(0.0),
    //End of Properties of current TRTUncompressedHit
    m_HitColl( hitCollectionName ), m_pParameters(nullptr),
    m_pProcessingOfBarrelHits(nullptr), m_pProcessingOfEndCapHits(nullptr),
    m_pMaterialXe(nullptr), m_pMaterialKr(nullptr), m_pMaterialAr(nullptr)
{
  m_pParameters = TRTParameters::GetPointer();

  m_printMessages = m_pParameters->GetInteger("PrintMessages"); //FIXME not used - remove?
  verboseLevel = setVerboseLevel;

  this->InitializeHitProcessing();

  m_pParameters = nullptr;
}


// Called by TRTSensitiveDetector
// Once per run

void TRTSensitiveDetector::InitializeHitProcessing()
{
  if(verboseLevel>4)
    {
      G4cout << GetName() << " InitializeHitProcessing()" << G4endl;
    }
  m_hitsWithZeroEnergyDeposit =
    m_pParameters->GetInteger("HitsWithZeroEnergyDeposit");

  m_phot = nullptr;

  // Parameters describing flourecense in Xe gas mixture:
  // Units of these numbers in management file are in keV; change to default units

  m_energyThreshold           = m_pParameters->GetDouble("EnergyThreshold"  ) * CLHEP::keV;
  m_energyThresholdKr         = m_pParameters->GetDouble("EnergyThresholdKr") * CLHEP::keV;
  m_energyThresholdAr         = m_pParameters->GetDouble("EnergyThresholdAr") * CLHEP::keV;
  m_probabilityThreshold      = m_pParameters->GetDouble("ProbabilityThreshold"  );
  m_probabilityThresholdKr    = m_pParameters->GetDouble("ProbabilityThresholdKr");
  m_probabilityThresholdAr    = m_pParameters->GetDouble("ProbabilityThresholdAr");
  m_energyDepositCorrection   = m_pParameters->GetDouble("EnergyDepositCorrection"  ) * CLHEP::keV;
  m_energyDepositCorrectionKr = m_pParameters->GetDouble("EnergyDepositCorrectionKr") * CLHEP::keV;
  m_energyDepositCorrectionAr = m_pParameters->GetDouble("EnergyDepositCorrectionAr") * CLHEP::keV;
  if(verboseLevel>9)
    {
      G4cout << GetName() << " Fluorescence parameters: EnergyThreshold            "
             << m_energyThreshold           << G4endl;
      G4cout << GetName() << " Fluorescence parameters: EnergyThresholdKr          "
             << m_energyThresholdKr         << G4endl;
      G4cout << GetName() << " Fluorescence parameters: EnergyThresholdAr          "
             << m_energyThresholdAr         << G4endl;
      G4cout << GetName() << " Fluorescence parameters: ProbabilityThreshold       "
             << m_probabilityThreshold      << G4endl;
      G4cout << GetName() << " Fluorescence parameters: ProbabilityThresholdKr     "
             << m_probabilityThresholdKr    << G4endl;
      G4cout << GetName() << " Fluorescence parameters: ProbabilityThresholdAr     "
             << m_probabilityThresholdAr    << G4endl;
      G4cout << GetName() << " Fluorescence parameters: EnergyDepositCorrection    "
             << m_energyDepositCorrection   << G4endl;
      G4cout << GetName() << " Fluorescence parameters: EnergyDepositCorrectionKr  "
             << m_energyDepositCorrectionKr << G4endl;
      G4cout << GetName() << " Fluorescence parameters: EnergyDepositCorrectionAr  "
             << m_energyDepositCorrectionAr << G4endl;
    }
  m_boundaryZ = m_pParameters->GetDouble("LengthOfBarrelVolume") / 2.;

  TRTParametersForBarrelHits* pParametersForBarrelHits = nullptr;
  TRTParametersForEndCapHits* pParametersForEndCapHits = nullptr;

  m_pProcessingOfBarrelHits = new TRTProcessingOfBarrelHits(this);
  m_pProcessingOfEndCapHits = new TRTProcessingOfEndCapHits(this);

  pParametersForBarrelHits =
    new TRTParametersForBarrelHits(m_pProcessingOfBarrelHits);
  pParametersForEndCapHits =
    new TRTParametersForEndCapHits(m_pProcessingOfEndCapHits);

  delete pParametersForBarrelHits;
  delete pParametersForEndCapHits;

  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  m_pMaterialXe = nist->FindOrBuildMaterial("trt::XeCO2O2");
  if (!m_pMaterialXe && verboseLevel>4)
    {
      G4cout << GetName() << " Could not find Xe material (Only OK if no TRT straws are filled with Xenon)" << G4endl;
    }
  m_pMaterialKr = nist->FindOrBuildMaterial("trt::KrCO2O2");
  if (!m_pMaterialKr && verboseLevel>4)
    {
      G4cout << GetName() << " Could not find Kr material (Only OK if no TRT straws are filled with Krypton)" << G4endl;
    }
  m_pMaterialAr = nist->FindOrBuildMaterial("trt::ArCO2O2");
  if (!m_pMaterialAr && verboseLevel>4)
    {
      G4cout << GetName() << " Could not find Ar material (Only OK if no TRT straws are filled with Argon)" << G4endl;
    }
  if(!m_pMaterialXe && !m_pMaterialKr && !m_pMaterialAr)
    {
      G4ExceptionDescription description;
      description << "InitializeHitProcessing: Could not find Xe, Kr or Ar materials (Not OK!)";
      G4Exception("TRTSensitiveDetector", "NoTRTGasesFound", FatalException, description);
    }

  if(verboseLevel>4)
    {
      G4cout << GetName() << " InitializeHitProcessing() done" << G4endl;
    }
}


// Called by Geant4
// For each event

void TRTSensitiveDetector::Initialize(G4HCofThisEvent* /*pHCofThisEvent*/)
{
  if(verboseLevel>4)
    {
      G4cout << GetName() << " Initialize()" << G4endl;
    }

  // The following code etablishes a pointer to the "phot" process.
  // This pointer is used in ProcessHits.
  // For performance reasons we test directly on the pointer address rather
  // than doing the string comparison for each hit in ProcessHits.

  if(!m_phot) {  // Only for the first event
    const G4ProcessVector* pVec =
      G4Gamma::Definition()->GetProcessManager()->GetProcessList();
    for(G4int ip=0;ip<pVec->entries();ip++)
      {
        if((*pVec)[ip]->GetProcessName()=="phot")
          {
            m_phot = (*pVec)[ip];
            break;
          }
      }
  }

  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<TRTUncompressedHitCollection>();

  if(verboseLevel>4)
    {
      G4cout << GetName() << " Initialize() done" << G4endl;
    }
}



// Called by Geant4

bool TRTSensitiveDetector::ProcessHits(G4Step* pStep,
                                       G4TouchableHistory* /*pTouchableHistory*/)
{
  m_energyDeposit = pStep->GetTotalEnergyDeposit();

  G4Track* pTrack = pStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = pTrack->GetDefinition();

  // Skip particles which deposit no energy
  if ( m_energyDeposit == 0. && !m_hitsWithZeroEnergyDeposit )
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
  if(pTrack->GetCreatorProcess()==m_phot)
    {
      return false;
    }

  // Get kinetic energy of depositing particle
  m_kineticEnergy = pStep->GetPreStepPoint()->GetKineticEnergy();

  // If we are dealing with a photon undergoing a "phot" reaction, count
  // the photon kinetic energy as deposited energy in the point of
  // the reaction

  if ( pParticleDefinition==G4Gamma::GammaDefinition() &&
       pStep->GetPostStepPoint()->GetProcessDefinedStep()==m_phot )
    {
      m_energyDeposit = m_kineticEnergy;
      double current_energyThreshold         = 1E+99;
      double current_probabilityThreshold    = 2.0;
      double current_energyDepositCorrection = 0.0;
      G4Material *pPreStepMaterial = pStep->GetPreStepPoint()->GetMaterial();
      if      (pPreStepMaterial == m_pMaterialXe)
        {
          current_energyThreshold         = m_energyThreshold;
          current_probabilityThreshold    = m_probabilityThreshold;
          current_energyDepositCorrection = m_energyDepositCorrection;
        }
      else if (pPreStepMaterial == m_pMaterialKr)
        {
          current_energyThreshold         = m_energyThresholdKr;
          current_probabilityThreshold    = m_probabilityThresholdKr;
          current_energyDepositCorrection = m_energyDepositCorrectionKr;
        }
      else if (pPreStepMaterial == m_pMaterialAr)
        {
          current_energyThreshold         = m_energyThresholdAr;
          current_probabilityThreshold    = m_probabilityThresholdAr;
          current_energyDepositCorrection = m_energyDepositCorrectionAr;
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
      if ( m_energyDeposit > current_energyThreshold &&
           CLHEP::RandFlat::shoot() > current_probabilityThreshold )
        {
          m_energyDeposit -= current_energyDepositCorrection;
        }
    }


  // Particle code of depositing particle
  m_particleEncoding = pParticleDefinition->GetPDGEncoding();

  bool trackerHit = false;

  if ( std::fabs(pStep->GetPreStepPoint()->GetPosition().z()) < m_boundaryZ )
    {
      trackerHit = m_pProcessingOfBarrelHits->ProcessHit(pStep);
    }
  else
    {
      trackerHit = m_pProcessingOfEndCapHits->ProcessHit(pStep);
    }

  // TRTUncompressedHit appearently expects energyDeposit in keV.
  // Convert:
  m_energyDepositInKeV = m_energyDeposit/CLHEP::keV;

  // Create UncompressedHit

  if (trackerHit)
    {
      // Build the hit straight onto the vector
      m_HitColl->Emplace(m_hitID, m_partLink, m_particleEncoding,
                         (float) m_kineticEnergy, (float) m_energyDepositInKeV,
                         (float) m_preStepX, (float) m_preStepY, (float) m_preStepZ,
                         (float) m_postStepX, (float) m_postStepY, (float) m_postStepZ,
                         (float) m_globalTime );
    }

  return true;
}


// It was called by TRTRunAction::EndOfRunAction ...

void TRTSensitiveDetector::DeleteObjects()
{
  if(verboseLevel>4)
    {
      G4cout << GetName() << " DeleteObjects()" << G4endl;
    }

  delete m_pProcessingOfBarrelHits;
  delete m_pProcessingOfEndCapHits;

  if(verboseLevel>4)
    {
      G4cout << GetName() << " DeleteObjects() done" << G4endl;
    }
}
