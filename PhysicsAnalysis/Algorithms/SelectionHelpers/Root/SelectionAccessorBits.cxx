/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorBits.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorBits ::
  SelectionAccessorBits (const std::string& name)
    : m_accessor (name), m_constAccessor (name), m_label(name)
  {}



  SelectionType SelectionAccessorBits ::
  getBits (const SG::AuxElement& element,
           const CP::SystematicSet * /*sys*/) const
  {
    return m_constAccessor (element);
  }



  void SelectionAccessorBits ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet * /*sys*/) const
  {
    m_accessor (element) = selection;
  }



  bool SelectionAccessorBits ::
  getBool (const SG::AuxElement& element,
           const CP::SystematicSet * /*sys*/) const
  {
    return m_constAccessor (element) == selectionAccept();
  }



  void SelectionAccessorBits ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet * /*sys*/) const
  {
    if (selection)
      m_accessor (element) = selectionAccept();
    else
      m_accessor (element) = selectionAccept() ^ 0x1;
  }



  std::string SelectionAccessorBits ::
  label () const
  {
    return m_label;
  }



  CP::SystematicSet SelectionAccessorBits ::
  getInputAffecting (const ISystematicsSvc& /*svc*/,
                     const std::string& /*objectName*/) const
  {
    return CP::SystematicSet ();
  }



  StatusCode SelectionAccessorBits ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
