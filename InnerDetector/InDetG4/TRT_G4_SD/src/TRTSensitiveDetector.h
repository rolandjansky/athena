/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTSensitiveDetector_h
#define TRT_G4_SD_TRTSensitiveDetector_h

// Base class
#include "G4VSensitiveDetector.hh"

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "StoreGate/WriteHandle.h"

class TRTParameters;
class TRTProcessingOfBarrelHits;
class TRTProcessingOfEndCapHits;
class TRTOutputFile;

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class TRTSensitiveDetector : public G4VSensitiveDetector
{
  friend class TRTProcessingOfBarrelHits;
  friend class TRTProcessingOfEndCapHits;

 public:
  TRTSensitiveDetector(const std::string& name, const std::string& hitCollectionName, int setVerboseLevel=0);
  ~TRTSensitiveDetector() { /* don't own any of the pointers... */ }

  void Initialize(G4HCofThisEvent*) override final;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  /** Called by TRTRunAction::EndOfRunAction ... */
  void DeleteObjects();

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

 private:
  void InitializeHitProcessing();
  //FIXME all class variables should have an "m_" prefix.

  ///Configuration paremeters
  int printMessages; // FIXME not used anywhere?
  int hitsWithZeroEnergyDeposit;
  G4VProcess* phot;
  double energyThreshold;
  double probabilityThreshold;
  double energyDepositCorrection;
  double energyThresholdKr;
  double probabilityThresholdKr;
  double energyDepositCorrectionKr;
  double energyThresholdAr;
  double probabilityThresholdAr;
  double energyDepositCorrectionAr;
  double boundaryZ;

  ///Properties of current TRTUncompressedHit, set by
  ///TRTProcessingOfBarrelHits and TRTProcessingOfEndCapHits friend classes.
  int hitID;
  int trackID; //DC 2-29-04 use it as barcode
  int particleEncoding;
  double kineticEnergy;
  double energyDeposit;
  double energyDepositInKeV;
  double preStepX;
  double preStepY;
  double preStepZ;
  double postStepX;
  double postStepY;
  double postStepZ;
  double globalTime;

  ///Other member variables
  // The hits collection
  SG::WriteHandle<TRTUncompressedHitCollection> m_HitColl; //pUncompressedHitCollection;

  TRTParameters* pParameters;

  TRTProcessingOfBarrelHits* pProcessingOfBarrelHits;
  TRTProcessingOfEndCapHits* pProcessingOfEndCapHits;

  // Gas switches
  G4Material *pMaterialXe;
  G4Material *pMaterialKr;
  G4Material *pMaterialAr;
};

#endif //TRT_G4_SD_TRTSensitiveDetector_h
