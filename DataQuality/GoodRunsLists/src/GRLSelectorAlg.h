/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODRUNSLISTS_GRLSELECTORALG_H
#define GOODRUNSLISTS_GRLSELECTORALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

///Author: will buttinger
///To use this alg in your joboptions:
///filterseq = CfgMgr.AthSequencer("AthFilterSeq")
///ToolSvc += CfgMgr.GoodRunsListSelectionTool("myGRLTool",GoodRunsListVec=["my.grl.xml"])
///filterseq += CfgMgr.GRLSelectorAlg(Tool=ToolSvc.myGRLTool)


class GRLSelectorAlg: public ::AthAlgorithm { 
 public: 
  GRLSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GRLSelectorAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
  int m_passed = 0;
  int m_total = 0;

}; 

#endif //> !GOODRUNSLISTS_GRLSELECTORALG_H
