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
    StatusCode set(const xAOD::TrigCompositeContainer* costCollection, uint32_t onlineSlot);

    /**
     * @brief Getter of the cached algorithm cost collection pointer.
     */
    const xAOD::TrigCompositeContainer& costCollection() const;

    /**
     * @brief Setter of effective P1 walltime represented by the current event.
     */
    void setLb(uint32_t lb);

    /**
     * @brief Setter of the online Slot number of the current event.
     */
    void setOnlineSlot(uint32_t slot);

    /**
     * @brief Setter of effective P1 walltime represented by the current event, or the current lumi block. As specified by the second parameter
     */
    void setLivetime(float time, bool liveTimeIsPerEvent);

    /**
     * @brief Getter of effective P1 walltime represented by either the current event, or the current lumi block.
     * @return Walltime in seconds.
     */
    float liveTime() const;

    /**
     * @brief If a call to liveTime() is providing data on a single event or a whole LB
     * @return Walltime in seconds.
     */
    bool liveTimeIsPerEvent() const;

    /**
     * @brief Current luminosity block number
     * @return Luminosity block number.
     */
    uint32_t lb() const;

    /**
     * @return Online slot number
     */
    uint32_t onlineSlot() const;

    /**
     * @return True if event was processed in the master slot (0), and hence contains cost data spanning all concurrent slots
     */
    bool isMasterSlot() const;

    /**
     * @brief Getter of the total algorithm CPU time in the event. 
     * @return Total CPU time in milliseconds.
     */
    float algTotalTimeMilliSec() const;


    /**
     * @brief Get the class typename given an algorithm instance name. Name is supplied in serialised hashed form.
     * @return Total CPU time in milliseconds.
     */
    const std::string& algNameToClassType(size_t algNameHash) const;

    /**
     * @brief Set internal type map pointer
     * @return Total CPU time in milliseconds.
     */
    void setTypeMap( const std::unordered_map<uint32_t, std::string>& typeMap );

  private:

    /**
     * @brief Compute and cache derived quantities, called automatically after set().
     * Computes algTotalTimeMilliSec()
     */
    StatusCode cache();

    const xAOD::TrigCompositeContainer* m_costCollection; //!< Cached non-owning pointer to main algorithm cost collection.
    uint64_t m_algTotalTime; //!< Integrated CPU time of all algorithms in the event. Stored in discrete microseconds.
    float m_liveTime; //!< Effective walltime of either the event or the LB, in seconds (@see m_liveTimeIsPerEvent).
    uint32_t m_lb; //!< Current luminosity block number
    uint32_t m_slot; //!< Current online slot number
    bool m_liveTimeIsPerEvent; //!< If the livetime represents a single event of all of the current LB
    const std::unordered_map<uint32_t, std::string>* m_typeMapPtr; //!< Cached non-owning pointer mapping algorithm instance names to types
};

#endif // TRIGCOSTANALYSIS_COSTDATA_H
