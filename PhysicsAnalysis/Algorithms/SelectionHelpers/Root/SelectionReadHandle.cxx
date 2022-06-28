/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionReadHandle.h>

#include <AsgMessaging/StatusCode.h>

//
// method implementations
//

namespace CP
{
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
    return makeSelectionReadAccessor (m_selection, m_accessor);
  }
}
