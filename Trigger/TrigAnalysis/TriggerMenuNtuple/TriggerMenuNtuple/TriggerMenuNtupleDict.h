/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TriggerMenuNtupleDict_h__
#define __TriggerMenuNtupleDict_h__
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
  std::vector<ChainEntry> ddd;
  std::vector<TrigStatus> ff;
  std::vector<FeatureIndex> ii;
  std::map<int,FeatureIndex> ii2;
  std::map<int,std::vector<FeatureIndex> > ii3;
  TMNUtil u;

  std::vector<CombLinks> x0;
  std::vector<MuonRoILinks> x1;
  std::vector<ElectronRoILinks> x2;

  std::vector<RoIData_Muon> a1;
  std::vector<RoIData_Jet> a2;
  std::vector<RoIData_Electron> a3;
  std::vector<RoIData_Tau> a4;
  std::vector<RoIData_MissingET> a5;
  std::vector<RoIData_TileMu> a6;

  TrigEventInfo a7;
  std::vector<TgcHpt> a8;
  std::vector<MSMuon> a9;
  std::vector<CBMuon> a10;
  std::vector<TileL2Muon> a11;
  std::vector<TileMuon> a12;
  std::vector<L2IDTrack> a13;
};

#endif // __TriggerMenuNtupleDict_h__
