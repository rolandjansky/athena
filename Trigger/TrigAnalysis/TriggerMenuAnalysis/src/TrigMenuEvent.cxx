/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuEvent.cxx
*/
#include "TriggerMenuAnalysis/TrigMenuEvent.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

using namespace std;


TrigMenuEvent* TrigMenuEvent::gEvent = 0;

TrigMenuEvent* TrigMenuEvent::getInstance() {
  if (gEvent == 0) {
    gEvent = new TrigMenuEvent();
  }
  return gEvent;
}

TrigMenuEvent::TrigMenuEvent() {
}

TrigMenuEvent::~TrigMenuEvent() {
}

void TrigMenuEvent::clear() {
  clearHLTObjects(mMuonChainObjects);
  clearHLTObjects(mTileMuChainObjects);
  clearHLTObjects(mElectronChainObjects);
  clearHLTObjects(mJetChainObjects);
  clearHLTObjects(mTauChainObjects);
  clearHLTObjects(mMissingETChainObjects);
  clearHLTObjects(mTileMuChainObjects);
}

void TrigMenuEvent::clearHLTObjects(std::vector<HLTObjectsInRoI*>& objs) {
  std::vector<HLTObjectsInRoI*>::iterator p;

  for (p=objs.begin(); p!=objs.end(); ++p) {
    if (*p) delete *p;
    *p = 0;
  }
  objs.clear();
}
