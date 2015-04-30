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

//::: EDM includes
#include "xAODEventInfo/EventInfo.h"

//::: Local includes
#include "MuonPtCalibNtupleMaker/ICalibMuonsSelectorTool.h"

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
  ServiceHandle< ITHistSvc >            m_HistSvc;

  //:::
  TH1F* m_CutFlow;
  //:::
  TTree* m_OutputTree;
  //:::
  UInt_t m_RunNumber, m_LumiBlock, m_ChannelNumber;
  ULong64_t m_EvtNumber;
  Float_t m_EventWeight;
  //:::
  Float_t m_Pos_Pt,              m_Neg_Pt;
  Float_t m_Pos_Eta,             m_Neg_Eta;
  Float_t m_Pos_Phi,             m_Neg_Phi;
  Float_t m_Pos_Charge,          m_Neg_Charge;
  Float_t m_Pos_ID_Pt,           m_Neg_ID_Pt;
  Float_t m_Pos_ME_Pt,           m_Neg_ME_Pt;
  Float_t m_Pos_ME_Eta,          m_Neg_ME_Eta;
  Float_t m_Pos_ME_Phi,          m_Neg_ME_Phi;
  Float_t m_Pos_PtCone20,        m_Neg_PtCone20;
  Int_t   m_Pos_Quality,         m_Neg_Quality;
  Int_t   m_Pos_Author,          m_Neg_Author;
  Int_t   m_Pos_EnergyLossType,  m_Neg_EnergyLossType;
  Int_t   m_Pos_PrimarySector,   m_Neg_PrimarySector;
  Int_t   m_Pos_SecondarySector, m_Neg_SecondarySector;
  Float_t m_CB_Mass;
  Float_t m_ID_Mass;
  Float_t m_ME_Mass;
  //:::

  CalibMuonsNtupleMaker();

}; 

#endif
