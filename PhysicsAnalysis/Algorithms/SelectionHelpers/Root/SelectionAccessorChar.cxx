/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorChar.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorChar ::
  SelectionAccessorChar (const std::string& name)
    : m_accessor (name)
  {}



  SelectionType SelectionAccessorChar ::
  getBits (const SG::AuxElement& element) const
  {
    if (m_accessor (element))
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorChar ::
  setBits (SG::AuxElement& element,
           SelectionType selection) const
  {
    if (selection == selectionAccept())
      m_accessor (element) = 1;
    else
      m_accessor (element) = 0;
  }



  bool SelectionAccessorChar ::
  getBool (const SG::AuxElement& element) const
  {
    return m_accessor (element);
  }



  void SelectionAccessorChar ::
  setBool (SG::AuxElement& element,
           bool selection) const
  {
    m_accessor (element) = selection;
  }
}
