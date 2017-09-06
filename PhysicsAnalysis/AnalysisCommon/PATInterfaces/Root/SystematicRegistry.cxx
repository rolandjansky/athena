// Author: Steve Farrell (steven.farrell@cern.ch)


//
// includes
//

// ROOT include(s):
#include <ThreadLocalStorage.h>

// Local include(s):
#include <PATInterfaces/SystematicRegistry.h>
#include <PATInterfaces/ISystematicsTool.h>
#include <PATInterfaces/SystematicSet.h>
#include <PATInterfaces/SystematicCode.h>

//
// method implementations
//

namespace CP
{

  // Get the singleton registry instance
  SystematicRegistry& SystematicRegistry::getInstance()
  {
    TTHREAD_TLS( SystematicRegistry ) systRegistry;
    return systRegistry;
  }


  const SystematicSet& SystematicRegistry::globalSystematics() const
  {
    return m_globalSystematics;
  }


  const SystematicSet& SystematicRegistry::recommendedSystematics() const
  {
    return m_recommendedSystematics;
  }


  SystematicCode SystematicRegistry ::
  registerSystematics (const ISystematicsTool& tool)
  {
    registerSystematics (tool.affectingSystematics());
    return addSystematicsToRecommended (tool.recommendedSystematics());
  }


  // Add a systematic to the global set
  void SystematicRegistry::registerSystematic
  (const SystematicVariation& systematic)
  {
    m_globalSystematics.insert(systematic);
  }

  // Add a set of systematics to the global set
  void SystematicRegistry::registerSystematics(const SystematicSet& systematics)
  {
    std::set<SystematicVariation>::const_iterator sysItr;
    for(sysItr = systematics.begin(); sysItr != systematics.end(); ++sysItr)
    {
      registerSystematic(*sysItr);
    }
  }


  // Add a systematic to the configuratin set
  SystematicCode SystematicRegistry::addSystematicToRecommended
  (const SystematicVariation& systematic)
  {
    // Check for consistency
    if(!m_globalSystematics.matchSystematic(systematic,
                                            SystematicSet::FULLORCONTINUOUS))
    {
      return SystematicCode::Unsupported;
    }
    m_recommendedSystematics.insert(systematic);
    return SystematicCode::Ok;
  }


  // Add a systematic set to the configuration set
  SystematicCode SystematicRegistry::addSystematicsToRecommended
  (const SystematicSet& systematics)
  {
    std::set<SystematicVariation>::const_iterator sysItr;
    for(sysItr = systematics.begin(); sysItr != systematics.end(); ++sysItr)
    {
      SystematicCode code = addSystematicToRecommended(*sysItr);
      if(code != SystematicCode::Ok) return code;
    }
    return SystematicCode::Ok;
  }


  // Private constructor
  SystematicRegistry::SystematicRegistry()
  {
  }

}
