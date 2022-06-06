/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionWriteAccessorNull.h>

#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace CP
{
  SelectionWriteAccessorNull ::
  SelectionWriteAccessorNull ()
  {}



  void SelectionWriteAccessorNull ::
  setBits (const SG::AuxElement& /*element*/,
           SelectionType /*selection*/,
           const CP::SystematicSet * /*sys*/) const
  {
    // ok, let's not do anything here, making this an easy way to
    // ignore selection decorations an algorithm calculates.  however,
    // the implication is that you can simply forget to set the
    // decoration property and not get an error
  }



  void SelectionWriteAccessorNull ::
  setBool (const SG::AuxElement& /*element*/,
           bool /*selection*/,
           const CP::SystematicSet * /*sys*/) const
  {
    // ok, let's not do anything here, making this an easy way to
    // ignore selection decorations an algorithm calculates.  however,
    // the implication is that you can simply forget to set the
    // decoration property and not get an error
  }


  std::string SelectionWriteAccessorNull ::
  label () const
  {
    return "null";
  }



  StatusCode SelectionWriteAccessorNull ::
  fillSystematics (const ISystematicsSvc& /*svc*/,
                   const CP::SystematicSet& /*fullAffecting*/,
                   const std::vector<CP::SystematicSet>& /*sysList*/,
                   const std::string& /*objectName*/)
  {
    return StatusCode::SUCCESS;
  }
}
