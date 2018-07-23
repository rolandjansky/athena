/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SelectionHelpers/SelectionAccessorRegular.h>

//
// method implementations
//

namespace CP
{
  SelectionAccessorRegular ::
  SelectionAccessorRegular (const std::string& name)
    : m_accessor (name)
  {}



  SelectionType SelectionAccessorRegular ::
  getBits (const xAOD::IParticle& particle) const
  {
    return m_accessor (particle);
  }



  void SelectionAccessorRegular ::
  setBits (xAOD::IParticle& particle,
           SelectionType selection) const
  {
    m_accessor (particle) = selection;
  }



  bool SelectionAccessorRegular ::
  getBool (const xAOD::IParticle& particle) const
  {
    return m_accessor (particle) == selectionAccept();
  }



  void SelectionAccessorRegular ::
  setBool (xAOD::IParticle& particle,
           bool selection) const
  {
    if (selection)
      m_accessor (particle) = selectionAccept();
    else
      m_accessor (particle) = selectionAccept() ^ 0x1;
  }
}
