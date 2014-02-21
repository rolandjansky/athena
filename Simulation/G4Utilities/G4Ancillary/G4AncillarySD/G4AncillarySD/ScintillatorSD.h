/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTILLATORSD_H
#define SCINTILLATORSD_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include <string>

// G4 needed classes
class G4Step;
class G4HCofThisEvent;
class G4LogicalVolume;

class StoreGateSvc;

#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "G4AncillarySimEvent/ScintillatorHitCollection.h"

#include <map>

typedef std::map<G4LogicalVolume *, ScintillatorHitCollection * *> volToHCMap;
typedef std::map<std::string, ScintillatorHitCollection *> HCMap;
typedef HCMap::const_iterator HCMapConstIter;
typedef HCMap::iterator HCMapIter;

class ScintillatorSD : public FADS::FadsSensitiveDetector {

 public:

  ScintillatorSD( G4String );
  ~ScintillatorSD();
  void Initialize( G4HCofThisEvent * );
  G4bool ProcessHits( G4Step *, G4TouchableHistory * );
  void EndOfEvent( G4HCofThisEvent * );
  //To be used by the xml & python handlers:
  void addVolume( G4LogicalVolume * logVol , std::string hitCollectionName, bool writeHitsWithZeroEnergy );
  static void addVolumeDelayed( G4LogicalVolume * logVol , std::string hitCollectionName, bool writeHitsWithZeroEnergy );

private:
  HCMap m_HCMap;
  volToHCMap m_volToHCMap;
  std::map<G4LogicalVolume *, bool> m_storeHitsWithZeroEnergy;

  StoreGateSvc * m_sgSvc;
  AthenaHitsCollectionHelper m_hitCollHelp;

  static bool sm_createdBefore;

  static bool sm_sourceIsDelayed;
  static std::vector<G4LogicalVolume *> sm_delayed_logVols;
  static std::vector<std::string> sm_delayed_hitCollectionNames;
  static std::vector<bool> sm_delayed_writeHitsWithZeroEnergies;

};

#endif
