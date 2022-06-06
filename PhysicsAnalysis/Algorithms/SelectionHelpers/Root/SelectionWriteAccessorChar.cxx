/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionWriteAccessorChar.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionWriteAccessorChar ::
  SelectionWriteAccessorChar (const std::string& name)
    : m_accessor (name), m_label(name)
  {}



  void SelectionWriteAccessorChar ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet * /*sys*/) const
  {
    if (selection == selectionAccept())
      m_accessor (element) = 1;
    else
      m_accessor (element) = 0;
  }



  void SelectionWriteAccessorChar ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet * /*sys*/) const
  {
    m_accessor (element) = selection;
  }



  std::string SelectionWriteAccessorChar ::
  label () const
  {
    return m_label;
  }



  StatusCode SelectionWriteAccessorChar ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const CP::SystematicSet& /*fullAffecting*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
