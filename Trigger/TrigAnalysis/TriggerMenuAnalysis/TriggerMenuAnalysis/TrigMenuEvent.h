/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMenuEvent_h__
#define __TrigMenuEvent_h__
/*
  TrigMenuEvent.h
*/
#include <vector>

class HLTObjectsInRoI;


class TrigMenuEvent {
public:
  static TrigMenuEvent* getInstance();

private:
  static TrigMenuEvent* gEvent;
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
  std::vector<HLTObjectsInRoI*> mMuonChainObjects;
  std::vector<HLTObjectsInRoI*> mElectronChainObjects;
  std::vector<HLTObjectsInRoI*> mJetChainObjects;
  std::vector<HLTObjectsInRoI*> mTauChainObjects;
  std::vector<HLTObjectsInRoI*> mMissingETChainObjects;
  std::vector<HLTObjectsInRoI*> mTileMuChainObjects;
};

void TrigMenuEvent::setMuonChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mMuonChainObjects = objs;
}
void TrigMenuEvent::setElectronChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mElectronChainObjects = objs;
}
void TrigMenuEvent::setJetChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mJetChainObjects = objs;
}
void TrigMenuEvent::setTauChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mTauChainObjects = objs;
}
void TrigMenuEvent::setMissingETChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mMissingETChainObjects = objs;
}
void TrigMenuEvent::setTileMuChainObjects(std::vector<HLTObjectsInRoI*>& objs) {
  mTileMuChainObjects = objs;
}



std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getMuonChainObjects() {
  return mMuonChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getMuonChainObjects() const {
  return mMuonChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTileMuChainObjects() {
  return mTileMuChainObjects;
}
const std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTileMuChainObjects() const {
  return mTileMuChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getElectronChainObjects() {
  return mElectronChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getElectronChainObjects() const {
  return mElectronChainObjects;
}


std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getJetChainObjects() {
  return mJetChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getJetChainObjects() const {
  return mJetChainObjects;
}


std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getTauChainObjects() {
  return mTauChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getTauChainObjects() const {
  return mTauChainObjects;
}

std::vector<HLTObjectsInRoI*>& TrigMenuEvent::getMissingETChainObjects() {
  return mMissingETChainObjects;
}
const std::vector<HLTObjectsInRoI*>& 
TrigMenuEvent::getMissingETChainObjects() const {
  return mMissingETChainObjects;
}


#endif // __TrigMenuEvent_h__
