// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ATHENAKERNEL_RNGWRAPPER_H
#define ATHENAKERNEL_RNGWRAPPER_H

#include <string>
#include <cstdint>
#include <vector>
#include <functional>

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

namespace CLHEP{
  class HepRandomEngine;
}

namespace ATHRNG{

  /// @class RNGWrapper
  /// @brief A wrapper class for event-slot-local random engines.
  ///
  /// Implemented primarily for use in services implementing IAthRNGSvc
  /// for thread-safe random number generation in AthenaMT.
  ///
  /// The slot-local HepRandomEngine is retrieved via the dereference operator
  /// or the getEngine method.
  ///
  /// This class also provides a seed setting and generating mechanism. The
  /// seed is calculated from an algorithm name and the slot, event, and run
  /// numbers.
  ///
  /// @author Sami Kama <sami.kama@cern.ch>
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class RNGWrapper
  {

    typedef std::function<CLHEP::HepRandomEngine*(void)> factoryFunc;

  public:

    /// Constructor takes a factory function which can instantiate a
    /// CLHEP::HepRandomEngine and the number of event slots.
    RNGWrapper(factoryFunc& genFact, size_t nSlots);
    ~RNGWrapper();

    /// Set the random seed using a string (e.g. algorithm name) and the
    /// current EventContext. Does nothing if the contex is invalid.
    void setSeed(const std::string& algName, const EventContext& ctx);

    /// Set the random seed using a string (e.g. algorithm name) and the
    /// current slot, event, and run numbers.
    void setSeed(const std::string& algName, size_t slot, uint64_t ev, uint64_t run);

    /// Cast-to-engine pointer operator.
    /// Retrieves the current event context and returns the engine
    /// corresponding to the current event slot.
    operator CLHEP::HepRandomEngine*() const {
      return m_engines[Gaudi::Hive::currentContext().slot()];
    }

    /// Retrieve the random engine corresponding to the provided EventContext.
    CLHEP::HepRandomEngine* getEngine(const EventContext &ctx) const {
      return m_engines[ctx.slot()];
    }

  private:

    /// Vector of random engines, ordered by slot number.
    std::vector<CLHEP::HepRandomEngine*> m_engines;

  };

}

#endif
