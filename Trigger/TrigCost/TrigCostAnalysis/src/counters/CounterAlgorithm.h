/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERALGORITHM_H
#define TRIGCOSTANALYSIS_COUNTERALGORITHM_H 1

#include "../CounterBase.h"

/**
 * @class CounterAlgorithm
 * @brief Concrete implimentation of Counter to monitor all properties of a given Algorithm instance.
 */
class CounterAlgorithm : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterAlgorithm() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     * @param[in] skipHistograms Skip histogram's decalration - for derived classes
     */
    CounterAlgorithm(const std::string& name, const MonitorBase* parent, const bool skipHistograms=false);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterAlgorithm() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterAlgorithm& operator=(const CounterAlgorithm&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterAlgorithm(const CounterAlgorithm&) = delete;

    /**
     * @brief Concrete implimentation. Monitores algorithm at specified index. Expected to match this instances name().
     * @param[in] data Access to event data
     * @param[in] index Index within appropriate event data container which is to be analysed by this Counter
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, const float weight = 1.) override;
};

#endif // TRIGCOSTANALYSIS_COUNTERALGORITHM_H
