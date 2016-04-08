///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Jet_p5.cxx
// Implementation file for class Jet_p5
// Author: Rolf Seuster
/////////////////////////////////////////////////////////////////// 


// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p5.h"

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
// these actually get copied is when vector<Jet_p5> gets expanded.
//
// [C++0X will have a way to distinguish copy from move constructors...]
//
Jet_p5::Jet_p5 (const Jet_p5& other)
  : m_nav (other.m_nav),
    m_momentum (other.m_momentum),
    m_partBase (other.m_partBase),
    m_author (other.m_author),
    m_num_combinedLikelihood(other.m_num_combinedLikelihood),
    m_tagJetInfo(other.m_tagJetInfo),
    m_associations (other.m_associations),
    m_recoStatus(other.m_recoStatus),
    m_usedForTrigger(other.m_usedForTrigger),
    m_constituentsN(other.m_constituentsN)
{
  Jet_p5& o = const_cast<Jet_p5&> (other);
  //m_combinedLikelihood.swap (o.m_combinedLikelihood);
  m_shapeStore.swap (o.m_shapeStore);
  m_rawSignal.swap  (o.m_rawSignal);
  /* / signal state
     m_ratioE  = o.m_ratioE ;
     m_ratioPx = o.m_ratioPx;
     m_ratioPy = o.m_ratioPy;
     m_ratioPz = o.m_ratioPz; */
}


// Delete the pointed-to objects, if we own them.
Jet_p5::~Jet_p5()
{ }

