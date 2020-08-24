/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_COUNTERBASE_H
#define TRIGCOSTANALYSIS_COUNTERBASE_H 1

#include "GaudiKernel/StatusCode.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "CostData.h"
#include "Variable.h"

class MonitorBase; //!< Forward declare
class TH1; //!< Forward declare

/**
 * @class CounterBase
 * @brief Base class for all "Counter" objects, with common histogram management functionality.
 *
 * A Counter object represents a logical object which has monitored properties. A counter might represent
 * an Algorithm, a Chain, a Lumi block, a ROS, etc. Each specialisation will have its own set of histograms. 
 */
class CounterBase {
  public:
    /**
     * @brief Forbid default constructor.
     */
    CounterBase() = delete;

    /**
     * @brief Construct counter.
     * @param[in] name Counter's name
     * @param[in] parent Counter's parent monitor, cached non-owning pointer.
     */
    CounterBase(const std::string& name, const MonitorBase* parent);

    /**
     * @brief Default destructor.
     */
    virtual ~CounterBase() = default;

    /**
     * @brief Forbid assignment.
     */
    CounterBase& operator=(const CounterBase&) = delete;

    /**
     * @brief Forbid copy.
     */
    CounterBase(const CounterBase&) = delete;

    /**
     * @brief Getter for Counter's name
     * @return Counter's name const string reference.
     */
    const std::string& getName() const;

    /**
     * @brief Return cached non-owning const ptr to this Counter's parent Monitor.
     * @return Cached pointer parent Monitor.
     */
    const MonitorBase* getParent() const;

    /**
     * @brief Returns a mutable reference to a named Variable. Throws if no such Variable exists.
     * Used when more complicated logic is needed on a Variable than simply using the fill() method.
     * @param[in] name Variable to return
     * @return Mutable reference to Variable
     */
    Variable& getVariable(const std::string& name);

    /**
     * @brief Fill (for per-Call) or accumulate in a buffer (for per-Event) a quantity histogrammed by a named Variable 
     * @param[in] name Variable to increment
     * @param[in] value The payload value
     * @param[in] weight Global event weight
     */
    StatusCode fill(const std::string& name, float value, float weight = 1.0);

    /**
     * @brief Convenience function. Equivalent to fill(name, 1.0, weight);
     * @param[in] name Variable to increment
     * @param[in] weight Global event weight
     */
    StatusCode increment(const std::string& name, float weight = 1.0);

    /**
     * @brief Optional for per-Event Variables. Sets a denominator to divide the value through by before filling. 
     * @param[in] name Variable to set the denominator for.
     * @param[in] value The denominator
     */
    StatusCode setDenominator(const std::string& name, float value);

    /**
     * @brief Called by the framework. Causes per-Event Variables to fill their histograms with their accumulated data.
     */
    virtual StatusCode endEvent(float weight = 1.0);

    /**
     * @brief Pure virtual interface called by Monitor to instruct this Counter to perform its analysis
     * @param[in] data Access to event data
     * @param[in] index Index within appropriate event data container which is to be analysed by this Counter
     * @param[in] weight Global event weight
     */
    virtual StatusCode newEvent(const CostData& data, size_t index, float weight = 1.0) = 0;

  protected:

    /**
     * @brief Book a histogram for this Counter, to be filled in per-event monitoring.
     * @param[in] name The name of the histogram (and corresponding Variable)
     * @param[in] title ROOT histogram title string in format "title;xaxis;yaxis"
     * @param[in] xaxis Controls if the x-axis should use fixed-width or logarithmic bin boundaries.
     * @param[in] min X-axis minimum bound.
     * @param[in] max X-axis maximum bound.
     * @param[in] bins Number of histogram bins.
     */
    void regHistogram(const std::string& name,
      const std::string& title, 
      const VariableType type = VariableType::kPerCall, 
      const LogType xaxis = kLog,
      const float min = 0.1, 
      const float max = 10000.,
      const size_t bins = 50);

    /**
     * @brief Appends Counter name (to histogram path) and forwards histogram book request to parent Monitor.
     * @param[in] hist Bare pointer to histogram. Ownership transferred to THistSvc.
     * @param[in] tDir Histogram name & directory.
     * @return Cached pointer to histogram. Used to fill histogram without having to perform THishSvc lookup. 
     */
    TH1* bookGetPointer(TH1* hist, const std::string& tDir = "") const;

    /**
     * @brief Helper function. Compute the time difference in discreet microseconds and covert to fractional milliseconds.
     * @return Time difference in milliseconds, floating point.
     */
    float timeToMilliSec(const uint64_t start, const uint64_t stop) const;

  private:

    const std::string m_name; //!< Counter's name
    const MonitorBase* m_parent; //!< Counter's parent Monitor. Cached non-owning const pointer.
    std::unordered_map< std::string, Variable > m_variables; //!< Store of Counter's Variables. These wrap a histogram pointer. Keyed by name.
};

#endif // TRIGCOSTANALYSIS_COUNTERBASE_H
