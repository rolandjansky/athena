/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrigTestBase.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:38 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAEXAMPLE_TRIGTESTBASE_H
#define TIDAEXAMPLE_TRIGTESTBASE_H


#include "InDetBeamSpotService/IBeamCondSvc.h"

// #include "AthenaMonitoring/ManagedMonitorToolBase.h"

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
// #include "TrigInDetAnalysisExample/T_AnalysisConfig_Tier0.h"
#include "TrigInDetAnalysisExample/AnalysisConfig_Tier0.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

// class TrigTestPhysValMon : virtual public ManagedMonitorToolBase {
class TrigTestBase : public IHLTMonTool {

  //  typedef T_AnalysisConfig_Tier0<IHLTMonTool> AnalysisConfig_PhysValMon;

public:

  using IHLTMonTool::addHistogram;

public:

  TrigTestBase( const std::string & type, const std::string & name, const IInterface* parent);

  virtual ~TrigTestBase();

  virtual StatusCode init();

#ifdef ManagedMonitorToolBase_Uses_API_201401
  virtual StatusCode book();
  virtual StatusCode fill();
  virtual StatusCode proc();
#else
  virtual StatusCode book( bool newEventsBlock, bool newLumiBlock, bool newRun );
  virtual StatusCode fill();
  virtual StatusCode proc( bool /*newEventsBlock*/, bool /*newLumiBlock*/, bool newRun );
#endif

  // Inheriting class independant histogram registration
  // histogram registration
  virtual void addHistogram( TH1* h ) { 
    ATH_MSG_DEBUG("TIDAMonTool::addHistogram()");
    IHLTMonTool::addHistogram( h ); 
  }


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

  int m_siHitsOffline; // total number of si hits
  int m_pixHitsOffline; // pixel hits
  int m_sctHitsOffline; // sct hits
  int m_blayerHitsOffline;

  int m_pixHolesOffline; // pixel holes
  int m_sctHolesOffline;  // sct holes
  int m_siHolesOffline;   // total pix+sct holes

  int   m_trtHitsOffline; // high threshold hits
  int m_strawHitsOffline; // total number of straws

  // roi size
  //  double m_phiWidth;
  //  double m_etaWidth;
  //  double m_zedWidth;

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

  /// kepp events even if they fail the requested trigger chains
  bool  m_keepAllEvents;

  /// if an ntple file open?
  bool m_fileopen;

  /// is this the first event
  bool m_first; 

  /// use only the highest pt tracks
  bool m_useHighestPT;

  /// if performing the vertex analysis, the index of the 
  /// offline vertex to look for
  int m_vtxIndex;

  /// also run purity analyses
  bool m_runPurity;

  /// determine whether this should be treated as a shifter chain
  bool m_shifter;

  /// max number of shifter chains to use - must be < 2 at the moment
  int m_shifterChains;

  /// additional string for the histogram directory
  std::string  m_sliceTag;

  /// do we want basic, or rigorous roi track containment
  bool         m_containTracks;

};



#endif //  TIDAEXAMPLE_TRIGTESTBASE_H
