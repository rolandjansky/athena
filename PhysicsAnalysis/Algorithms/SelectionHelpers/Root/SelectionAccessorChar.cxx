/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorChar.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorChar ::
  SelectionAccessorChar (const std::string& name)
    : m_accessor (name)
  {}



  SelectionType SelectionAccessorChar ::
  getBits (const xAOD::IParticle& particle) const
  {
    if (m_accessor (particle))
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorChar ::
  setBits (xAOD::IParticle& particle,
           SelectionType selection) const
  {
    if (selection == selectionAccept())
      m_accessor (particle) = 1;
    else
      m_accessor (particle) = 0;
  }



  bool SelectionAccessorChar ::
  getBool (const xAOD::IParticle& particle) const
  {
    return m_accessor (particle);
  }



  void SelectionAccessorChar ::
  setBool (xAOD::IParticle& particle,
           bool selection) const
  {
    m_accessor (particle) = selection;
  }
}
