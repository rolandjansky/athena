/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorOR.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorOR ::
  SelectionAccessorOR (const std::string& name)
    : m_accessor (name)
  {}



  SelectionType SelectionAccessorOR ::
  getBits (const xAOD::IParticle& particle) const
  {
    if (m_accessor (particle))
      return selectionAccept();
    else
      return 0;
  }



  void SelectionAccessorOR ::
  setBits (xAOD::IParticle& particle,
           SelectionType selection) const
  {
    if (selection == selectionAccept())
      m_accessor (particle) = 1;
    else
      m_accessor (particle) = 0;
  }



  bool SelectionAccessorOR ::
  getBool (const xAOD::IParticle& particle) const
  {
    return m_accessor (particle);
  }



  void SelectionAccessorOR ::
  setBool (xAOD::IParticle& particle,
           bool selection) const
  {
    m_accessor (particle) = selection;
  }
}
