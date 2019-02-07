/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTMuonIDTrackTool.h
// PACKAGE:  DataQualityTools
//
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            and Max Baak (mbaakcern.ch)
//            Updated by: 
//              Simon Viel (svielcern.ch)
//		Suvayu Ali (sali@cern.ch)
//	     
//
// ********************************************************************
#ifndef DQTMuonIDTrackTool_H
#define DQTMuonIDTrackTool_H

#include <set>
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include <stdint.h>
#include <string>

#include "DataQualityTools/DataQualityFatherMonTool.h"
#include "TH1.h"

class TProfile;

class DQTMuonIDTrackTool: public DataQualityFatherMonTool
{

 public:
  
  DQTMuonIDTrackTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTMuonIDTrackTool();

  StatusCode initialize();
    
  StatusCode bookHistograms( );
  //StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  StatusCode fillHistograms();
  StatusCode procHistograms( );
  //StatusCode procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );
  StatusCode checkHists(bool fromFinalize);

  bool bookMuons();

private:

  TH2F_LW* m_trkEtasGM;
  TH2F_LW* m_trkThetasGM;
  TH2F_LW* m_trkPhi0sGM;
  TH2F_LW* m_trkd0sGM;
  TH2F_LW* m_trkz0sGM;
  TH2F_LW* m_trkpOverQsGM;

  TH1F_LW* m_trkEtaDiff;
  TH1F_LW* m_trkThetaDiff;
  TH1F_LW* m_trkPhi0Diff;
  TH1F_LW* m_trkd0Diff;
  TH1F_LW* m_trkz0Diff;
  TH1F_LW* m_trkpOverQDiff;
  TH1F_LW* m_trkPxDiff;
  TH1F_LW* m_trkPyDiff;
  TH1F_LW* m_trkPzDiff;
  TH1F_LW* m_trkPDiff;
  TH1F_LW* m_trkPtDiff;
  TH1F_LW* m_trkChargeDiff;
  TH1F_LW* m_trkXDiff;
  TH1F_LW* m_trkYDiff;
  TH1F_LW* m_trkZDiff;
  
  TH1F_LW* m_ntrkMS;       
  TH1F_LW* m_ntrkID;       
  TH1F_LW* m_ntrkComb;
  TH1F_LW* m_trkEtaIDTag;	
  TH1F_LW* m_trkPhi0IDTag;  
  TH1F_LW* m_trkEtaMSProbe; 
  TH1F_LW* m_trkPhi0MSProbe;  

  TH1F_LW* m_trkEtaID;
  TH1F_LW* m_trkThetaID;
  TH1F_LW* m_trkPhi0ID;
  TH1F_LW* m_trkd0ID;
  TH1F_LW* m_trkd0IDZOOM;
  TH1F_LW* m_trkz0ID;
  TH1F_LW* m_trkpOverQID;
  TH1F_LW* m_trkPxID;
  TH1F_LW* m_trkPyID;
  TH1F_LW* m_trkPzID;
  TH1F_LW* m_trkPID;
  TH1F_LW* m_trkPtID;
  TH1F_LW* m_trkChargeID;
  TH1F_LW* m_trkXID;
  TH1F_LW* m_trkYID;
  TH1F_LW* m_trkZID;

  TProfile* m_trkd0vsPhi0ID;
  TProfile* m_trkeffvsphi;
  TProfile* m_trkeffvseta;

  // integrated eff (over eta/phi) vs Lumi blk 
  TProfile* m_trkeffvsLumi;
  TProfile* m_fwdetatrkeffvsLumi;
  TProfile* m_bkwdetatrkeffvsLumi;

  // Muon trk efficiency
  TProfile *m_MS2ID_MuontrkeffvsPhi;
  TProfile *m_ID2MS_MuontrkeffvsEta;

  TH1F_LW* m_trkEtaMuon;
  TH1F_LW* m_trkThetaMuon;
  TH1F_LW* m_trkPhi0Muon;
  TH1F_LW* m_trkd0Muon;
  TH1F_LW* m_trkz0Muon;
  TH1F_LW* m_trkpOverQMuon;
  TH1F_LW* m_trkPxMuon;
  TH1F_LW* m_trkPyMuon;
  TH1F_LW* m_trkPzMuon;
  TH1F_LW* m_trkPMuon;
  TH1F_LW* m_trkPtMuon;
  TH1F_LW* m_trkChargeMuon;
  TH1F_LW* m_trkXMuon;
  TH1F_LW* m_trkYMuon;
  TH1F_LW* m_trkZMuon;

  TH1F_LW* m_JPsiMassID;
  TH1F_LW* m_JPsiMassMuon;
  TH1F_LW* m_JPsiMassDiff;
  TH2F_LW* m_JPsiMass2D;
	   
  TH1F_LW* m_UpsilonMassID;
  TH1F_LW* m_UpsilonMassMuon;
  TH1F_LW* m_UpsilonMassDiff;
  TH2F_LW* m_UpsilonMass2D;
	   
  TH1F_LW* m_ZMassID;
  TH1F_LW* m_ZMassMuon;
  TH1F_LW* m_ZMassDiff;
  TH2F_LW* m_ZMass2D;

  TH1F_LW* m_JPsiMassID_broad;
  TH1F_LW* m_UpsilonMassID_broad;

  std::string m_CombinedInDetTracksName;
  std::string m_MooreTracksName;
  std::string m_CombinedTracksName;

  ToolHandle< Trk::IExtrapolator > m_extrapolator;

  Float_t m_z0DiffCutID;
  Float_t m_d0DiffCutID;

  Float_t m_z0DiffCutMuon;
  Float_t m_d0DiffCutMuon;

  Int_t m_nMinSCTHits;
  Float_t m_minPtCut;
  Float_t m_muonPtCut;

  //JPsiCounter  
  TH1F_LW* m_JPsiCounter;

  // so we don't print out endless messages
  bool m_printedErrorID;
  bool m_printedErrorMuon;
  bool m_printedErrorCombined;    
  bool m_printedErrorMuonColl;

  // EventInfo key
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey
    { "EventInfo" };
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey
    { "Muons" };

};

#endif
