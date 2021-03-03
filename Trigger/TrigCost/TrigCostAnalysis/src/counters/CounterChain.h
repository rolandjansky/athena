/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERCHAIN_H
#define TRIGCOSTANALYSIS_COUNTERCHAIN_H 1

#include "../CounterBase.h"

/**
 * @class CounterAlgorithm
 * @brief Concrete implimentation of Counter to monitor all properties of a given Algorithm instance.
 */
class CounterChain : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterChain() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterChain(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterChain() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterChain& operator=(const CounterChain&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterChain(const CounterChain&) = delete;

    /**
     * @brief Concrete implimentation. Monitores chain's algorithm at specified index. Expected to match this instances name().
     * @param[in] data Access to event data
     * @param[in] index Index within appropriate event data container which is to be analysed by this Counter
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, const float weight = 1.) override;
};

#endif // TRIGCOSTANALYSIS_COUNTERALGORITHM_H
