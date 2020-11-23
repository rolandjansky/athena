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
#include <AsgMessaging/StatusCode.h>

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


  StatusCode SystematicRegistry ::
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
  StatusCode SystematicRegistry::addSystematicToRecommended
  (const SystematicVariation& systematic)
  {
    // Check for consistency
    if(!m_globalSystematics.matchSystematic(systematic,
                                            SystematicSet::FULLORCONTINUOUS))
    {
      return StatusCode::FAILURE;
    }
    m_recommendedSystematics.insert(systematic);
    return StatusCode::SUCCESS;
  }


  // Add a systematic set to the configuration set
  StatusCode SystematicRegistry::addSystematicsToRecommended
  (const SystematicSet& systematics)
  {
    std::set<SystematicVariation>::const_iterator sysItr;
    for(sysItr = systematics.begin(); sysItr != systematics.end(); ++sysItr)
    {
      StatusCode code = addSystematicToRecommended(*sysItr);
      if(code != StatusCode::SUCCESS) return code;
    }
    return StatusCode::SUCCESS;
  }


  // Private constructor
  SystematicRegistry::SystematicRegistry()
  {
  }

}
