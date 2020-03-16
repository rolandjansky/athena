/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GOODRUNSLISTS_GRLSELECTORALG_H
#define GOODRUNSLISTS_GRLSELECTORALG_H 1

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/FilterReporterParams.h>
#include <AsgTools/ToolHandle.h> //included under assumption you'll want to use some tools! Remove if you don't!

#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

///Author: will buttinger
///To use this alg in your joboptions:
///masterseq = CfgMgr.AthSequencer("AthMasterSeq")
///ToolSvc += CfgMgr.GoodRunsListSelectionTool("myGRLTool",GoodRunsListVec=["my.grl.xml"])
///masterseq += CfgMgr.GRLSelectorAlg(Tool=ToolSvc.myGRLTool)


class GRLSelectorAlg: public EL::AnaAlgorithm { 
 public: 
  GRLSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GRLSelectorAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
  EL::FilterReporterParams m_filterParams {this};
  int m_passed = 0;
  int m_total = 0;

}; 

#endif //> !GOODRUNSLISTS_GRLSELECTORALG_H
