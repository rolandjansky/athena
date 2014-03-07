/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLLJETS_H
#define TTBARLLJETS_H

#include "TopEvent/TTbarLL.h"
#include "TopEvent/TTbarJetsBase.h"

namespace TopRec {

  //================================================================
  //! Dileptonic top that includes jets, no associations.

  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLLJets : public TTbarJetsBase,
		      public TTbarLL<LeptonCollection1,LeptonCollection2> {
  public:

    //! All jets are from one collection
    TTbarLLJets(const ElementLink<LeptonCollection1>& lepton1,
		const ElementLink<LeptonCollection2>& lepton2,
		const JetCollection* jc, const IndexSet& jetNumbers)
      : TTbarJetsBase(jc, jetNumbers), TTbarLL<LeptonCollection1,LeptonCollection2>(lepton1, lepton2)
    {}

    //! General case
    TTbarLLJets(const ElementLink<LeptonCollection1>& lepton1,
		const ElementLink<LeptonCollection2>& lepton2,
		const JetSet& jets)
      : TTbarJetsBase(jets), TTbarLL<LeptonCollection1,LeptonCollection2>(lepton1, lepton2)
    {}

    //! General case
    TTbarLLJets(const ElementLink<LeptonCollection1>& lepton1,
		const ElementLink<LeptonCollection2>& lepton2,
		const JetVector& jets)
      : TTbarJetsBase(jets), TTbarLL<LeptonCollection1,LeptonCollection2>(lepton1, lepton2)
    {}

    //! default constructor is needed for POOL and dictionaries
    TTbarLLJets() {}
  };

  //================================================================

  typedef TTbarLLJets<ElectronContainer,ElectronContainer>                 TTbarEEJets;
  typedef TTbarLLJets<ElectronContainer,Analysis::MuonContainer>           TTbarEMuJets;
  typedef TTbarLLJets<ElectronContainer,Analysis::TauJetContainer>         TTbarETauJets;
  typedef TTbarLLJets<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrackJets;
  typedef TTbarLLJets<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMuJets;
  typedef TTbarLLJets<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTauJets;
  typedef TTbarLLJets<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrackJets;
  typedef TTbarLLJets<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTauJets;
  typedef TTbarLLJets<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruthJets;

} // namespace TopRec

#endif //TTBARLLJETS_H
