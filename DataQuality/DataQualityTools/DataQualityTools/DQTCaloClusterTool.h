/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTCaloClusterTool.h
// PACKAGE:  DataQualityTools
//
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            and Max Baak (mbaakcern.ch)
//
//	     
//
// ********************************************************************
#ifndef DQTCaloClusterTool_H
#define DQTCaloClusterTool_H

#include <set>
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
//REL19 
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
//REL18 #include <ITrackToCalo/IExtrapolTrackToCaloTool.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"
#include "TMath.h"
#include <string>
////#include "TrigDecision/TrigDecisionTool.h"

class TProfile;

class DQTCaloClusterTool: public DataQualityFatherMonTool
{

 public:
  
  DQTCaloClusterTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTCaloClusterTool();

  //StatusCode initialize();
    
  StatusCode bookHistograms( );
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode fillHistograms();
  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  StatusCode checkHists(bool fromFinalize);

  bool bookDQTCaloCluster();

private:


      TH2F_LW *m_eta_highestEtGM;
      TH2F_LW *m_phi_highestEtGM;
      TH2F_LW *m_eta_highestEtCombined_v_L1GM;
      TH2F_LW *m_phi_highestEtCombined_v_L1GM;
      TH2F_LW *m_et_highestEtCombined_v_L1GM;
      TH2F_LW *m_phi_metGM;
      TH2F_LW *m_phi_trt_v_caloGM;
      TH2F_LW *m_phi_trt_v_emGM;
      TH2F_LW *m_phi_trt_v_hadGM;


      TH1F_LW *m_deta_highestEtGM;
      TH1F_LW *m_dphi_highestEtGM;
      TH1F_LW *m_deta_highestEtCombined_v_L1GM;
      TH1F_LW *m_dphi_highestEtCombined_v_L1GM;
      TH1F_LW *m_det_highestEtCombined_v_L1GM;
      TH1F_LW *m_dphi_metGM;
      TH1F_LW *m_dphi_trt_v_caloGM;
      TH1F_LW *m_dphi_trt_v_emGM;
      TH1F_LW *m_dphi_trt_v_hadGM;

      TH1F_LW *m_eOverpGM;
      TH1F_LW *m_dRTrackDQTCaloClusterGM;

      //REL19 
      ToolHandle<IExtrapolateToCaloTool> m_trackToCalo;
      //REL18 ToolHandle<IExtrapolTrackToCaloTool> m_trackToCalo;

      std::string m_EMClusterName;
      std::string m_HadClusterName;
      std::string m_DQTCaloClusterName;
      std::string m_METTopoName;
      std::string m_METName;
      std::string m_trackIDName;
      Float_t m_dRTrackCalo;
      Float_t m_TrackIsoCut;
      Float_t m_TrackIsoCone;
     
      // create tool handle for TrigDecisionTool
/////      ToolHandle<TrigDec::TrigDecisionTool> m_trigDec;

      // so we only print out one error message per job
      bool printedErrorL1Item;
      bool printedErrorEMCluster;
      bool printedErrorDQTCaloCluster;
      bool printedErrorHadCluster;
      bool printedErrorMetTopo;
      bool printedErrorMet;
      bool printedErrorROI;
      bool printedErrorTrackID;
      bool printedErrorTrackFound;
   
};

#endif
