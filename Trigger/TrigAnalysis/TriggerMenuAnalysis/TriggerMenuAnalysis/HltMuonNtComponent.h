/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_HLTMUONNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_HLTMUONNTCOMPONENT_H
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

  std::string m_eventInfoName;

  MsgStream& log() const;

private:
  std::vector<RoIData_Muon>* m_muonDataVec;
  std::vector<RoIData_TileMu>* m_tileMuDataVec;

  TrigEventInfo* m_trigEventInfo;
  std::vector<TgcHpt>* m_tgcHptWire;
  std::vector<TgcHpt>* m_tgcHptStrip;
  std::vector<MSMuon>* m_moore;
  std::vector<CBMuon>* m_muid;
  std::vector<TileL2Muon>* m_tileL2Muon;
  std::vector<TileMuon>* m_tileMuon;

  ActiveStoreSvc* m_activeStore;
  TrigAccessTool* m_trigAccessTool;
  TrigMenuEvent* m_event;

  std::vector<std::string> m_muonChains;

};

//for Muon
void setMSMuon(MSMuon& m, const Rec::TrackParticle* p);
void setSAMuon(CBMuon& m, const Rec::TrackParticle* p);
void setCBMuon(CBMuon& m, const Rec::TrackParticle* p);

#endif // TRIGGERMENUANALYSIS_HLTMUONNTCOMPONENT_H
