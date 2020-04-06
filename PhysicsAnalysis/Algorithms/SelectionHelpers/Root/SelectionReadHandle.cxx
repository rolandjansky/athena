/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionReadHandle.h>

#include <AsgTools/MessageCheck.h>
#include <AsgTools/StatusCode.h>

//
// method implementations
//

namespace CP
{
  MsgStream& SelectionReadHandle ::
  msg () const
  {
    assert (m_msg != nullptr);
    return *m_msg;
  }



  MsgStream& SelectionReadHandle ::
  msg (MSG::Level lvl) const
  {
    assert (m_msg != nullptr);
    *m_msg << lvl;
    return *m_msg;
  }



  bool SelectionReadHandle ::
  empty () const noexcept
  {
    return m_selection.empty();
  }



  SelectionReadHandle ::
  operator bool () const noexcept
  {
    return !m_selection.empty();
  }



  StatusCode SelectionReadHandle ::
  initialize ()
  {
    ANA_CHECK (makeSelectionAccessor (m_selection, m_accessor));
    return StatusCode::SUCCESS;
  }
}
