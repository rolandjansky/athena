/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SysWriteSelectionHandle.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>
#include <SelectionHelpers/SelectionWriteAccessorNull.h>
#include <SystematicsHandles/SysListHandle.h>

//
// method implementations
//

namespace CP
{
  SysWriteSelectionHandle ::
  operator bool () const noexcept
  {
    return !m_selection.empty();
  }



  StatusCode SysWriteSelectionHandle ::
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
    ANA_CHECK (makeSelectionWriteAccessor (m_selection, m_accessor));
    return sysListHandle.addHandle (*this);
  }



  StatusCode SysWriteSelectionHandle ::
  initialize (SysListHandle& sysListHandle, const ISysHandleBase& objectHandle, SG::AllowEmptyEnum)
  {
    if (!empty())
      return initialize (sysListHandle, objectHandle);
    else
    {
      m_accessor = std::make_unique<SelectionWriteAccessorNull>();
      return StatusCode::SUCCESS;
    }
  }



  bool SysWriteSelectionHandle ::
  empty () const noexcept
  {
    return m_selection.empty();
  }



  std::string SysWriteSelectionHandle ::
  getNamePattern () const
  {
    // So far it is undefined what to return here.  I'll fill this in
    // once there is a reason for it to be one or the other.
    return "";
  }



  CP::SystematicSet SysWriteSelectionHandle ::
  getInputAffecting (const ISystematicsSvc& /*svc*/) const
  {
    return CP::SystematicSet ();
  }



  StatusCode SysWriteSelectionHandle ::
  fillSystematics (const ISystematicsSvc& svc,
                   const CP::SystematicSet& fullAffecting,
                   const std::vector<CP::SystematicSet>& sysList)
  {
    ANA_CHECK (m_accessor->fillSystematics (svc, fullAffecting, sysList, m_objectHandle->getNamePattern()));
    return StatusCode::SUCCESS;
  }
}
