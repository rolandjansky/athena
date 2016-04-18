/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_ISOLATIONTOOL_ATHTEST_H
#define ISOLATIONTOOL_ISOLATIONTOOL_ATHTEST_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"


class IsolationTool_AthTest: public ::AthAlgorithm { 
 public: 
  IsolationTool_AthTest( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~IsolationTool_AthTest(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  std::string m_containerName;
  std::string m_prefix;

  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_caloIsolationTool;
  std::vector<xAOD::Iso::IsolationType> m_ptconeTypes;
  std::vector< int > m_ptcones;
  xAOD::TrackCorrection m_trkCorrList; 
  std::vector<xAOD::Iso::IsolationType> m_topoetconeTypes;
  std::vector<int> m_topoetcones;
  xAOD::CaloCorrection m_caloCorrList;

  std::vector< SG::AuxElement::Decorator< float >* > m_decorators;
}; 

#endif //> !ISOLATIONTOOL_ISOLATIONTOOL_ATHTEST_H
