/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_VARIABLE_H
#define TRIGCOSTANALYSIS_VARIABLE_H 1

#include "GaudiKernel/StatusCode.h"
#include "AthenaBaseComps/AthCheckMacros.h"

class TH1; //<! Forward declaration

/**
 * @enum VariableType
 * @brief Behaviour of Variable
 */
enum VariableType { 
  kPerCall, //!< Variable should fill underlying histogram on each fill
  kPerEvent //!< Variable should buffer fill calls in an accumulator and fill the underlying histogram once at the end of the event.
};

/**
 * @enum LogType
 * @brief Histogram x-axis type flag
 */
enum LogType { 
  kLinear, //!< Linear x-binning
  kLog  //!< Logarithmic x-binning
};

/**
 * @class Variable
 * @brief Wrapper around a histogram which allows for some additional filling patterns and data manipulation. 
 *
 * Behaviour based on type parameter. per-Call directly fills underlying histogram for each call of fill. per-Event
 * fills the underlying histogram exactly once in events where fill was called one or more times. The filled value
 * is accumulated from all calls to fill during the event in per-Event mode.
 */
class Variable {
  public:
    /**
     * @brief Forbid default constructor.
     */
    Variable() = delete;

    /**
     * @brief Construct Variable.
     * @param[in] name Variable's name
     * @param[in] cacheHistoPtr Cached non-owning pointer to the histogram which this Variable wraps.
     * @param[in] type If the Variable should operate in per-Call mode (direct fill) or per-Event mode (accumulate and fill at the end of the event).
     */
    Variable(const std::string& name, TH1* cacheHistoPtr, VariableType type = kPerCall);

    /**
     * @brief Default destructor.
     */
    ~Variable() = default;

    /**
     * @brief Forbid copy.
     */
    Variable& operator=(const Variable&) = delete;

    /**
     * @brief Forbid copy.
     */
    Variable(const Variable&) = delete;

    /**
     * @brief Getter for Variable's name
     * @return Variable's name const string reference.
     */
    const std::string& getName() const;

    /**
     * @brief Getter for how many times fill() has already been called on this Variable in this event.
     * @return Number of fill calls this event.
     */
    size_t getCalls() const;

    /**
     * @brief Getter for accumulated value of a kPerEvent Variable
     * @return Total accumulated payload
     */
    float getAccumulator() const;

    /**
     * @brief Fill histogram (per-Call Variable), or add value to internal accumulator (per-Event Variable) to be filled at the end of the event.
     * @pram[in] value The payload.     
     * @param[in] weight Global event weight
     */
    StatusCode fill(float value, float weight = 1.0);

    /**
     * @brief Convenience function. Equivalent of fill(1.0, weight). For use with per-Event counting type variables.
     * @param[in] weight Global event weight
     */
    StatusCode increment(float weight = 1.0);

    /**
     * @brief Sets, until the end of the event, a denominator which will be used to normalise every Fill.
     * @pram[in] value The denominator to normalise Fill operations.
     */
    void setDenominator(float value);

    /**
     * @brief Called by the framework. Triggers actual histogram fill for a per-Event variable. Resets internals.
     */
    StatusCode endEvent();

  private:

    const std::string m_name; //<! Variable's name
    const VariableType m_variableType; //<! Variable's type enumeration
    TH1* m_cacheHistoPtr; //<! Mutable cached non-owning ptr to this Variable's histogram.
    size_t m_calls; //<! Counter of how many times the Variable is Filled in an event.
    float m_accumulator; //<! For per-Event quantities, the accumulator buffers until the final quantity is known.
    float m_weight; //!< Cache of the event weight. Assumed to be the same for every call to fill with per-Event monitoring!
    float m_oneOverDenominator; //!< Cache of the reciprocal of the denominator used to normalise when filling the histogram.
};

#endif // TRIGCOSTANALYSIS_VARIABLE_H
