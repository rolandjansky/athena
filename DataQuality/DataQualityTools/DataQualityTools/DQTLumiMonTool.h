/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTLumiMonTool.h
// PACKAGE:  DataQualityTools
//
// AUTHORS:  Zoltan Gecse (zgecse@cern.ch) 
//           Anadi Canepa (acanepa@cern.ch)
//
// ********************************************************************
#ifndef DQTLumiMonTool_H
#define DQTLumiMonTool_H

#include <set>   
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"
#include "TMath.h"
#include "TProfile.h"
#include "TString.h"
#include <string>
#include <vector>

#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/VertexContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetIdentifier/PixelID.h"
#include "xAODEventInfo/EventInfo.h"


class DQTLumiMonTool: public DataQualityFatherMonTool{
 public:  
  DQTLumiMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  ~DQTLumiMonTool();
  StatusCode initialize();
  // StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode bookHistograms( );
  StatusCode fillHistograms();
  bool bookDQTLumiMonTool();

 private:
  bool m_failedBooking;

  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { "EventInfo" };
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertexContainerKey
    { this, "VertexContainerKey", "PrimaryVertices", "" };
  int m_MinNtracksForTight;
  float m_MinTrackWeightForTight;
  SG::ReadHandleKey<InDet::PixelClusterContainer> m_PixelClustersKey
    { this, "PixelClustersKey", "PixelClusters", "" };
  Gaudi::Property<std::string> m_PixelIDKey
    { this, "PixelIDKey", "PixelID", "" };

  TProfile* m_aveMu_vs_LB;

  TProfile* m_nLooseVtx_vs_LB;
  TProfile* m_nTightVtx_vs_LB;
  TProfile* m_nLooseVtx_perMu_vs_LB;
  TProfile* m_nTightVtx_perMu_vs_LB;
  TProfile* m_nLooseVtx_vs_aveMu;
  TProfile* m_nTightVtx_vs_aveMu;

  TProfile* m_nClustersAll_vs_LB;
  TProfile* m_nClustersECA_vs_LB;
  TProfile* m_nClustersECC_vs_LB;
  TProfile* m_nClustersB0_vs_LB;
  TProfile* m_nClustersB1_vs_LB;
  TProfile* m_nClustersB2_vs_LB;
  TProfile* m_nClustersAll_perMu_vs_LB;
  TProfile* m_nClustersECA_perMu_vs_LB;
  TProfile* m_nClustersECC_perMu_vs_LB;
  TProfile* m_nClustersB0_perMu_vs_LB;
  TProfile* m_nClustersB1_perMu_vs_LB;
  TProfile* m_nClustersB2_perMu_vs_LB;
  TProfile* m_nClustersAll_vs_aveMu;
  TProfile* m_nClustersECA_vs_aveMu;
  TProfile* m_nClustersECC_vs_aveMu;
  TProfile* m_nClustersB0_vs_aveMu;
  TProfile* m_nClustersB1_vs_aveMu;
  TProfile* m_nClustersB2_vs_aveMu;

  TProfile* m_nAvgLumi_vs_LB;
  TProfile* m_nAvgIntPerXing_vs_LB;
  TProfile* m_lumiperBCID_vs_LB;
  TProfile* m_lbintperxing_vs_LB;
  TProfile* m_duration_vs_LB;
  TProfile* m_avglivefrac_vs_LB;
  TProfile* m_fracperBCID_vs_LB;
  TProfile* m_lumiweight_VS_LB;
};

#endif
