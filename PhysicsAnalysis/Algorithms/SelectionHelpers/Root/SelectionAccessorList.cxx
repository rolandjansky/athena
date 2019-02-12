/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorList.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorList ::
  SelectionAccessorList (std::vector<std::unique_ptr<ISelectionAccessor> > val_list)
    : m_list (std::move (val_list))
  {}



  SelectionType SelectionAccessorList ::
  getBits (const SG::AuxElement& element) const
  {
    // total number of bits in SelectionType
    constexpr size_t SelectionTotalBits = 8 * sizeof (SelectionType);

    // if we have more cuts than bits, just return true/false
    if (m_list.size() > SelectionTotalBits)
    {
      if (getBool (element))
        return selectionAccept();
      else
        return 0;
    }

    SelectionType result = selectionAccept();
    for (std::size_t iter = 0; iter != m_list.size(); ++ iter)
    {
      if (m_list[iter]->getBool (element) == false)
      {
        result = result & ~(SelectionType (1) << iter);
      }
    }
    return result;
  }



  void SelectionAccessorList ::
  setBits (SG::AuxElement& element,
           SelectionType selection) const
  {
    // technically we could support setting by setting all the
    // components, but I can't think of a situation in which that
    // would be a good idea
    throw std::runtime_error ("setting not supported for CP::SelectionAccessorList");
  }



  bool SelectionAccessorList ::
  getBool (const SG::AuxElement& element) const
  {
    for (auto& accessor : m_list)
    {
      if (!accessor->getBool (element))
        return false;
    }
    return true;
  }



  void SelectionAccessorList ::
  setBool (SG::AuxElement& /*element*/,
           bool /*selection*/) const
  {
    // technically we could support setting by setting all the
    // components, but I can't think of a situation in which that
    // would be a good idea
    throw std::runtime_error ("setting not supported for CP::SelectionAccessorList");
  }
}
