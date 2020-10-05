/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITOREDRANGE_H
#define TRIGCOSTANALYSIS_MONITOREDRANGE_H 1

#include "GaudiKernel/StatusCode.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "CostData.h"
#include "MonitorBase.h"

#include <string>
#include <set>
#include <memory>

class TrigCostAnalysis; // Forward reference
class TH1; // Forward reference

/**
 * @class MonitoredRange
 * @brief Container which represents a time range and holds a collection of Monitors which monitor this range.
 *
 * A job is configured to use a suite of Monitors. However, we also want to separate different regions of the 
 * run via lumi block splitting. This class represents one such time interval and hosts a complete set of 
 * Monitors which are used to monitor the time range.
 */
class MonitoredRange{
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitoredRange() = delete;

    /**
     * @brief Construct Range.
     *
     * Range constructor sets up a histogram which is used to keep track of normalisation quantities.
     * @param[in] name Range's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitoredRange(const std::string& name, TrigCostAnalysis* parent);

    /**
     * @brief Default destructor.
     */
    ~MonitoredRange() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitoredRange& operator=(const MonitoredRange&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitoredRange(const MonitoredRange&) = delete;

    /**
     * @brief Getter for Range's name
     * @return Range's name const string reference.
     */
    const std::string& getName() const;

    /**
     * @brief Return cached non-owning ptr to this Monitor's parent TrigCostAnalysis Athena algorithm.
     * @return Cached pointer to parent TrigCostAnalysis algorithm.
     */
    TrigCostAnalysis* getParent() const;

    /**
     * @brief Appends Range's name (to histogram path) and forwards histogram book request to parent Athena algorithm.
     * @param[in] hist Bare pointer to histogram. Ownership transferred to THistSvc.
     * @param[in] tDir Histogram name & directory.
     * @return Cached pointer to histogram. Used to fill histogram without having to perform THishSvc lookup. 
     */
    TH1* bookGetPointer(TH1* hist, const std::string& tDir = "") const;

    /**
     * @brief Getter for Range's owned Monitors
     * @return Set reference to Range's Monitors 
     */
    std::set< std::unique_ptr<MonitorBase> >& getMonitors();

    /**
     * @brief Adds a new Monitor into this Range.
     * @pram[in] Owning unique ptr of new Monitor to add. Typed on Monitor's base class. 
     */
    StatusCode addMonitor(std::unique_ptr<MonitorBase> monitor);

    /**
     * @brief Interface called by TrigCostAnalysis algorithm to instruct this Range to trigger all its Monitors.
     * @param[in] data Access to event data
     * @param[in] weight Global event weight
     */
    StatusCode newEvent(const CostData& data, const float weight = 1.);

  private:

    const std::string m_name; //!< Name of the Monitored Range
    TrigCostAnalysis* m_parent; //!< Cached ptr to the parent Athena algorithm
    TH1* m_cachedLifetimeHistPtr; //!< Cached histogram ptr used to store normalisation quantities.
    std::set< std::unique_ptr<MonitorBase> > m_monitors; //!< Storage of Range's Monitors. Keyed by Monitor's name.
    std::set< uint32_t > m_seenLB; //!< Luminosity blocks for which we have seen at least one event

};

#endif // TRIGCOSTANALYSIS_MONITOREDRANGE_H
