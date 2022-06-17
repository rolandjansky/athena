/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionWriteAccessorBits.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionWriteAccessorBits ::
  SelectionWriteAccessorBits (const std::string& name)
    : m_accessor (name), m_label(name)
  {}



  void SelectionWriteAccessorBits ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet * /*sys*/) const
  {
    m_accessor (element) = selection;
  }



  void SelectionWriteAccessorBits ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet * /*sys*/) const
  {
    if (selection)
      m_accessor (element) = selectionAccept();
    else
      m_accessor (element) = selectionAccept() ^ 0x1;
  }



  std::string SelectionWriteAccessorBits ::
  label () const
  {
    return m_label;
  }



  StatusCode SelectionWriteAccessorBits ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const CP::SystematicSet& /*fullAffecting*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
