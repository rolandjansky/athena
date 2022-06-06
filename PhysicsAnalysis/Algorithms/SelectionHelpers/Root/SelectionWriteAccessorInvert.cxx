/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionWriteAccessorInvert.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionWriteAccessorInvert ::
  SelectionWriteAccessorInvert (std::unique_ptr<ISelectionWriteAccessor> val_base)
    : m_base (std::move (val_base))
  {}



  void SelectionWriteAccessorInvert ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet *sys) const
  {
    m_base->setBool (element, selection != selectionAccept(), sys);
  }



  void SelectionWriteAccessorInvert ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet *sys) const
  {
    m_base->setBool (element, !selection, sys);
  }



  std::string SelectionWriteAccessorInvert ::
  label () const
  {
    return m_base->label() + ",invert";
  }



  StatusCode SelectionWriteAccessorInvert ::
  fillSystematics (const ISystematicsSvc& svc,
                   const CP::SystematicSet& fullAffecting,
                   const std::vector<CP::SystematicSet>& sysList,
                   const std::string& objectName)
  {
    return m_base->fillSystematics (svc, fullAffecting, sysList, objectName);
  }
}
