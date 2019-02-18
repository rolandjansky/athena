/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorInvert.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorInvert ::
  SelectionAccessorInvert (std::unique_ptr<ISelectionAccessor> val_base)
    : m_base (std::move (val_base))
  {}



  SelectionType SelectionAccessorInvert ::
  getBits (const SG::AuxElement& element) const
  {
    if (m_base->getBool (element) == false)
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorInvert ::
  setBits (SG::AuxElement& element,
           SelectionType selection) const
  {
    m_base->setBool (element, selection != selectionAccept());
  }



  bool SelectionAccessorInvert ::
  getBool (const SG::AuxElement& element) const
  {
    return m_base->getBool (element) == false;
  }



  void SelectionAccessorInvert ::
  setBool (SG::AuxElement& element,
           bool selection) const
  {
    m_base->setBool (element, !selection);
  }
}
