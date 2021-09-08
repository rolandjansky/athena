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
#include <PATInterfaces/SystematicRegistry.h>
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



  const SysListType& SystematicsSvc ::
  systematicsVector () const
  {
    if (m_hasVector.load())
      return m_systematicsVector;
    std::lock_guard<std::mutex> lock (m_vectorMutex);
    if (m_hasVector.load())
      return m_systematicsVector;

    assert (m_systematicsVector.empty());

    if (m_sigmaRecommended != 0)
    {
      assert (m_sigmaRecommended > 0);

      const CP::SystematicSet recommended
        = CP::SystematicRegistry::getInstance().recommendedSystematics();

      CP::MakeSystematicsVector sys;
      sys.setSigma (m_sigmaRecommended);
      sys.calc (recommended);

      std::regex expr (m_systematicsRegex);
      for (const CP::SystematicSet& mysys : sys.result(""))
      {
        if (!regex_match (mysys.name(), expr))
        {
          ANA_MSG_INFO ("skipping systematic: " << mysys.name());
        } else {
          ANA_MSG_INFO ("configuring systematic: " << mysys.name());
          m_systematicsVector.push_back (mysys);
        }
      }
    } else if (m_systematicsList.empty())
    {
      // take an empty property as running just the central
      // systematics set.  implication is that you can't really run
      // doing nothing, but that ought to be Ok.
      m_systematicsVector.push_back (CP::SystematicSet ());
    } else
    {
      for (const std::string& sysName : m_systematicsList)
        m_systematicsVector.push_back (CP::SystematicSet (sysName));
    }

    assert (!m_systematicsVector.empty());
    m_hasVector = true;
    return m_systematicsVector;
  }



  StatusCode SystematicsSvc ::
  finalize ()
  {
    const CP::SystematicSet recommended
      = CP::SystematicRegistry::getInstance().recommendedSystematics();
    const CP::SystematicSet affecting
      = CP::SystematicRegistry::getInstance().globalSystematics();

    for (const CP::SystematicVariation& mysys : affecting)
    {
      // this logic checks whether a systematic is recommended and
      // affecting, or only affecting.  if it is only the later, it
      // reports the systematic in parenthesis to set it apart.
      if (recommended.find (mysys) == recommended.end())
        ANA_MSG_INFO ("found systematic: (" << mysys << ")");
      else
        ANA_MSG_INFO ("found systematic: " << mysys);
    }

    for (const CP::SystematicVariation& mysys : recommended)
    {
      if (affecting.find (mysys) == affecting.end())
      {
        ANA_MSG_WARNING ("systematic is only registered as recommended, not affecting: " << mysys);
      }
    }
    return StatusCode::SUCCESS;
  }
}
