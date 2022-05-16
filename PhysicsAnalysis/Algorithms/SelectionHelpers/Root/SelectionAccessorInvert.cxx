/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorInvert.h>

#include <PATInterfaces/SystematicSet.h>

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
  getBits (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    if (m_base->getBool (element, sys) == false)
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorInvert ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet *sys) const
  {
    m_base->setBool (element, selection != selectionAccept(), sys);
  }



  bool SelectionAccessorInvert ::
  getBool (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    return m_base->getBool (element, sys) == false;
  }



  void SelectionAccessorInvert ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet *sys) const
  {
    m_base->setBool (element, !selection, sys);
  }



  std::string SelectionAccessorInvert ::
  label () const
  {
    return "not " + m_base->label();
  }



  CP::SystematicSet SelectionAccessorInvert ::
  getInputAffecting (const ISystematicsSvc& svc,
                     const std::string& objectName) const
  {
    return m_base->getInputAffecting (svc, objectName);
  }



  StatusCode SelectionAccessorInvert ::
  fillSystematics (const ISystematicsSvc& svc,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName)
  {
    return m_base->fillSystematics (svc, sysList, objectName);
  }
}
