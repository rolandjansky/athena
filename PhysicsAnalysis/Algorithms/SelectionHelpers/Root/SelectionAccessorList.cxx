/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorList.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorList ::
  SelectionAccessorList (std::vector<std::unique_ptr<ISelectionReadAccessor> > val_list)
    : m_list (std::move (val_list))
  {
    for (const std::unique_ptr<ISelectionReadAccessor> &acc : m_list)
    {
      if (!m_label.empty())
        m_label.append(" && ");
      
      m_label.append(acc->label());
    }

    m_label = "( " + m_label + " )";
  }



  SelectionType SelectionAccessorList ::
  getBits (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    // total number of bits in SelectionType
    constexpr size_t SelectionTotalBits = 8 * sizeof (SelectionType);

    // if we have more cuts than bits, just return true/false
    if (m_list.size() > SelectionTotalBits)
    {
      if (getBool (element, sys))
        return selectionAccept();
      else
        return 0;
    }

    SelectionType result = selectionAccept();
    for (std::size_t iter = 0; iter != m_list.size(); ++ iter)
    {
      if (m_list[iter]->getBool (element, sys) == false)
      {
        result = result & ~(SelectionType (1) << iter);
      }
    }
    return result;
  }



  bool SelectionAccessorList ::
  getBool (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    for (auto& accessor : m_list)
    {
      if (!accessor->getBool (element, sys))
        return false;
    }
    return true;
  }


  std::string SelectionAccessorList ::
  label () const
  {
    return m_label;
  }



  CP::SystematicSet SelectionAccessorList ::
  getInputAffecting (const ISystematicsSvc& svc,
                     const std::string& objectName) const
  {
    CP::SystematicSet result;

    for (auto& base : m_list)
      result.insert (base->getInputAffecting (svc, objectName));
    return result;
  }



  StatusCode SelectionAccessorList ::
  fillSystematics (const ISystematicsSvc& svc,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName)
  {
    using namespace msgSelectionHelpers;

    for (auto& base : m_list)
      ANA_CHECK (base->fillSystematics (svc, sysList, objectName));
    return StatusCode::SUCCESS;
  }
}
