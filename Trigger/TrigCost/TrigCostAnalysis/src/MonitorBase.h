/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORBASE_H
#define TRIGCOSTANALYSIS_MONITORBASE_H 1

#include "GaudiKernel/StatusCode.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "TrigConfHLTData/HLTUtils.h"

#include "CostData.h"
#include "CounterBase.h"

#include <unordered_map>

class MonitoredRange; //!< Forward declare
class TH1; //!< Forward declare

/**
 * @class MonitorBase
 * @brief Base class for all "Monitor" objects, which maintain a collection of Counters of a given specialisation.
 *
 * A Monitor is responsible for monitoring a specific feature of an event. It could be, monitoring all Chains,
 * all ROS', all Algorithms, etc. It maintains an internal name-keyed map of Counters of a given specialisation
 * and for a given event it loops over all instances of the feature it monitors and dispatches to the matching 
 * Counter objects. 
 */
class MonitorBase{
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorBase() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorBase(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorBase() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorBase& operator=(const MonitorBase&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorBase(const MonitorBase&) = delete;

    /**
     * @brief Getter for Monitor's name
     * @return Counter's name const string reference.
     */
    const std::string& getName() const;

    /**
     * @brief Return cached non-owning const ptr to this Monitor's parent Range.
     * @return Cached pointer parent Range.
     */
    const MonitoredRange* getParent() const;

    /**
     * @brief Appends Monitor name (to histogram path) and forwards histogram book request to parent Range.
     * @param[in] hist Bare pointer to histogram. Ownership transferred to THistSvc.
     * @param[in] tDir Histogram name & directory.
     * @return Cached pointer to histogram. Used to fill histogram without having to perform THishSvc lookup. 
     */
    TH1* bookGetPointer(TH1* hist, const std::string& tDir = "") const;

    /**
     * @brief Check if a counter of a given name exists.
     * @param[in] name Name of Counter.
     * @return True if counter already exists.  
     */
    bool counterExists(const std::string& name) const;

    /**
     * @brief Retrieve counter by name. If no such counter exists, a new one will be instanced and returned.
     * @param[in] name Name of Counter.
     * @return Mutable base-class pointer to Counter.  
     */
    CounterBase* getCounter(const std::string& name);

    /**
     * @brief Pure virtual interface called by Range to instruct this Monitor to perform its analysis
     * @param[in] data Access to event data
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, float weight = 1.) = 0;

    /**
     * @brief Called by the framework. Causes per-Event Variables to fill their histograms with their accumulated data.
     */
    virtual StatusCode endEvent(float weight = 1.);

  protected:

    /**
     * @brief Pure virtual Counter instantiation specialisation. Derived Monitor must instantiate a concrete Counter
     * of the correct type for the Monitor and return it wrapped in an owning unique ptr.
     * @param[in] name Name of Counter to mint.
     * @return Owning unique ptr object typed on the CounterBase base class which points to concrete Counter of specialised type.
     */
    virtual std::unique_ptr<CounterBase> newCounter(const std::string& name) = 0; 

    std::unordered_map< std::string, std::unique_ptr<CounterBase> > m_counters; //!< Storage of Monitor's collection of Counters. Keyed by name.

  private:

    const std::string m_name; //!< Monitor's name
    const MonitoredRange* m_parent; //!< Monitor's parent Range. Cached non-owning const ptr.

};

#endif // TRIGCOSTANALYSIS_MONITORBASE_H
