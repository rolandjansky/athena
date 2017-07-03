// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PATINTERFACES_SYSTEMATIC_REGISTRY_H
#define PATINTERFACES_SYSTEMATIC_REGISTRY_H

// Local include(s):
#include <PATInterfaces/Global.h>
#include <PATInterfaces/SystematicSet.h>

namespace CP
{

  /// This module implements the central registry for handling systematic
  /// uncertainties with CP tools.
  ///
  /// @author Steve Farrell <steven.farrell@cern.ch>
  ///
  class SystematicRegistry
  {

  public:
    /// Get the singleton instance of the registry for the curren thread
    ///
    /// Note that each thread gets its own instance of SystematicRegistry in
    /// the current setup. Assuming that all analysis tools get instantiated
    /// once per thread.
    ///
    /// This will have to be re-visited once we start making analysis tools
    /// thread-safe.
    ///
    /// @return A thread specific SystematicRegistry instance
    ///
    static SystematicRegistry& getInstance();

  public:
    // Public accessors of systematic sets

    /// returns: the global set of systematics
    const SystematicSet& globalSystematics() const;

    /// returns: the recommended set of systematics
    const SystematicSet& recommendedSystematics() const;

  public:
    // Interface methods for editing the registry

    /// effects: register all the systematics from the tool
    SystematicCode registerSystematics (const ISystematicsTool& tool);

    /// description: add a systematic to the global registry set
    void registerSystematic(const SystematicVariation& systematic);

    /// description: add a set of systematics to the global registry set
    /// failures: requesting a systematic that's not on the global list
    void registerSystematics(const SystematicSet& systematics);

    /// description: add a systematic to the recommended set
    SystematicCode addSystematicToRecommended(const SystematicVariation& systematic);

    /// description: add a set of systematics to the recommended set
    SystematicCode addSystematicsToRecommended(const SystematicSet& systematics);

  private:
    /// Private constructor
    SystematicRegistry();

  private:
    /// @name Sets of systematics in the registry
    /// @{

    /// Global set of possible systematics
    SystematicSet m_globalSystematics;

    /// recommended set of systematics
    SystematicSet m_recommendedSystematics;

    /// @}

  private:
    // Disallow copying of the registry.
    // These will not be implemented

    /// Private copy constructor
    SystematicRegistry(SystematicRegistry const&) = delete;
    /// Private assignment operator
    void operator=(SystematicRegistry const&) = delete;

  };

}


#endif
