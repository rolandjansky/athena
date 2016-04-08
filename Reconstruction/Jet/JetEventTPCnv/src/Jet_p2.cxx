///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Jet_p2.cxx 
// Implementation file for class Jet_p2
// Author: R.Seuster<seuster@cern.ch>
// new JetEDM
/////////////////////////////////////////////////////////////////// 


// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p2.h"

#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetAssociationBase.h"


/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

//
// Warning: this is a _move_ constructor, not really a copy constructor.
// The memory contained in the vectors in other is transferred to the
// new object (leaving the vectors in the original object empty).
//
// We can get away with this for these objects because the only place
// these actually get copied is when vector<Jet_p2> gets expanded.
//
// [C++0X will have a way to distinguish copy from move constructors...]
//
Jet_p2::Jet_p2 (const Jet_p2& other)
  : m_nav (other.m_nav),
    m_momentum (other.m_momentum),
    m_author (other.m_author),
    m_ownPointers (other.m_ownPointers)
{
  Jet_p2& o = const_cast<Jet_p2&> (other);
  m_combinedLikelihood.swap (o.m_combinedLikelihood);
  m_shapeStore.swap (o.m_shapeStore);
  m_tagJetInfo.swap (o.m_tagJetInfo);
  m_associations.swap (o.m_associations);
}


// Delete the pointed-to objects, if we own them.
Jet_p2::~Jet_p2()
{
  if (m_ownPointers) {
    for (size_t i = 0; i < m_tagJetInfo.size(); i++)
      delete m_tagJetInfo[i];

    for (size_t i = 0; i < m_associations.size(); i++)
      delete m_associations[i];
  }
}

