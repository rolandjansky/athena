/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarHadronicTop.h"

namespace TopRec {
#if 0 /*no-op block for better indentation in Emacs*/
}
#endif

//================================================================
TTbarHadronicTop::TTbarHadronicTop(const TTbarJetsBase& base, const IndexSet& topJets)
  : m_parent(&base), m_indexes(topJets.begin(), topJets.end())
{
  computeFourMomentum();
}

//================================================================
TTbarHadronicTop::TTbarHadronicTop(const TTbarJetsBase& base, const IndexVector& topJets)
  : m_parent(&base), m_indexes(topJets)
{
  computeFourMomentum();
}

//================================================================
void TTbarHadronicTop::computeFourMomentum() {
  CLHEP::HepLorentzVector fourMom;
  for(IndexVector::const_iterator i = m_indexes.begin(); i != m_indexes.end(); ++i) {
    fourMom += m_parent->jet(*i).hlv();
  }
  m_fourMomentum.set4Mom(fourMom);
}

//================================================================
	    
} // namespace TopRec
