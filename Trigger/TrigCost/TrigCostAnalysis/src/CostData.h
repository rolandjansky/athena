/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COSTDATA_H
#define TRIGCOSTANALYSIS_COSTDATA_H 1

#include "GaudiKernel/StatusCode.h"
#include "xAODTrigger/TrigCompositeContainer.h"

/**
 * @class CostData
 * @brief Caches and propagates event data to be used by monitoring algorithms.
 *
 * Cost Monitors and their Counters need access to different storegate collections, derived data,
 * and other normalisation factors. The CostData object wraps all of these, providing a single
 * "data" object which is passed down to all clients.
 */
class CostData {
  public:
    /**
     * @brief Construct an empty CostData.
     */
    CostData();

    /**
     * @brief Default destructor.
     */
    ~CostData() = default;

    /**
     * @brief Forbid assignment.
     */
    CostData& operator=(const CostData&) = delete;

    /**
     * @brief Forbid copy.
     */
    CostData(const CostData&) = delete;

    /**
     * @brief Cache the cost collection, after formally requesting it from storegate.
     */
    StatusCode set(const xAOD::TrigCompositeContainer* costCollection);

    /**
     * @brief Getter of the cached algorithm cost collection pointer.
     */
    const xAOD::TrigCompositeContainer& costCollection() const;

    /**
     * @brief Setter of effective P1 walltime represented by the current event.
     */
    void setEventLivetime(float time);

    /**
     * @brief Getter of effective P1 walltime represented by the current event.
     * @return Walltime in seconds.
     */
    float eventLiveTime() const;

    /**
     * @brief Getter of the total algorithm CPU time in the event. 
     * @return Total CPU time in milliseconds.
     */
    float algTotalTimeMilliSec() const;
    


  private:

    /**
     * @brief Compute and cache derived quantities, called automatically after set().
     * Computes algTotalTimeMilliSec()
     */
    StatusCode cache();

    const xAOD::TrigCompositeContainer* m_costCollection; //!< Cached non-owning pointer to main algorithm cost collection.
    uint64_t m_algTotalTime; //!< Integrated CPU time of all algorithms in the event. Stored in discrete microseconds.
    float m_eventLiveTime; //!< Effective P1 walltime of the event, in seconds.
};

#endif // TRIGCOSTANALYSIS_COSTDATA_H
