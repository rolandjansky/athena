/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTBECHitEventAction_H
#define LArTBECHitEventAction_H

#include "FadsActions/ApplicationAction.h"
class G4Event;
class G4Run;

#include "SimHelpers/AthenaHitsCollectionHelper.h"

class LArHitContainer;

class TBStoredLArHitContainers
{
public:
  TBStoredLArHitContainers() {}
  std::map<std::string, LArHitContainer*> emecHitCollection;
};

CLASS_DEF( TBStoredLArHitContainers , 1239862175, 1 )

class LArTBECHitsEventAction: public FADS::ApplicationAction {
public:
  LArTBECHitsEventAction();
  ~LArTBECHitsEventAction();
  void BeginOfRunAction(const G4Run*);
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
private:
  AthenaHitsCollectionHelper helper;
  TBStoredLArHitContainers* m_storedContainers;

  StoreGateSvc* m_detStore;
  IMessageSvc*  m_msgSvc;
};

#endif
