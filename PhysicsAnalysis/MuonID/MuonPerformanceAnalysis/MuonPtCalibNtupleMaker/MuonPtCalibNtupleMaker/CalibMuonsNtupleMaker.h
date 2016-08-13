/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CalibMuonsNtupleMaker_h__
#define __CalibMuonsNtupleMaker_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>
#include <TH1F.h>

//::: Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ITHistSvc.h" 
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"

//::: EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/VertexContainer.h"

//::: Local includes
#include "MuonPtCalibNtupleMaker/ICalibMuonsSelectorTool.h"
#include "MuonPtCalibNtupleMaker/TrackInfo.h"
#include "MuonPtCalibNtupleMaker/HitsInfo.h"
#include "MuonPtCalibNtupleMaker/MuonInfo.h"
#include "MuonPtCalibNtupleMaker/EvtInfo.h"
#include "MuonPtCalibNtupleMaker/TrigInfo.h"

#define MuonMass 0.10565837

class CalibMuonsNtupleMaker : public ::AthAlgorithm { 

public: 

  CalibMuonsNtupleMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CalibMuonsNtupleMaker() {} 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

private: 

  bool                                  m_Extended;
  ToolHandle< ICalibMuonsSelectorTool > m_SelectorTool;
  ToolHandle< Trig::TrigDecisionTool >  m_TrigDecisionTool;
  ToolHandle< Trig::ITrigMuonMatching > m_TrigMuonMatching;
  ServiceHandle< ITHistSvc >            m_HistSvc;
  //:::
  const xAOD::Vertex* m_PrimaryVertex;
  const xAOD::EventInfo* m_EvtInfo;
  //:::
  TH1F* m_CutFlow;
  //:::
  TTree* m_OutputTree;
  //:::
  std::vector< std::string > m_Types;
  //:::
  EvtInfo*  m_Event;
  TrigInfo* m_TrigInfo;
  //:::
  std::map< std::string, TrackInfo* >  m_Combined;
  std::map< std::string, TrackInfo* >  m_InnerDet;
  std::map< std::string, TrackInfo* >  m_MSExtr;
  std::map< std::string, HitsInfo* >   m_Hits;
  std::map< std::string, MuonInfo* >   m_MuonInfo;
  
  CalibMuonsNtupleMaker();

}; 

#endif
