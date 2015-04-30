/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CalibMuonsSelectorTool_h__
#define __CalibMuonsSelectorTool_h__

//::: STL includes
#include <string>

//::: Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//::: GRL includes
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"

//::: MCP includes
#include "MuonSelectorTools/IMuonSelectionTool.h"

//::: Local includes
#include "MuonPtCalibNtupleMaker/ICalibMuonsSelectorTool.h"

class CalibMuonsSelectorTool : virtual public ::ICalibMuonsSelectorTool, public ::AthAlgTool { 

public: 

  CalibMuonsSelectorTool( const std::string& type, const std::string& name, const IInterface* parent );
  virtual ~CalibMuonsSelectorTool() {} 

  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  virtual PairsVector GetMuons( const xAOD::MuonContainer* the_muons, TH1F* cut_flow );

  CalibMuonsSelectorTool();

  ToolHandle< CP::IMuonSelectionTool >     m_MuonSelectorTool;
  ToolHandle< IGoodRunsListSelectionTool > m_GRLTool;

private: 

  //::: Properties
  double m_PtCut;
  double m_EtaCut;
  double m_MinJpsiMassCut;
  double m_MaxJpsiMassCut;
  double m_MinZMassCut;
  double m_MaxZMassCut;

  //::: Methods
  bool IsGoodMuon( const xAOD::Muon* the_muon );
  bool AreInMassWindow( const xAOD::Muon* first_muon, const xAOD::Muon* second_muon );
  bool AreOppositelyCharged( const xAOD::Muon* first_muon, const xAOD::Muon* second_muon );

}; 

#endif
