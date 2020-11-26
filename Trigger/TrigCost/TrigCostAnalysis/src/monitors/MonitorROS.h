/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_MONITORROS_H
#define TRIGCOSTANALYSIS_MONITORROS_H 1

#include "../MonitorBase.h"

/**
 * @class MonitorROS
 * @brief Concrete implimentation of Monitor to monitor Read Out System
 */
class MonitorROS : public MonitorBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    MonitorROS() = delete;

    /**
     * @brief Construct monitor.
     * @param[in] name Monitor's name
     * @param[in] parent Monitor's parent Range, cached non-owning pointer.
     */
    MonitorROS(const std::string& name, const MonitoredRange* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~MonitorROS() = default;

    /**
     * @brief Forbid assignment.
     */
    MonitorROS& operator=(const MonitorROS&) = delete;

    /**
     * @brief Forbid copy.
     */
    MonitorROS(const MonitorROS&) = delete;

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

  private:
    std::map<uint32_t, std::string> m_robToRos; //!< Cache correspondis ROS per ROB id
};

#endif // TRIGCOSTANALYSIS_MONITORROS_H