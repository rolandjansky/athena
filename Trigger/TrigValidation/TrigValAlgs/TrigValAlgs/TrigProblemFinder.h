/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigValAlgs_TrigProblemFinder_h
#define TrigValAlgs_TrigProblemFinder_h


#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "AthenaBaseComps/AthAlgorithm.h"
namespace HLT {
  class HLTResult;
}

class TrigProblemFinder : public AthAlgorithm {
public:
  
  TrigProblemFinder(const std::string &name, ISvcLocator *pSvcLocator);
  ~TrigProblemFinder();

  
  // IAlgorithm virtual methods to implement
  virtual StatusCode initialize();
  virtual StatusCode execute(); 
  virtual StatusCode finalize();

 private:
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; 
   
  bool m_checkHeaders; 
  bool m_checkChains;
  bool m_checkNavigation;
  
  //  void checkStreamTag();
  void checkAndPrintErrorCode(HLT::ErrorCode, const std::string& context);

  void checkHeader(const HLT::HLTResult* );

  
  void checkChains();
  void checkNavigation();
  void checkTE(const HLT::TriggerElement* te, HLT::NavigationCore* nav, const Trig::TrigDecisionTool* td);
};


#endif
