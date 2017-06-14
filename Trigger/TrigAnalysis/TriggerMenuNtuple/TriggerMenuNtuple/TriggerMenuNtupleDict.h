/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_TRIGGERMENUNTUPLEDICT_H
#define TRIGGERMENUNTUPLE_TRIGGERMENUNTUPLEDICT_H
/*
  TriggerMenuNtupleDict.h
*/
#include <vector>

#include "GaudiKernel/IConversionSvc.h"

#include "TriggerMenuNtuple/TrigDecSummary.h"
#include "TriggerMenuNtuple/RoIData.h"
#include "TriggerMenuNtuple/RoIData_Muon.h"
#include "TriggerMenuNtuple/RoIData_Jet.h"
#include "TriggerMenuNtuple/RoIData_Electron.h"
#include "TriggerMenuNtuple/RoIData_Tau.h"
#include "TriggerMenuNtuple/RoIData_MissingET.h"
#include "TriggerMenuNtuple/RoIData_TileMu.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/FeatureIndex.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/CombLinks.h"
#include "TriggerMenuNtuple/MuonRoILinks.h"
#include "TriggerMenuNtuple/ElectronRoILinks.h"
#include "TriggerMenuNtuple/TrigVarInHlt.h"
#include "TriggerMenuNtuple/TMNUtil.h"

#include "TriggerMenuNtuple/TrigEventInfo.h"
#include "TriggerMenuNtuple/TgcHpt.h"
#include "TriggerMenuNtuple/MSMuon.h"
#include "TriggerMenuNtuple/CBMuon.h"
#include "TriggerMenuNtuple/TileL2Muon.h"
#include "TriggerMenuNtuple/TileMuon.h"
#include "TriggerMenuNtuple/TrigStatus.h"
#include "TriggerMenuNtuple/L2IDTrack.h"

class TriggerMenuNtupleDict_dummy {
  std::vector<ChainEntry> m_ddd;
  std::vector<TrigStatus> m_ff;
  std::vector<FeatureIndex> m_ii;
  std::map<int,FeatureIndex> m_ii2;
  std::map<int,std::vector<FeatureIndex> > m_ii3;
  TMNUtil m_u;

  std::vector<CombLinks> m_x0;
  std::vector<MuonRoILinks> m_x1;
  std::vector<ElectronRoILinks> m_x2;

  std::vector<RoIData_Muon> m_a1;
  std::vector<RoIData_Jet> m_a2;
  std::vector<RoIData_Electron> m_a3;
  std::vector<RoIData_Tau> m_a4;
  std::vector<RoIData_MissingET> m_a5;
  std::vector<RoIData_TileMu> m_a6;

  TrigEventInfo m_a7;
  std::vector<TgcHpt> m_a8;
  std::vector<MSMuon> m_a9;
  std::vector<CBMuon> m_a10;
  std::vector<TileL2Muon> m_a11;
  std::vector<TileMuon> m_a12;
  std::vector<L2IDTrack> m_a13;
};

#endif // TRIGGERMENUNTUPLE_TRIGGERMENUNTUPLEDICT_H
