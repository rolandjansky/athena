#ifndef PATINTERFACES_SYSTEMATIC_REGISTRY_H
#define PATINTERFACES_SYSTEMATIC_REGISTRY_H

// Author: Steve Farrell (steven.farrell@cern.ch)

// This module implements the central registry for handling systematic
// uncertainties with CP tools.

#include <PATInterfaces/Global.h>
#include <PATInterfaces/SystematicSet.h>


namespace CP
{

  class SystematicRegistry
  {

  public:
    /// Get the singleton instance of the registry
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
    // Sets of systematics in the registry

    /// Global set of possible systematics
    SystematicSet m_globalSystematics;

    /// recommended set of systematics
    SystematicSet m_recommendedSystematics;

  private:
    // Disallow copying of the registry.
    // These will not be implemented

    /// Private copy constructor
    SystematicRegistry(SystematicRegistry const&);
    /// Private assignment operator
    void operator=(SystematicRegistry const&);

  };

}


#endif
