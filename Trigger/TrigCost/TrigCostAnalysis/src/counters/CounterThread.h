/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERTHREAD_H
#define TRIGCOSTANALYSIS_COUNTERTHREAD_H 1

#include "../CounterBase.h"

/**
 * @class CounterThread
 * @brief Concrete implimentation of Counter to monitor all algorithms executing on a single thread.
 */
class CounterThread : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterThread() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterThread(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterThread() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterThread& operator=(const CounterThread&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterThread(const CounterThread&) = delete;

    /**
     * @brief Concrete implementation. Monitores algorithm at specified index for this counter's thread.
     * @param[in] data Access to event data
     * @param[in] index Index within appropriate event data container which is to be analysed by this Counter
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, const float weight = 1.0) override;

    /**
     * @brief Apply post-processing to the Counter, before endEvent is called
     * @param[in] weight Global event weight
     */
    StatusCode postProcess(float weight = 1.0); 

    /**
     * @brief Get the lowest timestamp of this single thread
     */
    uint64_t getLowTimestamp() const;

    /**
     * @brief Get the highest timestamp of this single thread
     */
    uint64_t getHighTimestamp() const;

    /**
     * @brief Set the low and high timestamps over all threads
     */
    void setAllThreadsTimestamps(uint64_t low, uint64_t high);

    uint64_t m_lowTimestamp; //!< Records the lowest timestamp seen on this single thread in an event
    uint64_t m_highTimestamp; //!< Records the highest timestamp seen on this single thread in an event

    uint64_t m_globalLowTimestamp; //!< Records the lowest timestamp seen over all threads in an event
    uint64_t m_globalHighTimestamp; //!< Records the highest timestamp seen over all threads in an event

};


#endif // TRIGCOSTANALYSIS_COUNTERTHREAD_H
