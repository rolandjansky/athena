/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORCHAINALGORITHM_H
#define TRIGCOSTANALYSIS_MONITORCHAINALGORITHM_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorChainAlgorithm
 * @brief Concrete implementation of Monitor to monitor all Algorithm instances associated with chain in an event.
 */
class MonitorChainAlgorithm : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorChainAlgorithm() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorChainAlgorithm(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorChainAlgorithm() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorChainAlgorithm& operator=(const MonitorChainAlgorithm&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorChainAlgorithm(const MonitorChainAlgorithm&) = delete;

    /**
     * @brief Concrete dispatch method. Iterate over all Algorithms associated with chain in event data and dispatch to owned Counters
     * @param[in] data Access to event data
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, const float weight = 1.) override;

    /**
     * @brief Concrete counter instantiation. Mints named counter of CounterAlgorithm type.
     * @param[in] name Name of Counter to mint.
     * @return Owning unique ptr object typed on the CounterBase base class which points to concrete Counter of specialised type.
     */
    virtual std::unique_ptr<CounterBase> newCounter(const std::string& name) override; 
};

#endif // TRIGCOSTANALYSIS_MONITORCHAINALGORITHM_H
