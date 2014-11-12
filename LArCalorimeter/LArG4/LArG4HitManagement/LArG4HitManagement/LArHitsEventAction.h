/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHitEventAction_H
#define LArHitEventAction_H

#include "FadsActions/ApplicationAction.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "globals.hh"
#include <string>

class G4Event;
class G4Run;
class IMessageSvc;
class StoreGateSvc;
class StoredLArHitContainers;
class StoredLArCalibHitContainers;

class LArHitsEventAction: public FADS::ApplicationAction 
{
  friend class LArHitMerger;
  friend class LArCalibHitMerger;

 public:
  LArHitsEventAction();
  ~LArHitsEventAction();
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

 protected:
  StoredLArHitContainers* m_storedContainers;
  StoredLArCalibHitContainers* m_storedCalibContainers;

  StoredLArHitContainers* getStoredContainers() { return m_storedContainers;}
  StoredLArCalibHitContainers* getStoredCalibContainers() { return m_storedCalibContainers;}

 private:
  AthenaHitsCollectionHelper helper;

  StoreGateSvc* m_detStore;
  IMessageSvc*  m_msgSvc;

  // Lar hit time binning type
  std::string m_timeBinType;

  // Width of the time bins for summing hits - for the uniform binning
  G4float m_timeBinWidth;

  bool m_doMiniFcal;
  bool m_allowMods;
};

#endif
