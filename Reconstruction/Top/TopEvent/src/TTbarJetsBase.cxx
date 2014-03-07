/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarJetsBase.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TopEvent/TTbarHadronicTop.h"

#include <algorithm>
#include <iterator>

namespace TopRec {
#if 0 /*no-op block for better indentation in Emacs*/
}
#endif

//================================================================
TTbarJetsBase::TTbarJetsBase(const JetCollection* jc, const IndexSet& jetNumbers) {
  m_jets.reserve(jetNumbers.size());
  for(IndexSet::const_iterator i=jetNumbers.begin(); i!=jetNumbers.end(); ++i) {
    m_jets.push_back(ElementLink<JetCollection>(*jc, *i));
  }
}

//================================================================
TTbarJetsBase::TTbarJetsBase(const JetSet& jets) {
  m_jets.reserve(5); // enough for most uses; if not will be resized automatically
  std::copy(jets.begin(), jets.end(), std::back_inserter(m_jets) );
}

//================================================================
TTbarJetsBase::TTbarJetsBase(const JetVector& jets) 
  : m_jets(jets)
{
}

//================================================================
void TTbarJetsBase::copyHadronicTop(TTbarHadronicTop *dst,
				    const TTbarHadronicTop& src) const
{
  dst->m_indexes = src.m_indexes;
  dst->m_parent = this;
  dst->computeFourMomentum();
}

//================================================================
	    
} // namespace TopRec
