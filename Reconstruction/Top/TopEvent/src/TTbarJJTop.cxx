/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarJJTop.h"

namespace TopRec {
#if 0 /*no-op block for better indentation in Emacs*/
}
#endif

//================================================================
TTbarJJTop::TTbarJJTop(const TTbarJJ& parent, const IndexSet& topJets)
  : TTbarJJ(parent)
  , m_htop1(*this,topJets)
  , m_htop2(*this, getSetCompliment(parent.numJets(), topJets))
{
}

//================================================================
TTbarJJTop::TTbarJJTop(const TTbarJJ& parent, const IndexVector& topJets)
  : TTbarJJ(parent)
  , m_htop1(*this,topJets)
  , m_htop2(*this, getSetCompliment(parent.numJets(), topJets))
{
}

//================================================================
IndexSet TTbarJJTop::getSetCompliment(size_type max, const IndexVector& v) {
  IndexSet usedSet(v.begin(), v.end());
  return getSetCompliment(max, usedSet);
}

//================================================================
IndexSet TTbarJJTop::getSetCompliment(size_type max, const IndexSet& s) {
  IndexSet res;
  for(size_type i=0; i<max; ++i) {
    if(s.find(i) == s.end() )  {
      res.insert(i);
    }
  }
  return res;
}

//================================================================
TTbarJJTop::TTbarJJTop(const TTbarJJTop& other)
  : TTbarJJ(other)
  , m_htop1(*this, other.m_htop1.jetIndexes())
  , m_htop2(*this, other.m_htop2.jetIndexes())
{}

//================================================================
TTbarJJTop& TTbarJJTop::operator=(const TTbarJJTop& other) {
  if(this != &other) {
    TTbarJJ::operator=(other);
    copyHadronicTop(&m_htop1, other.m_htop1);
    copyHadronicTop(&m_htop2, other.m_htop2);
  }
  return *this;
}

//================================================================
	    
} // namespace TopRec

