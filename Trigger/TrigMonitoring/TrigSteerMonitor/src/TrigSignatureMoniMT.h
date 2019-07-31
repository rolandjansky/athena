/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
#define TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H 1

#include <string>
#include <mutex>
#include <memory>
#include <TH2.h>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/LockedHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODEventInfo/EventInfo.h"
#include "DecisionCollectorTool.h"

#include "TimeDivider.h"
#include "AthenaKernel/AlgorithmTimer.h"
/**
 * @class Algorithm implementing monitoring of the HLT decision in the MT framework
 * @brief 
 **/


class TrigSignatureMoniMT : public ::AthReentrantAlgorithm
{ 
 public: 

  TrigSignatureMoniMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  start() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  finalize() override;

 private:
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_l1DecisionsKey{ this, "L1Decisions", "L1DecoderSummary", "Chains activated after the L1" };
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalDecisionKey{ this, "FinalDecisionKey", "HLTSummary", "Final stage of all decisions" };

  Gaudi::Property<std::vector<std::string> > m_allChains{ this, "ChainsList", {}, "List of all configured chains" };
 
  std::map<unsigned int, int> m_chainIDToBinMap;
  
  ServiceHandle<ITHistSvc> m_histSvc{ this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_bookingPath{ this, "HistParh", "/EXPERT/TrigSteer_HLT", "Booking path for the histogram"};

  mutable LockedHandle<TH2> m_passHistogram;
  mutable LockedHandle<TH2> m_countHistogram;
  mutable LockedHandle<TH2> m_rateHistogram;
  mutable LockedHandle<TH2> m_outputHistogram;

  std::unique_ptr<Athena::AlgorithmTimer> m_timer;

  //helper to know when to switch to new interval  
  std::unique_ptr<TimeDivider> m_timeDivider;

  //necessary for asynchronous calling callback function
  Gaudi::Property<unsigned int> m_duration {this, "RateIntegrationDuration", 10, "Integration time for the rate histogram in seconds"};
  Gaudi::Property<unsigned int> m_intervals {this, "RateIntegrationIntervals", 6, "Number of the rate histogram publications"};

  ToolHandleArray<DecisionCollectorTool> m_collectorTools{ this, "CollectorTools", {}, "Tools that collect decisions for steps" };
  
  int nBinsX() const;
  int nBinsY() const;
  void callback() const;
  void updatePublished(unsigned int duration) const;
  StatusCode initHist(std::unique_ptr<TH2>&);
  StatusCode fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& dc, int row) const;
  StatusCode fillPassEvents(const TrigCompositeUtils::DecisionIDContainer& dc, int row, LockedHandle<TH2>& histogram) const;
  StatusCode fillRate(const TrigCompositeUtils::DecisionIDContainer& dc, int row) const;
};

#endif //> !TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
