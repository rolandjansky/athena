/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORCHAIN_H
#define TRIGCOSTANALYSIS_MONITORCHAIN_H 1


#include "../MonitorBase.h"

/**
 * @class MonitorAlgorithm
 * @brief Concrete implementation of Monitor to monitor all Chain instances in an event.
 */
class MonitorChain : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorChain() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorChain(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorChain() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorChain& operator=(const MonitorChain&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorChain(const MonitorChain&) = delete;

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

#endif // TRIGCOSTANALYSIS_MONITORCHAIN_H
