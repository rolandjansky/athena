/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
#define TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H 1

#include <string>
#include <mutex>
#include <memory>
#include <vector>
#include <TH2.h>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/LockedHandle.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/versions/TrigComposite_v1.h"
#include "DecisionCollectorTool.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/DataStructure.h"

#include "TimeDivider.h"
#include "AthenaKernel/AlgorithmTimer.h"

// Forward declarations
class IIncidentSvc;

/**
 * @class TrigSignatureMoniMT
 * @brief Algorithm implementing monitoring of the HLT decision in the MT framework
 **/
class TrigSignatureMoniMT : public extends<AthReentrantAlgorithm, IIncidentListener>
{ 
 public: 

  TrigSignatureMoniMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  start() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  stop() override;
  virtual void handle( const Incident& incident ) override;

 private:
  //class representing asynchronously published histograms
  class RateHistogram {
   public:
    ~RateHistogram();

    StatusCode init( const std::string histoName, const std::string histoTitle,
      const int x, const int y, const std::string registerPath, ServiceHandle<ITHistSvc> histSvc );

    LockedHandle<TH2> & getHistogram() const;
    LockedHandle<TH2> & getBuffer() const;
    std::unique_ptr<Athena::AlgorithmTimer> & getTimer();

    void startTimer(unsigned int duration, unsigned int intervals);
    void stopTimer();
    void fill(const double, const double) const;

   private:
    void updatePublished(unsigned int duration) const;
    void callback() const;

    mutable LockedHandle<TH2> m_bufferHistogram;
    mutable LockedHandle<TH2> m_histogram;
    std::mutex m_mutex;
    std::unique_ptr<Athena::AlgorithmTimer> m_timer;
    std::unique_ptr<TimeDivider> m_timeDivider;
    unsigned int m_duration;
  };

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_l1DecisionsKey{ this, "L1Decisions", "L1DecoderSummary", "Chains activated after the L1" };
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalDecisionKey{ this, "FinalDecisionKey", "HLTNav_Summary", "Final stage of all decisions" };
  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };
  SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey{ this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };

  std::map<unsigned int, int> m_chainIDToBinMap;
  std::map<unsigned int, int> m_BCIDchainIDToBinMap;
  std::map<std::string, int> m_nameToBinMap;
  std::map<std::string, int> m_sequenceToBinMap;
  std::map<unsigned int, std::set<std::string>> m_chainIDToBunchMap;
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_groupToChainMap;
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_streamToChainMap;
  
  ServiceHandle<IIncidentSvc> m_incidentSvc{ this, "IncidentSvc", "IncidentSvc", "Incident service"};
  ServiceHandle<ITHistSvc> m_histSvc{ this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_bookingPath{ this, "HistPath", "/EXPERT/HLTFramework", "Booking path for the histogram"};
  Gaudi::Property<unsigned int> m_bcidNumber{ this, "BCIDNumber", 500, "Number of BCIDs in BCIDperChain histogram"};

  mutable LockedHandle<TH2> m_passHistogram;
  mutable LockedHandle<TH2> m_countHistogram;
  mutable LockedHandle<TH2> m_bunchHistogram;
  RateHistogram m_rateHistogram;
  RateHistogram m_bcidHistogram;
  RateHistogram m_sequenceHistogram;

  //necessary for asynchronous calling callback function
  Gaudi::Property<unsigned int> m_duration {this, "RateIntegrationDuration", 10, "Integration time for the rate histogram in seconds"};
  Gaudi::Property<unsigned int> m_intervals {this, "RateIntegrationIntervals", 6, "Number of the rate histogram publications"};

  ToolHandleArray<DecisionCollectorTool> m_collectorTools{ this, "CollectorTools", {}, "Tools that collect decisions for steps" };
  
  int nBinsX(SG::ReadHandle<TrigConf::HLTMenu>& ) const;
  int nBinsX() const;
  int nBunchBinsX(SG::ReadHandle<TrigConf::HLTMenu>& ) const;
  int nBCIDbinsX() const;
  int nSequenceBinsX() const;
  int nBinsY() const;
  int nRateBinsY() const;
  int nBunchBinsY(SG::ReadHandle<TrigConf::L1Menu>& ) const;
  int nBCIDbinsY() const;

  StatusCode initHist(LockedHandle<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, bool = true);
  StatusCode initBunchHist(LockedHandle<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, SG::ReadHandle<TrigConf::L1Menu>&);
  StatusCode initBCIDhist(LockedHandle<TH2>&, const std::vector<std::string>&);
  StatusCode initSeqHist(LockedHandle<TH2>&, std::set<std::string>&);
  
  StatusCode fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& , int) const;
  StatusCode fillPassEvents(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillRate(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillHistogram(const TrigCompositeUtils::DecisionIDContainer&, int, LockedHandle<TH2>&) const;
  StatusCode fillBunchGroups(const TrigCompositeUtils::DecisionIDContainer&) const;
  StatusCode fillBCID(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillSequences(const std::set<std::string>&) const;
  StatusCode fillStreamsAndGroups(const std::map<std::string, TrigCompositeUtils::DecisionIDContainer>&, const TrigCompositeUtils::DecisionIDContainer&) const;
};

#endif //> !TRIGSTEERMONITOR_TRIGSIGNATUREMONIMT_H
