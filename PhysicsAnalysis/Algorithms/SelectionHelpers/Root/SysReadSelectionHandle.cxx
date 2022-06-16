/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SysReadSelectionHandle.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>
#include <SelectionHelpers/SelectionReadAccessorNull.h>
#include <SystematicsHandles/SysListHandle.h>

//
// method implementations
//

namespace CP
{
  SysReadSelectionHandle ::
  operator bool () const noexcept
  {
    return !m_selection.empty();
  }



  StatusCode SysReadSelectionHandle ::
  initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle)
  {
    if (m_selection.empty())
    {
      ANA_MSG_ERROR ("trying to initialize empty handle");
      return StatusCode::FAILURE;
    }
    if (m_objectHandle != nullptr)
    {
      ANA_MSG_ERROR ("trying to initialize handle twice");
      return StatusCode::FAILURE;
    }
    m_objectHandle = &objectHandle;
    ANA_CHECK (makeSelectionReadAccessor (m_selection, m_accessor));
    return sysListHandle.addHandle (*this);
  }



  StatusCode SysReadSelectionHandle ::
  initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle, SG::AllowEmptyEnum)
  {
    if (!empty())
      return initialize (sysListHandle, objectHandle);
    else
    {
      m_accessor = std::make_unique<SelectionReadAccessorNull>(true);
      return StatusCode::SUCCESS;
    }
  }



  bool SysReadSelectionHandle ::
  empty () const noexcept
  {
    return m_selection.empty();
  }



  std::string SysReadSelectionHandle ::
  getNamePattern () const
  {
    // So far it is undefined what to return here.  I'll fill this in
    // once there is a reason for it to be one or the other.
    return "";
  }



  CP::SystematicSet SysReadSelectionHandle ::
  getInputAffecting (const ISystematicsSvc& svc) const
  {
    return m_accessor->getInputAffecting (svc, m_objectHandle->getNamePattern());
  }



  StatusCode SysReadSelectionHandle ::
  fillSystematics (const ISystematicsSvc& svc,
                   const CP::SystematicSet& /*fullAffecting*/,
                   const std::vector<CP::SystematicSet>& sysList)
  {
    ANA_CHECK (m_accessor->fillSystematics (svc, sysList, m_objectHandle->getNamePattern()));
    return StatusCode::SUCCESS;
  }
}
