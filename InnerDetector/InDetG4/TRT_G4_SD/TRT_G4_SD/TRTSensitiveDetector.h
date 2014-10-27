/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTSensitiveDetector_hh
#define TRTSensitiveDetector_hh

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "globals.hh"

#include "AthenaKernel/MsgStreamMember.h"


class TRTParameters;
class TRTProcessingOfBarrelHits;
class TRTProcessingOfEndCapHits;
class TRTOutputFile;
class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;


class TRTSensitiveDetector : public FADS::FadsSensitiveDetector
{
  friend class TRTProcessingOfBarrelHits;
  friend class TRTProcessingOfEndCapHits;

public:
  TRTSensitiveDetector(const std::string);

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  void DeleteObjects();

  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

private:
  void InitializeHitProcessing();
  //FIXME all class variables should have an "m_" prefix.

  ///Configuration paremeters
  int printMessages;
  int hitsWithZeroEnergyDeposit;
  G4VProcess* phot;
  double energyThreshold;
  double probabilityThreshold;
  double energyDepositCorrection;
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
  TRTUncompressedHitCollection* pUncompressedHitCollection;

  TRTParameters* pParameters;

  TRTProcessingOfBarrelHits* pProcessingOfBarrelHits;
  TRTProcessingOfEndCapHits* pProcessingOfEndCapHits;

  AthenaHitsCollectionHelper m_hitCollHelp;

  mutable Athena::MsgStreamMember m_msg;

};

#endif
