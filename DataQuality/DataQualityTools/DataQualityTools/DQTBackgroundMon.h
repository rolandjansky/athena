/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTBackgroundMon
// PACKAGE:  DataQualityTools
//
// AUTHORS:  D. Salek <David.Salek@cern.ch>
//           I. Chalupkova <Ina.Chalupkova@cern.ch>
// based on code from A.Canepa and M.Tibbetts
// ********************************************************************

#ifndef DQTBackgroundMon_H
#define DQTBackgroundMon_H

#include <set>
#include <stdint.h>
#include "TH2F.h"
#include "TProfile.h"
#include "LWHists/TH1I_LW.h"

#include "TMath.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <functional>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "TagEvent/RawInfoSummaryForTag.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/MBTSCollisionTime.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODTracking/VertexContainer.h"


#include "DataQualityTools/DataQualityFatherMonTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"



namespace MuonCalib
{
  class IIdToFixedIdTool;
}
namespace Muon{
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
}

class DQTBackgroundMon: public DataQualityFatherMonTool
{

 public:

  DQTBackgroundMon(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTBackgroundMon();

  StatusCode initialize();

  StatusCode bookHistograms( );
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  
  StatusCode fillHistograms();
 
  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  
  StatusCode checkHists(bool fromFinalize);
  
  bool bookDQTBackgroundMon();

  StoreGateSvc*     m_eventStore;
  
private:

  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::MuonIdHelperTool>  m_idHelperTool;
  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;

  bool m_doMuons;
  bool m_doTrigger;
  int m_FilledBG;
  int m_EmptyBG;
  int m_UnpairIsoBG;
  int m_UnpairNonIsoBG;

  unsigned int m_bg;
  bool m_filled;
  bool m_empty;
  bool m_unpairiso;
  bool m_unpairnoniso;

  // All BG hists
  TH1F_LW *m_HistBitSet;
  TH1F_LW *m_HistBitSet_Filled;
  TH1F_LW *m_HistBitSet_Empty;
  TH1F_LW *m_HistBitSet_UnpairIso;
  TH1F_LW *m_HistBitSet_UnpairNonIso;

  TH1F_LW *m_HistPixSP;
  TH1F_LW *m_HistPixSP_UnpairIso;
  TH1F_LW *m_HistPixSP_UnpairNonIso;
  TH1F_LW *m_HistPixSPHuge;
  TH1F_LW *m_HistPixSPHuge_UnpairIso;
  TH1F_LW *m_HistPixSPHuge_UnpairNonIso;

  TH1F_LW *m_HistSctSP;
  TH1F_LW *m_HistSctSP_UnpairIso;
  TH1F_LW *m_HistSctSP_UnpairNonIso;
  TH1F_LW *m_HistSctSPHuge;
  TH1F_LW *m_HistSctSPHuge_UnpairIso;
  TH1F_LW *m_HistSctSPHuge_UnpairNonIso;

  TH1F_LW *m_HistLArTimeDiff;

  TH1F_LW *m_HistMBTSTimeDiff;
  TH1F_LW *m_HistMBTSVetoHits;

  TH1F_LW *m_HistLucidHits;

  TH1F_LW *m_HistThetaCsc;
  TH1F_LW *m_HistThetaCscA;
  TH1F_LW *m_HistThetaCscC;
  TH1F_LW *m_HistThetaMdt;
  TH1F_LW *m_HistTimeSeg0Csc;
  TH1F_LW *m_HistTimeSeg0Mdt;
  TH2F_LW *m_HistXYSeg0;
  TH1F_LW *m_HistClusterEnergy;
  TH2F_LW *m_HistClusterEtaPhi;
  TH2F_LW *m_HistClusterEtaTime;
  TH1F_LW *m_HistFakeJetIndex;
  TH1F_LW *m_HistFakeJet1Pt;
  TH1F_LW *m_HistFakeJet1Eta;
  TH1F_LW *m_HistFakeJet1Phi;
  TH1F_LW *m_HistFakeJet1Time;
  TH1F_LW *m_HistFakeJet1Chf;
  TH2F_LW *m_HistFakeJet1EtaTime;
  TH2F_LW *m_HistFakeJet1EmfChf;
  TH1F_LW *m_HistRateBcidACTwoSided;
  TH1F_LW *m_HistRateBcidCATwoSided;
  TH1F_LW *m_HistRateBcidACTwoSidedOneSided;
  TH1F_LW *m_HistRateBcidCATwoSidedOneSided;

  TH1F_LW *m_HistBadLooserJet1Pt;
  TH1F_LW *m_HistBadLooserJet1Eta;
  TH1F_LW *m_HistBadLooserJet1Phi;
  TH1F_LW *m_HistBadLooserJet1Time;
  TH1F_LW *m_HistBadLooserJet1Chf;
  TH2F_LW *m_HistBadLooserJet1EtaTime;
  TH2F_LW *m_HistBadLooserJet1EmfChf;

  TH1F_LW *m_HistNumVertex;
  TH1F_LW *m_HistNumVertex_UnpairIso;
  TH1F_LW *m_HistNumVertex_UnpairNonIso;
  TH1F_LW *m_HistMetLocHadTopo;

  //cut variables
  int m_LArEC_SideCut;
  int m_MBTS_SideCut;
  int m_MBTS_TimeCut;
  float m_MBTS_ThresholdCut;
  std::string m_mbtsContainerName;
  const uint8_t m_MBTS_mask, m_MBTS_pattern;

  //hist config
  int m_nBins_PixSP;
  int m_nBins_SctSP;
  int m_nBins_SPHuge;
  int m_nBins_TimeDiff;

  float m_Up_PixSP;
  float m_Up_SctSP;
  float m_Up_SPHuge;
  float m_Low_TimeDiff;
  float m_Up_TimeDiff;
  int m_nBkgWords;

  // StoreGate keys
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { this, "EventInfoKey", "EventInfo", "" };
  SG::ReadHandleKey<RawInfoSummaryForTag> m_RawInfoSummaryForTagKey
    { "RawInfoSummaryForTag" };
  SG::ReadHandleKey<LArCollisionTime> m_LArCollisionTimeKey
    { "LArCollisionTime" };
  SG::ReadHandleKey<MBTSCollisionTime> m_MBTSCollisionTimeKey
    { "MBTSCollisionTime" };
  SG::ReadHandleKey<TileCellContainer> m_TileCellContainerKey
    { this, "MBTSContainerName", "MBTSContainer", "" };
  SG::ReadHandleKey<LUCID_RawDataContainer> m_LUCID_RawDataContainerKey
    { "Lucid_RawData" };
  SG::ReadHandleKey<Trk::SegmentCollection> m_SegmentCollectionKey
    { "MuonSegments" };
  SG::ReadHandleKey<BeamBackgroundData> m_BeamBackgroundDataKey
    { "BeamBackgroundData" };
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertexContainerKey
    { "PrimaryVertices" };
};

#endif
