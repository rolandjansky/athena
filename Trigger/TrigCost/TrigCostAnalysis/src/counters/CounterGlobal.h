/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERGLOBAL_H
#define TRIGCOSTANALYSIS_COUNTERGLOBAL_H 1

#include "../CounterBase.h"

/**
 * @class CounterGlobal
 * @brief Concrete implimentation of Counter to monitor global properties of the event
 */
class CounterGlobal : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterGlobal() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterGlobal(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterGlobal() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterGlobal& operator=(const CounterGlobal&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterGlobal(const CounterGlobal&) = delete;

    /**
     * @brief Concrete implementation. Monitors global properties in a single LB, or over all LB in a Range
     * @param[in] data Access to event data
     * @param[in] incrementWalltime If 1, we should add the current events wall time to our internal histogram
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t incrementWalltime, const float weight = 1.) override;
};

#endif // TRIGCOSTANALYSIS_COUNTERGLOBAL_H
