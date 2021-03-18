/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_TRIGSIGNATUREMONI_H
#define TRIGSTEERMONITOR_TRIGSIGNATUREMONI_H 1

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
class TrigSignatureMoni : public extends<AthReentrantAlgorithm, IIncidentListener>
{ 
 public: 

  TrigSignatureMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  start() override;
  virtual StatusCode  execute( const EventContext& context ) const override;
  virtual StatusCode  stop() override;
  virtual void handle( const Incident& incident ) override;

 private:
  // Class representing asynchronously published histograms
  class RateHistogram {
   public:
    ~RateHistogram();

    // Initialize rate histogram and create buffer
    StatusCode init( const std::string& histoName, const std::string& histoTitle,
      const int x, const int y, const std::string& registerPath, ServiceHandle<ITHistSvc> histSvc );

    // Get the histogram
    LockedHandle<TH2>& getHistogram() const;

    // Get the histogram buffer
    LockedHandle<TH2>& getBuffer() const;

    // Get rate histogram timer pointer
    std::unique_ptr<Athena::AlgorithmTimer>& getTimer();

    // Start the histogram timer with given duration and intervals
    void startTimer(unsigned int duration, unsigned int intervals);

    // Stop the histogram timer
    void stopTimer();

    // Fill the histogram with given bins x, y
    void fill(const double x, const double y) const;

   private:
    // Update the histogram with values from the buffer
    void updatePublished(unsigned int duration) const;

    // Callback to check if interval's duration passed and histogram should be published
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
  
  ServiceHandle<IIncidentSvc> m_incidentSvc{ this, "IncidentSvc", "IncidentSvc", "Incident service"};
  ServiceHandle<ITHistSvc> m_histSvc{ this, "THistSvc", "THistSvc/THistSvc", "Histogramming svc" };
  Gaudi::Property<std::string> m_bookingPath{ this, "HistPath", "/EXPERT/HLTFramework", "Booking path for the histogram"};
  Gaudi::Property<unsigned int> m_bcidNumber{ this, "BCIDNumber", 500, "Number of BCIDs in BCIDperChain histogram"};

  // Necessary for asynchronous calling callback function
  Gaudi::Property<unsigned int> m_duration {this, "RateIntegrationDuration", 10, "Integration time for the rate histogram in seconds"};
  Gaudi::Property<unsigned int> m_intervals {this, "RateIntegrationIntervals", 6, "Number of the rate histogram publications"};

  ToolHandleArray<DecisionCollectorTool> m_decisionCollectorTools{ this, "DecisionCollectorTools", {}, "Tools that collect decisions (yes/no) for steps" };
  ToolHandleArray<DecisionCollectorTool> m_featureCollectorTools{ this, "FeatureCollectorTools", {}, "Tools that collect decision counts for specific features for steps" };

  // Histograms
  mutable LockedHandle<TH2> m_passHistogram;
  mutable LockedHandle<TH2> m_countHistogram;
  mutable LockedHandle<TH2> m_bunchHistogram;
  RateHistogram m_rateHistogram;
  RateHistogram m_bcidHistogram;
  RateHistogram m_sequenceHistogram;

  std::map<unsigned int, int> m_chainIDToBinMap; //!< Chain id to histogram bin map
  std::map<unsigned int, int> m_BCIDchainIDToBinMap; //!< Chain id to bin map for DecisionsPerBCID histogram
  std::map<std::string, int> m_nameToBinMap; //!< Sequence/group/bunchgroup name to bin map
  std::map<std::string, int> m_sequenceToBinMap; //!< Sequence to bin map for sequence histogram
  std::map<unsigned int, std::set<std::string>> m_chainIDToBunchMap; //!< Chain ID to bunchgroup name map
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_groupToChainMap; //!< Group name to chain objects map
  std::map<std::string, TrigCompositeUtils::DecisionIDContainer> m_streamToChainMap; //!< Stream name to chain objects map


  // Returns number of chains + groups + sequencers based on the menu handle
  int nBinsX(SG::ReadHandle<TrigConf::HLTMenu>& ) const;

  // Returns number of chains + groups + sequencers based of size of m_chainIDToBinMap
  int nBinsX() const;

  // Returns number of chains based on the menu handle
  int nChains(SG::ReadHandle<TrigConf::HLTMenu>& ) const;

  // Returns number of BCIDs to monitor
  int nBCIDs() const;

  // Returns number of sequences
  int nSequenceBins() const;

  // Returns number of steps
  int nSteps() const;

  // Returns number of base steps: in, after ps, out steps
  int nBaseSteps() const;

  // Returns number of bunchgroups from l1 menu
  int nBunchGroups(SG::ReadHandle<TrigConf::L1Menu>& ) const;

  // Returns number of BCID chains to monitor
  int nBCIDchains() const;

  // Init different types of histograms
  StatusCode initHist(LockedHandle<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, bool = true);
  StatusCode initBunchHist(LockedHandle<TH2>&, SG::ReadHandle<TrigConf::HLTMenu>&, SG::ReadHandle<TrigConf::L1Menu>&);
  StatusCode initBCIDhist(LockedHandle<TH2>&, const std::vector<std::string>&);
  StatusCode initSeqHist(LockedHandle<TH2>&, std::set<std::string>&);
  
  // Fill different types of histograms
  StatusCode fillDecisionCount(const std::vector<TrigCompositeUtils::DecisionID>& , int) const;
  StatusCode fillPassEvents(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillRate(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillHistogram(const TrigCompositeUtils::DecisionIDContainer&, int, LockedHandle<TH2>&) const;
  StatusCode fillBunchGroups(const TrigCompositeUtils::DecisionIDContainer&) const;
  StatusCode fillBCID(const TrigCompositeUtils::DecisionIDContainer&, int) const;
  StatusCode fillSequences(const std::set<std::string>&) const;
  StatusCode fillStreamsAndGroups(const std::map<std::string, TrigCompositeUtils::DecisionIDContainer>&, const TrigCompositeUtils::DecisionIDContainer&) const;
};

#endif //> !TRIGSTEERMONITOR_TRIGSIGNATUREMONI_H
