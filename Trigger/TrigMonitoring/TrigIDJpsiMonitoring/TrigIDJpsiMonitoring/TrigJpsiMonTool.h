// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//edits P.Wijeratne (paw@hep.ucl.ac.uk)

#ifndef TRIGIDJPSIMONITORING_TRIGJPSIMONTOOL_H
#define TRIGIDJPSIMONITORING_TRIGJPSIMONTOOL_H

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDDirectory.h"
#include "TrigInDetAnalysis/Efficiency.h"

#include "TrigInDetAnalysisUtils/Filter_Track.h"
#include "TrigInDetAnalysisUtils/Filter_RoiSelector.h"
#include "TrigInDetAnalysisUtils/T_AnalysisConfig.h"
#include "TrigInDetAnalysisUtils/Associator_BestMatch.h"
//#include "TrigInDetAnalysisUtils/TrackMatchDeltaR.h"
//#include "TrigInDetAnalysisUtils/TrackMatchDeltaRCosmic.h"

#include "TrigIDJpsiMonitoring/JpsiAnalysis.h"
#include "TrigIDJpsiMonitoring/JpsiEfficiency.h"

#include "TH1F.h"

class TrigJpsiMonTool : 
  public IHLTMonTool {  
  
public:


  TrigJpsiMonTool( const std::string & type, const std::string & name, const IInterface* parent);

  virtual ~TrigJpsiMonTool();
  
  StatusCode init();

  #ifdef ManagedMonitorToolBase_Uses_API_201401
    StatusCode book();
    StatusCode proc();
  #else
    StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); 
    StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
  #endif

  StatusCode fill();

  // histogram registration

  virtual void addHistogram( TH1* h , std::string MonGroup) { 
    IHLTMonTool::addHistogram( h, MonGroup );
  }

  void setprint(bool p) { m_print=p; }
  
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
  double  m_pTCutOffline;
  double m_etaCutOffline;
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
  std::vector<std::string> m_collectionNames;

  std::string m_chain0;
  std::string m_chain1;
  std::string m_chain2;
  std::string m_chain3;

  std::string m_collection0;
  std::string m_collection1;
  std::string m_collection2;
  std::string m_collection3;
  std::string m_collection4;
  std::string m_collection5;
  std::string m_collection6;
  std::string m_collection7;

  bool m_buildNtuple;
  bool m_mcTruth;

  private :

  /// number of reconstructed tracks
  //int Nreco;

  /// flag to print out the matched tracks etc
  bool m_print;

  //  TIDDirectory* m_dir;

  ///////////////////to stop multiple books
  bool m_countBook;

};


#endif // TRIGIDJPSIMONITORING_TRIGJPSIMONTOOL_H
