/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HltMuonNtComponent_h__
#define __HltMuonNtComponent_h__
/*
  HltMuonNtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include "TriggerMenuAnalysis/TrigMenuEvent.h"
#include "TriggerMenuNtuple/TrigEventInfo.h"
#include "TriggerMenuNtuple/TgcHpt.h"
#include "TriggerMenuNtuple/MSMuon.h"
#include "TriggerMenuNtuple/CBMuon.h"
#include "TriggerMenuNtuple/TileL2Muon.h"
#include "TriggerMenuNtuple/TileMuon.h"
#include "TriggerMenuNtuple/RoIData_Muon.h"
#include "TriggerMenuNtuple/RoIData_TileMu.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "StoreGate/ActiveStoreSvc.h"

namespace Rec {
  class TrackParticle;
}

class HltMuonNtComponent : public NtComponent::NtupleComponent {
public:
  HltMuonNtComponent(NtupleAlgorithm* mainAlg, 
		     NtComponent::NtComponentParameters parameters);
  ~HltMuonNtComponent();

  StatusCode book();
  StatusCode fill();

protected:
  void fillHltData();

  void fillEventInfo();
  void fillTgcHpt();
  void fillOfflineMuon();

  void readHLTData();
  void printChainNames(const std::vector<std::string>& v, 
		       const std::string& type);
  void createChainGroups();
  void createChainGroups(const std::vector<std::string>& v);

  std::string mEventInfoName;

  MsgStream& log() const;

private:
  std::vector<RoIData_Muon>* mMuonDataVec;
  std::vector<RoIData_TileMu>* mTileMuDataVec;

  TrigEventInfo* mTrigEventInfo;
  std::vector<TgcHpt>* mTgcHptWire;
  std::vector<TgcHpt>* mTgcHptStrip;
  std::vector<MSMuon>* mMoore;
  std::vector<CBMuon>* mMuid;
  std::vector<TileL2Muon>* mTileL2Muon;
  std::vector<TileMuon>* mTileMuon;

  ActiveStoreSvc* m_activeStore;
  TrigAccessTool* mTrigAccessTool;
  TrigMenuEvent* mEvent;

  std::vector<std::string> mMuonChains;

};

//for Muon
void setMSMuon(MSMuon& m, const Rec::TrackParticle* p);
void setSAMuon(CBMuon& m, const Rec::TrackParticle* p);
void setCBMuon(CBMuon& m, const Rec::TrackParticle* p);

#endif // __HltMuonNtComponent_h__
