/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrigTestMonToolAC.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAEXAMPLE_TRIGTESTMONTOOLAC_H
#define TIDAEXAMPLE_TRIGTESTMONTOOLAC_H


#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaR.h"
#include "TrigInDetAnalysisUtils/TrackMatchDeltaRCosmic.h"

#include "TrigInDetAnalysisExample/SigAnalysis.h"
#include "TrigInDetAnalysisExample/TrackEfficiency.h"


class TrigTestMonToolAC : 
  public IHLTMonTool  {  
  
public:


  TrigTestMonToolAC( const std::string & type, const std::string & name, const IInterface* parent); 
  
  virtual ~TrigTestMonToolAC();
  
  StatusCode init();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool /** newEventsBlock **/, bool /** newLumiBlock **/, bool newRun);
#endif

  StatusCode fill();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool /** endOfEventsBlock **/, bool /** endOfLumiBlock **/, bool endOfRun);
#endif
  
  // histogram registration

  virtual void addHistogram( TH1* h ) { 
    msg(MSG::INFO) << "TrigTestMonToolAC::addHistogram() " << endmsg;
    IHLTMonTool::addHistogram( h ); 
  }

  /// leave this stuff in, as I am currently trying to develop the code, but need this in quickly
  
  //  ReadCondHandleKey<InDet::BeamSpotData>& beamspot_key()       const { return m_beamspot_key; }
  //  ReadCondHandleKey<InDet::BeamSpotData>& beamspot_key_online() const { return m_beamspot_key)_online; }

  ///  Amg::Vector3D&  beamspot() { 
  ///    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamspot_key, ctx };
  ///    Amg::Vector3D beamspot = beamSpotHandle->beamPos();
  ///    ATH_MSG_VERBOSE("Beam spot position " << beamspot );
  ///    return beamspot;
  /// }

  //  Amg::Vector3D&  beamspot_online() { 
  //     SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamspot_key_online, ctx };
  //    Amg::Vector3D beamspot = beamSpotHandle->beamPos();
  //    ATH_MSG_VERBOSE("Beam spot position " << beamspot );
  //    return beamspot;
  // }

protected:

  // track selector cuts

  // test tracks
  double  m_pTCut;
  double  m_etaCut;
  double  m_d0Cut;
  double  m_z0Cut;

  int  m_siHits; // total number of si hits
  int m_pixHits; // pixel hits
  int m_sctHits; // sct hits

  int   m_trtHits; // high threshold hits
  int m_strawHits; // total number of straws
 
  // reference tracks
  double  m_tauEtCutOffline;
  double  m_doTauThreeProng;
  double  m_pTCutOffline;
  double  m_etaCutOffline;
  double  m_d0CutOffline;
  double  m_z0CutOffline;
 
  int  m_siHitsOffline; // total number of si hits
  int m_pixHitsOffline; // pixel hits
  int m_sctHitsOffline; // sct hits

  int   m_trtHitsOffline; // high threshold hits
  int m_strawHitsOffline; // total number of straws

  // roi size 
  double m_phiWidth;
  double m_etaWidth;
  double m_zedWidth;

  // matching parameters
  double m_matchR;   // for DeltaR matcher
  double m_matchPhi; // for DeltaPhi matcher
  
  ToolHandle<Trig::TrigDecisionTool> m_tdt;


  TIDARoiDescriptor m_roiInfo;
  
  std::vector<TrackFilter*>  m_filters;
  std::vector<TrackAssociator*>                 m_associators;

  std::vector<T_AnalysisConfig<IHLTMonTool>*>   m_sequences;

  std::vector<std::string> m_chainNames;
  std::vector<std::string> m_ntupleChainNames;
  std::string              m_releaseMetaData;

  bool m_buildNtuple;
  bool m_mcTruth;
  
  std::string m_analysis_config;
  std::string m_outputFileName;

  bool        m_genericFlag;

  bool        m_initialisePerRun;
  bool        m_firstRun;

  //pdgId
  int m_selectTruthPdgId;

  int m_selectParentTruthPdgId;

  /// kepp events even if they fail the requested trigger chains
  bool  m_keepAllEvents;

  bool m_fileopen;

  bool m_requireDecision;
  
  bool m_containTracks;

  bool m_filter_on_roi;

  bool m_legacy;

  double m_fiducial_radius;

  /// same here - currently trying to get this stuff working, but commented here, so that it will compile and work already ...
  ///  ReadCondHandleKey<InDet::BeamSpotData> m_beamspot_key { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  ///  ReadCondHandleKey<InDet::BeamSpotData> m_beamspot_Key_online { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

};



#endif //  TIDAEXAMPLE_TRIGTESTMONTOOLAC_H
