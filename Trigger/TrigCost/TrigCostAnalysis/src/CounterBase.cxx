/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitoredRange.h"
#include "MonitorBase.h"
#include "CounterBase.h"
#include "TH1F.h"

CounterBase::CounterBase(const std::string& name, const MonitorBase* parent) 
  : m_name(name), m_parent(parent) {
}


const std::string& CounterBase::getName() const {
  return m_name;
}


const MonitorBase* CounterBase::getParent() const {
  return m_parent;
}


void CounterBase::regHistogram(const std::string& name, 
  const std::string& title,
  const VariableType type,
  const LogType xaxis, 
  const float min, 
  const float max,
  const size_t bins) 
{
  std::string hisSvcName = getParent()->getParent()->getName() + "_" + getParent()->getName() + "_" + getName() + "_" + name;
  std::unique_ptr<TH1F> hist;

  if (max <= min || bins == 0) {
    throw std::runtime_error("CounterBase::regHistogram: Cannot have max <= min or bins == 0");
  }

  if (xaxis == kLinear) {
    hist = std::make_unique<TH1F>(hisSvcName.c_str(), title.c_str(), bins, min, max);
  } else if (xaxis == kLog) {
    if (min <= 0) {
      throw std::runtime_error("CounterBase::regHistogram: Cannot have min <= 0 with log binning");
    }
    std::unique_ptr<double[]> xbins = std::make_unique<double[]>(bins+1);
    const double xlogmin = log10(min);
    const double xlogmax = log10(max);
    const double dlogx   = (xlogmax-xlogmin)/((double)bins);
    for (size_t i = 0; i <= bins; ++i) { 
      const double xlog = xlogmin + i*dlogx;
      xbins[i] = exp( log(10) * xlog ); 
    }
    hist = std::make_unique<TH1F>(hisSvcName.c_str(), title.c_str(), bins, xbins.get());
  } else {
    throw std::runtime_error("CounterBase::regHistogram: Unknown logarithm flag");
  }

  m_variables.emplace(std::piecewise_construct,
    std::forward_as_tuple(name),
    std::forward_as_tuple(name, bookGetPointer(hist.release()), type)
  );
  // Histogram is now owned by THistSvc. A cache of the ptr is kept in the Variable
}


Variable& CounterBase::getVariable(const std::string& name) {
  auto it = m_variables.find(name);
  if (it == m_variables.end()) {
    throw std::runtime_error( "CounterBase::getVariable: No varialbe with name " + name );
  }
  return (it->second);
}


StatusCode CounterBase::fill(const std::string& name, float value, float weight) {
  auto it = m_variables.find(name);
  if (it == m_variables.end()) {
    return StatusCode::FAILURE;
  }
  ATH_CHECK( it->second.fill(value, weight) );
  return StatusCode::SUCCESS;
}


StatusCode CounterBase::setDenominator(const std::string& name, float value) {
  auto it = m_variables.find(name);
  if (it == m_variables.end()) {
    return StatusCode::FAILURE;
  }
  it->second.setDenominator(value);
  return StatusCode::SUCCESS;
}


StatusCode CounterBase::increment(const std::string& name, float weight) {
  return fill(name, 1.0, weight);
}


StatusCode CounterBase::endEvent(float) {
  for (auto& nameVariablePair : m_variables) {
    ATH_CHECK( nameVariablePair.second.endEvent() );
  }
  return StatusCode::SUCCESS;
}


TH1* CounterBase::bookGetPointer(TH1* hist, const std::string& tDir) const {
  std::string dir = getName();
  if (tDir != "") {
    dir += "/";
    dir += tDir;
  }
  return getParent()->bookGetPointer(hist, dir);
}


float CounterBase::timeToMilliSec(const uint64_t start, const uint64_t stop) const {
  if (stop < start) {
    throw std::runtime_error("Asked for a stop time " + std::to_string(stop) + " which is before the start time " + std::to_string(start));
  }
  const uint64_t difference = stop - start;
  return (difference * 1e-3); // micro to ms
}
