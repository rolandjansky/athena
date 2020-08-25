/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORTHREADOCCUPANCY_H
#define TRIGCOSTANALYSIS_MONITORTHREADOCCUPANCY_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorThreadOccupancy
 * @brief Concrete implementation of Monitor to monitor utilisation of individual threads in a MT job
 */
class MonitorThreadOccupancy : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorThreadOccupancy() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorThreadOccupancy(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorThreadOccupancy() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorThreadOccupancy& operator=(const MonitorThreadOccupancy&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorThreadOccupancy(const MonitorThreadOccupancy&) = delete;

    /**
     * @brief Concrete dispatch method. Iterate over all Algorithms in event data and dispatch to owned Counters
     * @param[in] data Access to event data
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, const float weight = 1.) override;

    /**
     * @brief Concrete counter instantiation. Mints named counter of CounterThread type.
     * @param[in] name Name of Counter to mint.
     * @return Owning unique ptr object typed on the CounterBase base class which points to concrete Counter of specialised type.
     */
    virtual std::unique_ptr<CounterBase> newCounter(const std::string& name) override; 

    /**
     * @brief Get the global start and stop timestamps over all threads and sync these to the individual threads.
     */
    StatusCode postProcess(float weight);

    std::unordered_map<uint32_t, std::string> m_threadToCounterMap; //!< Map thread's hash ID to a counting numeral embedded in a string
    size_t m_threadCounter; //!< Count how many unique thread ID we have seen 
};

#endif // TRIGCOSTANALYSIS_MONITORTHREADOCCUPANCY_H
