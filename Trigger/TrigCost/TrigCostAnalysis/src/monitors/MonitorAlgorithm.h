/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORALGORITHM_H
#define TRIGCOSTANALYSIS_MONITORALGORITHM_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorAlgorithm
 * @brief Concrete implementation of Monitor to monitor all Algorithm instances in an event.
 */
class MonitorAlgorithm : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorAlgorithm() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorAlgorithm(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorAlgorithm() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorAlgorithm& operator=(const MonitorAlgorithm&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorAlgorithm(const MonitorAlgorithm&) = delete;

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

#endif // TRIGCOSTANALYSIS_MONITORALGORITHM_H
