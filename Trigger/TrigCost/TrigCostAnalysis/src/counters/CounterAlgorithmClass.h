/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERALGORITHMCLASS_H
#define TRIGCOSTANALYSIS_COUNTERALGORITHMCLASS_H 1

#include "CounterAlgorithm.h"

/**
 * @class CounterAlgorithmClass
 * @brief Concrete implimentation of Counter to monitor all properties of a given Algorithm class. 
          The new event logic is the same as in CounterAlgorithm, only histogram definition differs
 */
class CounterAlgorithmClass : public CounterAlgorithm {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterAlgorithmClass() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterAlgorithmClass(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterAlgorithmClass() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterAlgorithmClass& operator=(const CounterAlgorithmClass&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterAlgorithmClass(const CounterAlgorithmClass&) = delete;
};

#endif // TRIGCOSTANALYSIS_COUNTERALGORITHMCLASS_H
