/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
#define TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H 1

#include <string>
#include <mutex>
#include <TH2.h>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODEventInfo/EventInfo.h"


/**
 * @class Algorithm implementing monitoring of the HLT decision in the MT framework
 * @brief 
 **/


class TrigSignatureMoniMT : public ::AthAlgorithm
{ 
 public: 

  TrigSignatureMoniMT( const std::string& name, ISvcLocator* pSvcLocator );


  virtual StatusCode  initialize() override;
  virtual StatusCode  execute() override;
  virtual StatusCode  finalize() override;
 
 private: 
  TrigSignatureMoniMT();
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_l1DecisionsKey{ this, "L1Decisions", "HLTChains", "Chains activated after the L1" };

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_finalDecisionsKey{ this, "FinalDecisions", {}, "Final stage of all decisions" };
  typedef std::map<std::string, std::vector<std::string> > StringToStringVectorMap;
  Gaudi::Property< StringToStringVectorMap > m_steps{ this, "FinalDecisionToStepsMap", {}, "Mapping from each final decision object in FinalDecisions to the decisions at earlier steps" };
  Gaudi::Property<std::vector<std::string> > m_allChains{ this, "ChainsList", {}, "List of all configured chains" };
  
  std::map<unsigned int, int> m_chainIDToBinMap;
  
  ServiceHandle<ITHistSvc> m_histSvc{ this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_bookingPath;

  TH2* m_outputHistogram;
  
  
  int nBinsX() const { return m_allChains.size() +1; }
  int nBinsY() const;
  StatusCode initHist();
  StatusCode fillChains(const TrigCompositeUtils::DecisionIDContainer& dc, int row);
}; 


#endif //> !TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
