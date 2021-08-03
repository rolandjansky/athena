/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERSEQUENCE_H
#define TRIGCOSTANALYSIS_COUNTERSEQUENCE_H 1

#include "../CounterBase.h"

/**
 * @class CounterSequence
 * @brief Concrete implimentation of Counter to monitor all properties of a given Sequence instance.
 */
class CounterSequence : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterSequence() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterSequence(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterSequence() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterSequence& operator=(const CounterSequence&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterSequence(const CounterSequence&) = delete;

    /**
     * @brief Concrete implimentation. Monitores Sequence at specified index. Expected to match this instances name().
     * @param[in] data Access to event data
     * @param[in] index Index within appropriate event data container which is to be analysed by this Counter
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, const float weight = 1.) override;
};

#endif // TRIGCOSTANALYSIS_COUNTERSEQUENCE_H
