/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysListHandle.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>
#include <PATInterfaces/IReentrantSystematicsTool.h>
#include <PATInterfaces/SystematicSet.h>
#include <SystematicsHandles/ISysHandleBase.h>
#include <regex>
#include <unordered_set>

//
// method implementations
//

namespace CP
{
  StatusCode SysListHandle ::
  addHandle (ISysHandleBase& handle)
  {
    if (isInitialized())
    {
      ANA_MSG_ERROR ("trying to add/initialize data handle after initializing SysListHandle");
      return StatusCode::FAILURE;
    }
    if (handle.empty())
    {
      ANA_MSG_ERROR ("trying to add/initialize empty data handle");
      return StatusCode::FAILURE;
    }
    m_sysHandles.push_back (&handle);
    return StatusCode::SUCCESS;
  }



  StatusCode SysListHandle ::
  addSystematics (const CP::SystematicSet& recommended,
                  const CP::SystematicSet& affecting)
  {
    assert (!isInitialized());
    ANA_CHECK (m_systematicsService->addSystematics (recommended, affecting));
    m_affecting.insert (affecting);
    return StatusCode::SUCCESS;
  }



  StatusCode SysListHandle ::
  addSystematics (const IReentrantSystematicsTool& tool)
  {
    return addSystematics (tool.recommendedSystematics(),
                           tool.affectingSystematics());
  }



  ::StatusCode SysListHandle ::
  initialize ()
  {
    for (ISysHandleBase *handle : m_sysHandles)
      m_affecting.insert (handle->getInputAffecting (*m_systematicsService));
    ANA_CHECK (fillSystematicsVector());
    for (ISysHandleBase *handle : m_sysHandles)
      ANA_CHECK (handle->fillSystematics (*m_systematicsService, m_affecting, m_systematicsVector));
    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  const std::vector<CP::SystematicSet>& SysListHandle ::
  systematicsVector () const
  {
    assert (isInitialized());
    return m_systematicsVector;
  }



  StatusCode SysListHandle ::
  fillSystematicsVector ()
  {
    assert (m_systematicsVector.empty());

    std::unordered_set<CP::SystematicSet> knownSys;

    for (const auto& sys : m_systematicsService->makeSystematicsVector())
    {
      CP::SystematicSet filtered;
      ANA_CHECK (SystematicSet::filterForAffectingSystematics (sys, m_affecting, filtered));
      if (knownSys.find (filtered) == knownSys.end())
      {
        m_systematicsVector.push_back (filtered);
        knownSys.insert (filtered);
      }
    }
    return StatusCode::SUCCESS;
  }



  StatusCode SysListHandle ::
  foreach (const std::function<StatusCode(const CP::SystematicSet&)>& func) const
  {
    assert (isInitialized());

    for (const CP::SystematicSet& sys : systematicsVector())
    {
      if (func (sys).isFailure())
      {
        ANA_MSG_ERROR ("failed to evaluate algorithm for systematic " << sys.name());
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }
}
