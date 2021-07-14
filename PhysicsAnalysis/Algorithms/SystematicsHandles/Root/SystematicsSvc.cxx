/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SystematicsSvc.h>

#include <AsgMessaging/MessageCheck.h>
#include <PATInterfaces/MakeSystematicsVector.h>
#include <RootCoreUtils/StringUtil.h>
#include <cassert>
#include <cmath>
#include <regex>

//
// method implementations
//

namespace CP
{
  SystematicsSvc ::
  SystematicsSvc (const std::string& name,
                    ISvcLocator* pSvcLocator)
    : AsgService (name, pSvcLocator)
  {
    declareProperty ("systematicsList", m_systematicsList, "the list of systematics to run");
    declareProperty ("systematicsRegex", m_systematicsRegex, "systematics filter regex");
    declareProperty ("sigmaRecommended", m_sigmaRecommended, "the sigma with which to run recommended systematics");
    declareProperty ("nominalSystematicsName", m_nominalSystematicsName, "the name to use for the nominal systematic (instead of the empty string)");

    declareServiceInterface<ISystematicsSvc>();
  }



  StatusCode SystematicsSvc ::
  initialize ()
  {
    if (!std::isfinite (m_sigmaRecommended) ||
        m_sigmaRecommended < 0)
    {
      ANA_MSG_ERROR ("invalid value for sigmaRecommended: " << m_sigmaRecommended);
      return StatusCode::FAILURE;
    }

    if (m_sigmaRecommended != 0)
    {
      if (!m_systematicsList.empty())
      {
        ANA_MSG_ERROR ("can't specify both sigmaRecommended and systematicsList");
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }



  std::vector<CP::SystematicSet> SystematicsSvc ::
  makeSystematicsVector () const
  {
    std::lock_guard<std::mutex> lock (m_systematicsMutex);
    std::vector<CP::SystematicSet> systematicsVector;

    if (m_sigmaRecommended != 0)
    {
      assert (m_sigmaRecommended > 0);

      CP::MakeSystematicsVector sys;
      sys.setSigma (m_sigmaRecommended);
      sys.calc (m_recommendedSystematics);

      std::regex expr (m_systematicsRegex);
      for (const CP::SystematicSet& mysys : sys.result(""))
      {
        if (!regex_match (mysys.name(), expr))
        {
          ANA_MSG_DEBUG ("skipping systematic: " << mysys.name());
        } else {
          ANA_MSG_DEBUG ("configuring systematic: " << mysys.name());
          systematicsVector.push_back (mysys);
        }
      }
    } else if (m_systematicsList.empty())
    {
      // take an empty property as running just the central
      // systematics set.  implication is that you can't really run
      // doing nothing, but that ought to be Ok.
      systematicsVector.push_back (CP::SystematicSet ());
    } else
    {
      for (const std::string& sysName : m_systematicsList)
        systematicsVector.push_back (CP::SystematicSet (sysName));
    }

    assert (!systematicsVector.empty());
    return systematicsVector;
  }



  StatusCode SystematicsSvc ::
  addSystematics (const CP::SystematicSet& recommended,
                  const CP::SystematicSet& affecting) const
  {
    std::lock_guard<std::mutex> lock (m_systematicsMutex);

    for (const CP::SystematicVariation& mysys : recommended)
    {
      if (affecting.find (mysys) == affecting.end())
      {
        ANA_MSG_ERROR ("systematic is only registered as recommended, not affecting: " << mysys);
        return StatusCode::FAILURE;
      }
    }

    m_affectingSystematics.insert (affecting);
    m_recommendedSystematics.insert (recommended);
    return StatusCode::SUCCESS;
  }



  CP::SystematicSet SystematicsSvc ::
  getObjectSystematics (const std::string& name) const
  {
    std::lock_guard<std::mutex> lock (m_systematicsMutex);
    auto iter = m_objectSystematics.find (name);
    if (iter != m_objectSystematics.end())
      return iter->second;
    else
      return CP::SystematicSet ();
  }



  StatusCode SystematicsSvc ::
  setObjectSystematics (const std::string& name,
                        const CP::SystematicSet& systematics) const
  {
    auto mysystematics = systematics;
    std::lock_guard<std::mutex> lock (m_systematicsMutex);
    for (const CP::SystematicVariation& mysys : systematics)
    {
      if (m_affectingSystematics.find (mysys) == m_affectingSystematics.end())
      {
        ANA_MSG_ERROR ("systematic is set as object systematic, but not affecting: " << mysys);
        return StatusCode::FAILURE;
      }
    }
    m_objectSystematics[name] = std::move (mysystematics);
    return StatusCode::SUCCESS;
  }



  CP::SystematicSet SystematicsSvc ::
  getDecorSystematics (const std::string& objectName,
                       const std::string& decorName) const
  {
    const std::string name = objectName + "." + decorName;
    std::unique_lock<std::mutex> lock (m_systematicsMutex);
    auto iter = m_decorSystematics.find (name);
    if (iter != m_decorSystematics.end())
      return iter->second;
    auto jter = m_copies.find (objectName);
    if (jter != m_copies.end())
    {
      const std::string& copyName = jter->second;
      lock.unlock();
      return getDecorSystematics (copyName, decorName);
    }
    return CP::SystematicSet ();
  }



  StatusCode SystematicsSvc ::
  setDecorSystematics (const std::string& objectName,
                       const std::string& decorName,
                       const CP::SystematicSet& systematics) const
  {
    const std::string name = objectName + "." + decorName;
    auto mysystematics = systematics;
    std::lock_guard<std::mutex> lock (m_systematicsMutex);
    for (const CP::SystematicVariation& mysys : systematics)
    {
      if (m_affectingSystematics.find (mysys) == m_affectingSystematics.end())
      {
        ANA_MSG_ERROR ("systematic is set as object systematic, but not affecting: " << mysys);
        return StatusCode::FAILURE;
      }
    }
    m_decorSystematics[name] = std::move (mysystematics);
    return StatusCode::SUCCESS;
  }



  StatusCode SystematicsSvc ::
  registerCopy (const std::string& fromName,
                const std::string& toName) const
  {
    std::lock_guard<std::mutex> lock (m_systematicsMutex);
    auto emplace_result = m_copies.emplace (std::make_pair (toName, fromName));
    if (emplace_result.second == false)
    {
      ANA_MSG_ERROR ("duplicate copy registered for name " << toName);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode SystematicsSvc ::
  makeSystematicsName (std::string& result,
                       const std::string& name,
                       const CP::SystematicSet& sys) const
  {
    if (name.empty())
    {
      ANA_MSG_ERROR ("not allowed to make systematic name for empty string");
      return StatusCode::FAILURE;
    }

    result = name;

    const auto sysSplit = result.find ("%SYS%");
    if (sysSplit == std::string::npos)
    {
      if (!sys.empty())
      {
        ANA_MSG_ERROR ("can't set systematics on name without \"%SYS%\": \"" << name << "\" sys=" << sys.name());
        return StatusCode::FAILURE;
      }
    } else
    {
      std::string sysName = sys.name();
      if (sysName.empty())
        sysName = m_nominalSystematicsName;
      result.replace (sysSplit, 5, sysName);
    }

    return StatusCode::SUCCESS;
  }



  StatusCode SystematicsSvc ::
  finalize ()
  {
    for (const CP::SystematicVariation& mysys : m_affectingSystematics)
    {
      // this logic checks whether a systematic is recommended and
      // affecting, or only affecting.  if it is only the later, it
      // reports the systematic in parenthesis to set it apart.
      if (m_recommendedSystematics.find (mysys) == m_recommendedSystematics.end())
        ANA_MSG_INFO ("found systematic: (" << mysys << ")");
      else
        ANA_MSG_INFO ("found systematic: " << mysys);
    }
    return StatusCode::SUCCESS;
  }
}
