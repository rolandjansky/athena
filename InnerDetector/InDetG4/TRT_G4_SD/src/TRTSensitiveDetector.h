/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTSensitiveDetector_h
#define TRT_G4_SD_TRTSensitiveDetector_h

// Base class
#include "G4VSensitiveDetector.hh"

#include "CxxUtils/checker_macros.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "StoreGate/WriteHandle.h"
#include <gtest/gtest_prod.h>

class TRTParameters;
class TRTProcessingOfBarrelHits;
class TRTProcessingOfEndCapHits;

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class TRTSensitiveDetector : public G4VSensitiveDetector
{
 FRIEND_TEST( TRTSensitiveDetectortest, Initialize);
 FRIEND_TEST( TRTSensitiveDetectortest, ProcessHits );
 FRIEND_TEST( TRTSensitiveDetectortest, AddHit );

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
  int m_printMessages; // FIXME not used anywhere?
  int m_hitsWithZeroEnergyDeposit;
  G4VProcess* m_phot;
  double m_energyThreshold;
  double m_probabilityThreshold;
  double m_energyDepositCorrection;
  double m_energyThresholdKr;
  double m_probabilityThresholdKr;
  double m_energyDepositCorrectionKr;
  double m_energyThresholdAr;
  double m_probabilityThresholdAr;
  double m_energyDepositCorrectionAr;
  double m_boundaryZ;

  ///Properties of current TRTUncompressedHit, set by
  ///TRTProcessingOfBarrelHits and TRTProcessingOfEndCapHits friend classes.
  int m_hitID;
  HepMcParticleLink m_partLink;
  int m_particleEncoding;
  double m_kineticEnergy;
  double m_energyDeposit;
  double m_energyDepositInKeV;
  double m_preStepX;
  double m_preStepY;
  double m_preStepZ;
  double m_postStepX;
  double m_postStepY;
  double m_postStepZ;
  double m_globalTime;

  ///Other member variables
  // The hits collection
  SG::WriteHandle<TRTUncompressedHitCollection> m_HitColl; //pUncompressedHitCollection;

  const TRTParameters* m_pParameters;

  TRTProcessingOfBarrelHits* m_pProcessingOfBarrelHits;
  TRTProcessingOfEndCapHits* m_pProcessingOfEndCapHits;

  // Gas switches
  G4Material *m_pMaterialXe;
  G4Material *m_pMaterialKr;
  G4Material *m_pMaterialAr;
};

#endif //TRT_G4_SD_TRTSensitiveDetector_h
