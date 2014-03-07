/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarHadronicTopB.h"

namespace TopRec {
#if 0 /*no-op block for better indentation in Emacs*/
}
#endif

//================================================================
TTbarHadronicTopB::TTbarHadronicTopB(const TTbarJetsBase& base, const IndexSet& topJets, const size_type& indexHB) : TTbarHadronicTop(base,topJets), m_indexB(indexHB) { makeW(); }

//================================================================
IndexSet makeSetAllJetsButLB(const size_type& numberOfJets , const size_type& LB) {
  IndexSet theSet;
  for (size_type i = 0; i < numberOfJets; i++) {
    if (i != LB)
      theSet.insert(i);
  }
  return theSet;
}
TTbarHadronicTopB::TTbarHadronicTopB(const TTbarJetsBase& base, const size_type& indexHB, const size_type& indexLB) : TTbarHadronicTop(base,makeSetAllJetsButLB(base.numJets(),indexLB)), m_indexB(indexHB) { makeW(); }

//================================================================
void TTbarHadronicTopB::makeW() {
  IndexVector topJets(this->jetIndexes());
  m_indexesW.reserve(topJets.size()-1);

  for(IndexVector::const_iterator i = topJets.begin(); i != topJets.end(); ++i) {
    if (*i != m_indexB)
      m_indexesW.push_back(*i);
  }

  computeWfourMom();
}

void TTbarHadronicTopB::computeWfourMom() {
  CLHEP::HepLorentzVector tmpW;
  for (size_type i = 0; i < numJetsW(); i++){
    tmpW += jetW(i).hlv();
  }
  m_W.set4Mom(tmpW);
}

} // namespace TopRec
