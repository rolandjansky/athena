/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGMENUEVENT_H
#define TRIGGERMENUANALYSIS_TRIGMENUEVENT_H
/*
  TrigMenuEvent.h
*/
#include <vector>

class HLTObjectsInRoI;


class TrigMenuEvent {
public:
  static TrigMenuEvent* getInstance();

private:
  static TrigMenuEvent* s_event;
  TrigMenuEvent();

public:
  virtual ~TrigMenuEvent();

  inline void setMuonChainObjects(std::vector<HLTObjectsInRoI*>& objs);
  inline void setElectronChainObjects(std::vector<HLTObjectsInRoI*>& objs);
  inline void setJetChainObjects(std::vector<HLTObjectsInRoI*>& objs);
  inline void setTauChainObjects(std::vector<HLTObjectsInRoI*>& objs);
  inline void setMissingETChainObjects(std::vector<HLTObjectsInRoI*>& objs);
  inline void setTileMuChainObjects(std::vector<HLTObjectsInRoI*>& objs);

  inline std::vector<HLTObjectsInRoI*>& getMuonChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getMuonChainObjects() const;

  inline std::vector<HLTObjectsInRoI*>& getTileMuChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getTileMuChainObjects() const;

  inline std::vector<HLTObjectsInRoI*>& getElectronChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getElectronChainObjects() const;

  inline std::vector<HLTObjectsInRoI*>& getJetChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getJetChainObjects() const;

  inline std::vector<HLTObjectsInRoI*>& getTauChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getTauChainObjects() const;

  inline std::vector<HLTObjectsInRoI*>& getMissingETChainObjects();
  inline const std::vector<HLTObjectsInRoI*>& getMissingETChainObjects() const;

  void clear();

protected:
  void clearHLTObjects(std::vector<HLTObjectsInRoI*>& objs);

protected:
  std::vector<HLTObjectsInRoI*> m_muonChainObjects;
  std::vector<HLTObjectsInRoI*> m_electronChainObjects;
  std::vector<HLTObjectsInRoI*> m_jetChainObjects;
  std::vector<HLTObjectsInRoI*> m_tauChainObjects;
  std::vector<HLTObjectsInRoI*> m_missingETChainObjects;
  std::vector<HLTObjectsInRoI*> m_tileMuChainObjects;
};

void TrigMenuEvent::setMuonChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_muonChainObjects = objs;
}
void TrigMenuEvent::setElectronChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_electronChainObjects = objs;
}
void TrigMenuEvent::setJetChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_jetChainObjects = objs;
}
void TrigMenuEvent::setTauChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_tauChainObjects = objs;
}
void TrigMenuEvent::setMissingETChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_missingETChainObjects = objs;
}
void TrigMenuEvent::setTileMuChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  m_tileMuChainObjects = objs;
}



std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getMuonChainObjects() {
  return m_muonChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getMuonChainObjects() const {
  return m_muonChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTileMuChainObjects() {
  return m_tileMuChainObjects;
}
const std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTileMuChainObjects() const {
  return m_tileMuChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getElectronChainObjects() {
  return m_electronChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getElectronChainObjects() const {
  return m_electronChainObjects;
}


std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getJetChainObjects() {
  return m_jetChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getJetChainObjects() const {
  return m_jetChainObjects;
}


std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTauChainObjects() {
  return m_tauChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getTauChainObjects() const {
  return m_tauChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getMissingETChainObjects() {
  return m_missingETChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getMissingETChainObjects() const {
  return m_missingETChainObjects;
}


#endif // TRIGGERMENUANALYSIS_TRIGMENUEVENT_H
