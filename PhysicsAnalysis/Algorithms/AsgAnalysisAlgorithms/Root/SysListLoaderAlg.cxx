/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <regex>

#include <AsgAnalysisAlgorithms/SysListLoaderAlg.h>

#include <AsgTools/MessageCheck.h>
#include <PATInterfaces/MakeSystematicsVector.h>
#include <PATInterfaces/SystematicRegistry.h>

//
// method implementations
//

namespace CP
{
  SysListLoaderAlg :: 
  SysListLoaderAlg (const std::string& name, 
                    ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("systematicsName", m_systematicsName, "the name of the systematics in the event store");
    declareProperty ("systematicsList", m_systematicsList, "the list of systematics to run");
    declareProperty ("systematicsRegex", m_systematicsRegex, "systematics filter regex");
    declareProperty ("sigmaRecommended", m_sigmaRecommended, "the sigma with which to run recommended systematics");
  }



  StatusCode SysListLoaderAlg ::
  initialize ()
  {
    if (m_systematicsName.empty())
    {
      ANA_MSG_ERROR ("no name configured for the systematics list");
      return StatusCode::FAILURE;
    }

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
    return StatusCode::SUCCESS;
  }



  StatusCode SysListLoaderAlg ::
  execute ()
  {
    if (m_firstEvent)
    {
      m_firstEvent = false;
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
      bool error = false;
      for (const CP::SystematicVariation& mysys : recommended)
      {
        if (affecting.find (mysys) == affecting.end())
        {
          ANA_MSG_ERROR ("systematic is only registered as recommended, not affecting: " << mysys);
          error = true;
        }
      }
      if (error)
        return StatusCode::FAILURE;


      if (m_systematicsVector.empty())
      {
        assert (m_sigmaRecommended > 0);
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
      }
    }

    std::unique_ptr<SysListType> list (new SysListType (m_systematicsVector));
    ANA_CHECK (evtStore()->record (list.release(), m_systematicsName));
    return StatusCode::SUCCESS;
  }
}
