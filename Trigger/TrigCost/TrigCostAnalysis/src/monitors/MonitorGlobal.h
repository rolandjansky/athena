/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORGLOBAL_H
#define TRIGCOSTANALYSIS_MONITORGLOBAL_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorGlobal
 * @brief Concrete implementation of Monitor to monitor global properties of event execution
 */
class MonitorGlobal : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorGlobal() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorGlobal(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorGlobal() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorGlobal& operator=(const MonitorGlobal&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorGlobal(const MonitorGlobal&) = delete;

    /**
     * @brief Concrete dispatch method. Iterate over all Algorithms in event data and dispatch to owned Counters
     * @param[in] data Access to event data
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, const float weight = 1.) override;

    /**
     * @brief Concrete counter instantiation. Mints named counter of CounterAlgorith type.
     * @param[in] name Name of Counter to mint.
     * @return Owning unique ptr object typed on the CounterBase base class which points to concrete Counter of specialised type.
     */
    virtual std::unique_ptr<CounterBase> newCounter(const std::string& name) override; 
};

#endif // TRIGCOSTANALYSIS_MONITORGLOBAL_H
