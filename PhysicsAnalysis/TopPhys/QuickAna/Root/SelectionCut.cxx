/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/SelectionCut.h>

#include <QuickAna/SelectionChoice.h>
#include <QuickAna/SelectionData.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace ana
{
  void SelectionCut ::
  testInvariant () const
  {
    RCU_INVARIANT (this != nullptr);
  }



  SelectionCut ::
  SelectionCut ()
    : m_data (nullptr), m_index (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  SelectionChoice SelectionCut ::
  get () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (m_data, "cut has been configured");
    return m_data->getCut (m_index);
  }



  bool SelectionCut ::
  getPass () const
  {
    // no invariant used
    return get() != SelectionChoice::FAIL;
  }



  bool SelectionCut ::
  getFail () const
  {
    // no invariant used
    return get() == SelectionChoice::FAIL;
  }



  void SelectionCut ::
  set (SelectionChoice choice) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (m_data, "cut has been configured");
    m_data->setCut (m_index, choice);
  }



  void SelectionCut ::
  setPassedIf (bool pass) const
  {
    // no invariant used
    set (pass ? SelectionChoice::PASS : SelectionChoice::FAIL);
  }



  void SelectionCut ::
  setFailedIf (bool fail) const
  {
    // no invariant used
    set (fail ? SelectionChoice::FAIL : SelectionChoice::PASS);
  }



  void SelectionCut ::
  configure (SelectionData *val_data, std::size_t val_index)
  {
    RCU_CHANGE_INVARIANT (this);
    m_data = val_data;
    m_index = val_index;
  }
}
