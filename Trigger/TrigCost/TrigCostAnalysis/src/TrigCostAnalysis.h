/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_TRIGCOSTALYSIS_H
#define TRIGCOSTANALYSIS_TRIGCOSTALYSIS_H 1

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigConfData/HLTMenu.h"

#include "EnhancedBiasWeighter/EnhancedBiasWeighter.h"

#include "MonitoredRange.h"

#include <unordered_map>

class TH1; //!< Forward reference

/**
 * @class TrigCostAnalysis
 * @brief Athena algorithm used to process Trigger cost monitoring data in Run 3 and above. Outputs histograms.
 *
 * Algorithm to read relevant trigger cost data from the event store and trigger configuration data from the 
 * config service, identify the time Range that the event falls into, and dispatches monitoring to the
 * correct time range. Time ranges, their monitors, and their monitor's counters are all instantiated on-demand.
 */
class TrigCostAnalysis: public ::AthHistogramAlgorithm { 
  public: 

    /**
     * @brief Construct TrigCostAnalysis.
     * @param[in] name Athena algorithm's name.
     * @param[in] pSvcLocator Framework service pointer.
     */
    TrigCostAnalysis( const std::string& name, ISvcLocator* pSvcLocator );

    /**
     * @brief Default destructor.
     */
    virtual ~TrigCostAnalysis() = default; 

    /**
     * @brief Retrieve tools and initialise read handles.
     */
    virtual StatusCode initialize() final;

    /**
     * @brief Retrieve menu handle
     */
    virtual StatusCode start();

    /**
     * @brief Monitor event, unless max range limit reached and event outside of all ranges.
     */
    virtual StatusCode execute() final;

    /**
     * @brief Currently a noop for this algorithm.
     */
    virtual StatusCode finalize() final;

    /**
     * @brief Public method forwarded to this class' AthHistogramAlgorithm::bookGetPointer base.
     * @param[in] hist Bare pointer to histogram. Ownership transferred to THistSvc.
     * @param[in] tDir Histogram name & directory.
     * @return Cached pointer to histogram. Used to fill histogram without having to perform THishSvc lookup. 
     */
    TH1* bookGetPointer_fwd(TH1* hist, const std::string& tDir = "");

    Gaudi::Property<bool> m_singleTimeRange { this, "UseSingleTimeRange", false,
      "Use a single time range rather than splitting by LB" };

    Gaudi::Property<std::string> m_singleTimeRangeName { this, "SingleTimeRangeName", "All",
      "Name for single time range" };

    Gaudi::Property<std::string> m_additionalHashMap { this, "AdditionalHashMap", "TrigCostRootAnalysis/hashes2string_08072020.txt",
      "Used to load strings corresponding to algorithms which are not explicitly scheduled by chains. To be updated periodically." };

    Gaudi::Property<size_t> m_TimeRangeLengthLB { this, "TimeRangeLengthLB", 50,
      "Length of each variable length Time Range in LB" };

    Gaudi::Property<size_t> m_maxTimeRange { this, "MaxTimeRanges", 5,
      "Maximum number of allowed time ranges" };

    Gaudi::Property<bool> m_doMonitorAlgorithm { this, "DoMonitorAlgs", true,
      "Monitor individual algorithms by instance name" };

    Gaudi::Property<bool> m_doMonitorAlgorithmClass { this, "DoMonitorAlgClass", true,
      "Monitor individual algorithms by instance class type name" };      

    Gaudi::Property<bool> m_doMonitorGlobal { this, "DoMonitorGlobal", true,
      "Monitor global event properties" };

    Gaudi::Property<bool> m_doMonitorThreadOccupancy { this, "DoMonitorThreadOccupancy", true,
      "Monitor algorithm occupancy load of individual threads in an MT execution environment" };

    Gaudi::Property<bool> m_useEBWeights { this, "UseEBWeights", true,
      "Apply Enhanced Bias weights" };

    Gaudi::Property<size_t> m_maxFullEventDumps { this, "MaxFullEventDumps", 10,
      "Maximum number of full event summaries which will be dumped" };

    Gaudi::Property<uint64_t> m_fullEventDumpProbability { this, "FullEventDumpProbability", 10,
      "Save a full record of one in every N events, up to MaxFullEventDumps." }; 

    Gaudi::Property<float> m_baseEventWeight { this, "BaseEventWeight", true,
      "Base events weight, other weights may be multiplied on top of this one." };

    SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_costDataKey { this, "CostReadHandleKey", "HLT_TrigCostContainer",
      "Trigger cost payload container for algorithms" };

    SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
      "HLT Menu"};

    ToolHandle<IEnhancedBiasWeighter> m_enhancedBiasTool{this, "EnhancedBiasTool", "",
      "Enhanced bias weighting tool."};
   
  private:

    /**
     * @brief Check if event dumping should be performed for the current event.
     * @param[in] context Event context.
     * @param[in] costData We only dump the master-slot (slot:0) as these events cary payload data for all slots / all threads.
     * @return True, if the event should be dumped.
     */
    bool checkDoFullEventDump(const EventContext& context, const CostData& costData);

    /**
     * @brief Return or construct and return a Range for the Context. Might return nullptr range.
     * @param[in] context Event context.
     * @param[out] range Reference to a mutable range ptr. Will be set to point to the Range. 
     * If range does not exist it will be created unless the maximum number of ranges has been
     * reached. If this happens, range will be set to nullptr.
     */
    StatusCode getRange(const EventContext& context, MonitoredRange*& range);

    /**
     * @brief Dump event algorithm execution data in ASCII format.
     * @param[in] context Event context.
     */
    StatusCode dumpEvent(const EventContext& context) const;

    /**
     * @brief Populate a newly minted Range object with all configured Monitors.
     * @param[in] range Mutable ptr to new Range object which is to be populated.
     */
    StatusCode registerMonitors(MonitoredRange* range);

    /**
     * @brief Compute global event weight to correct for online prescales.
     * @param[in] context Event context.
     * @return Global event weight.
     */
    float getWeight(const EventContext& context);

    /**
     * @brief Return the slot used to process the event online
     * @param[in] costCollection Cost data.
     * @return Oneline slot number..
     */
    uint32_t getOnlineSlot(const xAOD::TrigCompositeContainer* costCollection) const;


    /**
     * @brief High watermark for pre-cached string hashes for the SLOT category. Corresponding to SG and View IProxyDict names.
     * @param[in] max Pre-compute string hashes for View or Slot multiplicities up to this number.
     */
    StatusCode checkUpdateMaxView(const size_t max);

    std::unordered_map<std::string, std::unique_ptr<MonitoredRange> > m_monitoredRanges; //!< Owned storage of Ranges. Keyed on Range name.
    std::unordered_map<uint32_t, std::string > m_algTypeMap; //!< Cache of algorithm's type, read from configuration data.
    std::set<std::string> m_storeIdentifiers; //!< Identifiers of object stores, needed to cache STORE string-hash values

    mutable std::atomic<size_t> m_fullEventDumps; //!< Counter to keep track of how many events have been full-dumped
    mutable std::atomic<size_t> m_maxViewsNumber; //!< What is the maximum number of View instances we've so far cached string hashes to cover?

}; 

#endif // TRIGCOSTANALYSIS_TRIGCOSTALYSIS_H
