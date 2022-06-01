/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionReadAccessorNull.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionReadAccessorNull ::
  SelectionReadAccessorNull (bool value) : m_value(value)
  {}



  SelectionType SelectionReadAccessorNull ::
  getBits (const SG::AuxElement& /*element*/,
           const CP::SystematicSet * /*sys*/) const
  {
    return m_value ? selectionAccept() : selectionReject();
  }



  bool SelectionReadAccessorNull ::
  getBool (const SG::AuxElement& /*element*/,
           const CP::SystematicSet * /*sys*/) const
  {
    return m_value;
  }


  std::string SelectionReadAccessorNull ::
  label () const
  {
    return m_value ? "true" : "false";
  }



  CP::SystematicSet SelectionReadAccessorNull ::
  getInputAffecting (const ISystematicsSvc& /*svc*/,
                     const std::string& /*objectName*/) const
  {
    return SystematicSet ();
  }



  StatusCode SelectionReadAccessorNull ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
