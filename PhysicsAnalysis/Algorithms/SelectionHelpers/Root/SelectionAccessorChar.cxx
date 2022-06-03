/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorChar.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorChar ::
  SelectionAccessorChar (const std::string& name)
    : m_accessor (name), m_constAccessor (name), m_label(name)
  {}



  SelectionType SelectionAccessorChar ::
  getBits (const SG::AuxElement& element,
           const CP::SystematicSet * /*sys*/) const
  {
    if (m_constAccessor (element))
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorChar ::
  setBits (const SG::AuxElement& element,
           SelectionType selection,
           const CP::SystematicSet * /*sys*/) const
  {
    if (selection == selectionAccept())
      m_accessor (element) = 1;
    else
      m_accessor (element) = 0;
  }



  bool SelectionAccessorChar ::
  getBool (const SG::AuxElement& element,
           const CP::SystematicSet * /*sys*/) const
  {
    return m_constAccessor (element);
  }



  void SelectionAccessorChar ::
  setBool (const SG::AuxElement& element,
           bool selection,
           const CP::SystematicSet * /*sys*/) const
  {
    m_accessor (element) = selection;
  }



  std::string SelectionAccessorChar ::
  label () const
  {
    return m_label;
  }



  CP::SystematicSet SelectionAccessorChar ::
  getInputAffecting (const ISystematicsSvc& /*svc*/,
                     const std::string& /*objectName*/) const
  {
    return CP::SystematicSet ();
  }



  StatusCode SelectionAccessorChar ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
