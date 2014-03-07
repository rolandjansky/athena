/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLL_H
#define TTBARLL_H

#include "DataModel/ElementLink.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"

namespace TopRec {

  //================================================================
  //! Minimalistic dilepton candidate 

  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLL {
  private:
    ElementLink<LeptonCollection1> m_lepton1;
    ElementLink<LeptonCollection2> m_lepton2;
  public:
    typedef typename LeptonCollection1::base_value_type Lepton1;
    typedef typename LeptonCollection2::base_value_type Lepton2;

    TTbarLL(const ElementLink<LeptonCollection1>& lepton1,
	    const ElementLink<LeptonCollection2>& lepton2)
      : m_lepton1(lepton1), m_lepton2(lepton2)
    {}

    //! default constructor is needed for POOL and dictionaries
    TTbarLL() {}

    const Lepton1& lepton1() const { return **m_lepton1; }
    const Lepton2& lepton2() const { return **m_lepton2; }

    ElementLink<LeptonCollection1> lepton1Link() const { return m_lepton1; }
    ElementLink<LeptonCollection2> lepton2Link() const { return m_lepton2; }
  };

  //================================================================

  typedef TTbarLL<ElectronContainer,ElectronContainer>                 TTbarEE;
  typedef TTbarLL<ElectronContainer,Analysis::MuonContainer>           TTbarEMu;
  typedef TTbarLL<ElectronContainer,Analysis::TauJetContainer>         TTbarETau;
  typedef TTbarLL<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrack;
  typedef TTbarLL<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMu;
  typedef TTbarLL<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTau;
  typedef TTbarLL<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrack;
  typedef TTbarLL<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTau;
  typedef TTbarLL<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruth;

} // namespace TopRec

#endif //TTBARLL_H
