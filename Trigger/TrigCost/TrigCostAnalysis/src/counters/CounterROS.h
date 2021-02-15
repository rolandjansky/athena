/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERROS_H
#define TRIGCOSTANALYSIS_COUNTERROS_H 1

#include "../CounterBase.h"

/**
 * @class CounterROS
 * @brief Concrete implimentation of Counter to monitor the data requests for 
 *        a single Read Out System request and corresponding Read Out Buffer requests.
 */
class CounterROS : public CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterROS() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterROS(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterROS() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterROS& operator=(const CounterROS&) = delete;
    
    /**
     * @brief Forbid copy.
     */
    CounterROS(const CounterROS&) = delete;

    /**
     * @brief Concrete implementation. Monitors global properties in a single LB, or over all LB in a Range
     * @param[in] data Access to event data
     * @param[in] index Index of data request to monitor
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, const float weight = 1.) override;

  private:
      /**
     * @brief Get histogram bin for ROBHistory enum values
     * @param[in] history ROBHistory value
     */
    int getROBHistoryBin(const unsigned history);

    std::vector<uint32_t> m_robIdsPerROS; //!< Cached mapping of ROB ids corresponding to ROS
};

#endif // TRIGCOSTANALYSIS_COUNTERROS_H