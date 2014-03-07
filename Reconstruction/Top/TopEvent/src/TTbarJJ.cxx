/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarJJ.h"
#include "CLHEP/Vector/LorentzVector.h"

namespace TopRec {
#if 0 /*no-op block for better indentation in Emacs*/
}
#endif

//================================================================
TTbarJJ::TTbarJJ(const JetCollection* jc, const IndexSet& jetNumbers)
  : TTbarJetsBase(jc,jetNumbers)
{
  update4momentum();
}

//================================================================
TTbarJJ::TTbarJJ(const JetSet& jets)
  : TTbarJetsBase(jets)
{
  update4momentum();
}

//================================================================
TTbarJJ::TTbarJJ(const JetVector& jets)
  : TTbarJetsBase(jets)
{
  update4momentum();
}

//================================================================
void TTbarJJ::update4momentum() {
  CLHEP::HepLorentzVector fourMom;
  for(JetVector::const_iterator i = jets().begin(); i != jets().end(); ++i) {
    fourMom += (**i)->hlv();
  }

  m_fourMomentum.set4Mom(fourMom);
}

//================================================================
	    
} // namespace TopRec
