/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionReadAccessorInvert.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionReadAccessorInvert ::
  SelectionReadAccessorInvert (std::unique_ptr<ISelectionReadAccessor> val_base)
    : m_base (std::move (val_base))
  {}



  SelectionType SelectionReadAccessorInvert ::
  getBits (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    if (m_base->getBool (element, sys) == false)
      return selectionAccept();
    else
      return 0;
  }



  bool SelectionReadAccessorInvert ::
  getBool (const SG::AuxElement& element,
           const CP::SystematicSet *sys) const
  {
    return m_base->getBool (element, sys) == false;
  }



  std::string SelectionReadAccessorInvert ::
  label () const
  {
    return m_base->label() + ",invert";
  }



  CP::SystematicSet SelectionReadAccessorInvert ::
  getInputAffecting (const ISystematicsSvc& svc,
                     const std::string& objectName) const
  {
    return m_base->getInputAffecting (svc, objectName);
  }



  StatusCode SelectionReadAccessorInvert ::
  fillSystematics (const ISystematicsSvc& svc,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName)
  {
    return m_base->fillSystematics (svc, sysList, objectName);
  }
}
