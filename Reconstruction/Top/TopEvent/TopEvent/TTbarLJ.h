/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJ_H
#define TTBARLJ_H

#include "DataModel/ElementLink.h"
#include "JetEvent/JetCollection.h"

#include "TopEvent/TTbarJetsBase.h"

#include <vector>
#include <set>

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

namespace TopRec {

  //================================================================
  //! Here we have on lepton in addition to the jets, still 
  //! no associations.

  template<class LeptonCollection> 
  class TTbarLJ : public TTbarJetsBase {
  private:
    ElementLink<LeptonCollection> m_lepton;
  public: 
    typedef typename LeptonCollection::base_value_type Lepton;

    //! All jets are from one collection
    TTbarLJ(const ElementLink<LeptonCollection>& lepton, const JetCollection* jc, const IndexSet& jetNumbers)
      : TTbarJetsBase(jc, jetNumbers), m_lepton(lepton)
    {}      

    //! General case
    TTbarLJ(const ElementLink<LeptonCollection>& lepton, const JetSet& jets)
      : TTbarJetsBase(jets), m_lepton(lepton)
    {}

    //! General case
    TTbarLJ(const ElementLink<LeptonCollection>& lepton, const JetVector& jets)
      : TTbarJetsBase(jets), m_lepton(lepton)
    {}

    //! default constructor is needed for POOL and dictionaries
    TTbarLJ() {}

    const Lepton& lepton() const { return **m_lepton; }

    ElementLink<LeptonCollection> leptonLink() const { return m_lepton; }
  };

  //================================================================

  typedef TTbarLJ<ElectronContainer>         TTbarEJ;
  typedef TTbarLJ<Analysis::MuonContainer>   TTbarMuJ;
  typedef TTbarLJ<Analysis::TauJetContainer> TTbarTauJ;
  typedef TTbarLJ<TruthParticleContainer>    TTbarTruthJ;
  typedef TTbarLJ<JetCollection>    TTbarJeJ;

} // namespace TopRec

#endif //TTBARLJ_H
