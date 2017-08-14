// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RNGCOMPS_ATHRNGSVC_H
#define RNGCOMPS_ATHRNGSVC_H

#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <functional>

#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

namespace ATHRNG {
  class RNGWrapper;
}
namespace CLHEP{
  class HepRandomEngine;
}

/// @class AthRNGSvc
/// @brief A service to manage multiple RNG streams in thread-safe way.
///
/// The random engines are provided via the RNGWrapper which dereferences to
/// the appropriate slot-local engine.
///
/// @todo Move from manual pointer management to smart pointer management.
///
class AthRNGSvc : public extends<AthService, IAthRNGSvc, IIncidentListener>
{

public:

  /// Standard constructor
  AthRNGSvc(const std::string& name, ISvcLocator* svc);
  virtual ~AthRNGSvc();

  /// Initialize the service; this is not where the RNGs are constructed.
  StatusCode initialize() override final;
  /// Gaudi "start" state is where we construct the RNGs.
  StatusCode start() override final;
  /// Nothing currently happens in finalization.
  StatusCode finalize() override final;

  /// IAthRNGSvc method to retrieve the random number wrapper.
  virtual ATHRNG::RNGWrapper* getEngine(const std::string& streamName) override final;

  /// Incident handling method, where we reseed the engine of this slot.
  virtual void handle( const Incident& incident );

private:

  /// The structure for storing the RNGWrappers.
  std::unordered_map<std::string, ATHRNG::RNGWrapper*> m_wrappers;

  /// Mutex for protecting access to the wrapper structure.
  std::mutex m_mutex;

  /// @name configurable properties
  /// @{
  std::string m_RNGType;
  std::vector<std::string> m_seeds;
  /// @}

  std::size_t m_numSlots;
  bool m_initialized;
  typedef std::function<CLHEP::HepRandomEngine*(void)> factoryFunc;
  factoryFunc m_fact;
};

#endif
