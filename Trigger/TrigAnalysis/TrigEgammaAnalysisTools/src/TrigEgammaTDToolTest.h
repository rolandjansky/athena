/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaTDToolTest.h 

#ifndef TrigEgammaTDToolTest_H
#define TrigEgammaTDToolTest_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
class IExecuteTool;

class TrigEgammaTDToolTest : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  TrigEgammaTDToolTest(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~TrigEgammaTDToolTest(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  TrigEgammaTDToolTest();

private:
  ToolHandle<Trig::TrigDecisionTool> m_trigdec;
  ToolHandle<Trig::ITrigEgammaMatchingTool> m_matchTool;
  std::map<std::string,int> m_counterBits;
  StoreGateSvc * m_storeGate;
}; 

#endif
