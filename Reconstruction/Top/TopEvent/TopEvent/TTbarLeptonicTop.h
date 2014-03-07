/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLEPTONICTOP_H
#define TTBARLEPTONICTOP_H

#include "DataModel/ElementLink.h"
#include "JetEvent/Jet.h"

#include "EventKernel/I4Momentum.h"
#include "FourMom/P4PxPyPzE.h"

#include "TopEvent/TTbarLJ.h"

#include <vector>
#include <set>

namespace TopRec {
  template<class LeptonCollection> class TTbarLJDiTops;

  //================================================================
  //! This represents a leptonic top in a ttbar decay, not a standalone 
  //! top quark.  In other words, this class associates a jet of 
  //! ttbar jets and its lepton to a top quark.
  //! However, the neutrino it helds by the leptonic top
  
  template<class LeptonCollection>
  class TTbarLeptonicTop {
    friend class TTbarLJDiTops<LeptonCollection>;
    
    P4PxPyPzE m_fourMomentum;
    const TTbarLJ<LeptonCollection> *m_parent;
    P4PxPyPzE m_neutrino;
    P4PxPyPzE m_W;
    size_type m_indexB;

    void set4momenta() {
      m_fourMomentum.set4Mom(m_parent->lepton().hlv() + m_neutrino.hlv() + m_parent->jet(m_indexB).hlv());
      m_W.set4Mom(m_parent->lepton().hlv() + m_neutrino.hlv());
    }

    // Forbid copying or m_parent would become wrong.
    TTbarLeptonicTop(const TTbarLeptonicTop&);
    TTbarLeptonicTop& operator=(const TTbarLeptonicTop&);

  public:
    typedef typename LeptonCollection::base_value_type Lepton;
    
    TTbarLeptonicTop(const TTbarLJ<LeptonCollection>& base, const P4PxPyPzE& neutrino, const size_type& indexB) : m_parent(&base), m_neutrino(neutrino), m_indexB(indexB) { set4momenta(); }

    //! default constructor is needed for POOL and dictionaries
    TTbarLeptonicTop() {}

    // Return the parent TTbar
    const TTbarLJ<LeptonCollection>& parent() const { return *m_parent; } 

    //! 4-momentum of this top
    const I4Momentum& fourMom() const { return m_fourMomentum; }
    
    const I4Momentum& neutrino() const { return m_neutrino; }
    const size_type& jetindexB() const { return m_indexB; }
    const Jet& jetB() const { return parent().jet(m_indexB); }
    const Lepton& lepton() const { return parent().lepton(); }
    const I4Momentum& W() const { return m_W; } 
  };

  //================================================================
  
  typedef TTbarLeptonicTop<ElectronContainer>         TTbarElectronicTop;
  typedef TTbarLeptonicTop<Analysis::MuonContainer>   TTbarMuonicTop;
  typedef TTbarLeptonicTop<Analysis::TauJetContainer> TTbarTauTop;
  typedef TTbarLeptonicTop<TruthParticleContainer>    TTbarTruthLeptonicTop;
} // namespace TopRec

#endif //TTBARLEPTONICTOP_H
