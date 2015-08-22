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

class DQTLumiMonTool: public DataQualityFatherMonTool{
 public:  
  DQTLumiMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  ~DQTLumiMonTool();
  //StatusCode initialize();
  // StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode bookHistograms( );
  StatusCode fillHistograms();
  bool bookDQTLumiMonTool();

 private:
  bool failedBooking;

  std::string m_VertexContainerKey;
  int m_MinNtracksForTight;
  float m_MinTrackWeightForTight;
  std::string m_PixelClustersKey;
  std::string m_PixelIDKey;

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
};

#endif
