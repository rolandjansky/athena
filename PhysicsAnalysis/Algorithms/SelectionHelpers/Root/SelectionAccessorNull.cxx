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
  SelectionAccessorNull (bool value) : m_value(value)
  {}



  SelectionType SelectionAccessorNull ::
  getBits (const SG::AuxElement& /*element*/) const
  {
    return m_value ? selectionAccept() : selectionReject();
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
    return m_value;
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


  std::string SelectionAccessorNull ::
  label () const
  {
    return m_value ? "true" : "false";
  }
}
