/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorNull.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorNull ::
  SelectionAccessorNull ()
  {}



  SelectionType SelectionAccessorNull ::
  getBits (const SG::AuxElement& /*element*/) const
  {
    return selectionAccept();
  }



  void SelectionAccessorNull ::
  setBits (SG::AuxElement& /*element*/,
           SelectionType /*selection*/) const
  {
    // ok, let's not do anything here, making this an easy way to
    // ignore selection decorations an algorithm calculates.  however,
    // the implication is that you can simply forget to set the
    // decoration property and not get an error
  }



  bool SelectionAccessorNull ::
  getBool (const SG::AuxElement& /*element*/) const
  {
    return true;
  }



  void SelectionAccessorNull ::
  setBool (SG::AuxElement& /*element*/,
           bool /*selection*/) const
  {
    // ok, let's not do anything here, making this an easy way to
    // ignore selection decorations an algorithm calculates.  however,
    // the implication is that you can simply forget to set the
    // decoration property and not get an error
  }
}
