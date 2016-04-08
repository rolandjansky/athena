/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuEvent.cxx
*/
#include "TriggerMenuAnalysis/TrigMenuEvent.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"

using namespace std;


TrigMenuEvent* TrigMenuEvent::s_event = 0;

TrigMenuEvent* TrigMenuEvent::getInstance() {
  if (s_event == 0) {
    s_event = new TrigMenuEvent();
  }
  return s_event;
}

TrigMenuEvent::TrigMenuEvent() {
}

TrigMenuEvent::~TrigMenuEvent() {
}

void TrigMenuEvent::clear() {
  clearHLTObjects(m_muonChainObjects);
  clearHLTObjects(m_tileMuChainObjects);
  clearHLTObjects(m_electronChainObjects);
  clearHLTObjects(m_jetChainObjects);
  clearHLTObjects(m_tauChainObjects);
  clearHLTObjects(m_missingETChainObjects);
  clearHLTObjects(m_tileMuChainObjects);
}

void TrigMenuEvent::clearHLTObjects(std::vector<HLTObjectsInRoI*>& objs) {
  std::vector<HLTObjectsInRoI*>::iterator p;

  for (p=objs.begin(); p!=objs.end(); ++p) {
    if (*p) delete *p;
    *p = 0;
  }
  objs.clear();
}
