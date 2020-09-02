/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORALGORITHMCLASS_H
#define TRIGCOSTANALYSIS_MONITORALGORITHMCLASS_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorAlgorithmClass
 * @brief Concrete implementation of Monitor to monitor all Algorithm instances in an event, grouped by their instance class.
 */
class MonitorAlgorithmClass : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorAlgorithmClass() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorAlgorithmClass(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorAlgorithmClass() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorAlgorithmClass& operator=(const MonitorAlgorithmClass&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorAlgorithmClass(const MonitorAlgorithmClass&) = delete;

    /**
     * @brief Concrete dispatch method. Iterate over all Algorithms in event data and dispatch to owned Counters based on class name lookup
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

#endif // TRIGCOSTANALYSIS_MONITORALGORITHMCLASS_H
