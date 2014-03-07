/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJTOP_H
#define TTBARLJTOP_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"

#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarHadronicTop.h"

#include <vector>
#include <set>

namespace TopRec {
  //================================================================
  //! Single lepton TTbar with a hadronic top reconstructed

  template<class LeptonCollection> 
  class TTbarLJTop : public TTbarLJ<LeptonCollection> {
    TTbarHadronicTop m_htop;
  public:
    //! topJets are indexes of this top quark jets in the TTbar's numbering
    TTbarLJTop(const TTbarLJ<LeptonCollection>& parent, const IndexSet& topJets)
      : TTbarLJ<LeptonCollection>(parent), m_htop(*this, topJets)
    {}

    TTbarLJTop(const TTbarLJ<LeptonCollection>& parent, const IndexVector& topJets)
      : TTbarLJ<LeptonCollection>(parent), m_htop(*this, topJets)
    {}

    // Access to the hadronic top leg
    const TTbarHadronicTop& hadronicTop() const { return m_htop; }

    // These two are needed to make sure the parent pointer
    // in the hadronic ttbars is set correctly.
    TTbarLJTop(const TTbarLJTop& other)
      : TTbarLJ<LeptonCollection>(other), m_htop(*this, other.m_htop.jetIndexes())
    {}

    TTbarLJTop& operator=(const TTbarLJTop& other) {
      if(this != &other) {
	TTbarLJ<LeptonCollection>::operator=(other);
	this->copyHadronicTop(&m_htop, other.m_htop);
      }
      return *this;
    }

    //! default constructor is needed for POOL and dictionaries
    TTbarLJTop() {}
  };

  //================================================================

  typedef TTbarLJTop<ElectronContainer> TTbarEJTop;
  typedef TTbarLJTop<Analysis::MuonContainer> TTbarMuJTop;
  typedef TTbarLJTop<Analysis::TauJetContainer> TTbarTauJTop;
  typedef TTbarLJTop<TruthParticleContainer> TTbarTruthJTop;

} // namespace TopRec

#endif //TTBARLJTOP_H
