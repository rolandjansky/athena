// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETANALYSISEDM_CONSTITUENTITERATOR_H
#define JETANALYSISEDM_CONSTITUENTITERATOR_H
#include "JetAnalysisEDM/Particle.h"

namespace JetAnalysisEDM {
  
  /// //////////////////////////////////////////
  /// \class ConstituentIterator
  /// A constituent iterator aware of the constituent's signal state
  ///
  /// This class is meant to be compatible with Athena's JetConstituentIterator.
  /// It allows to iterate over the constituent of a jet with a derired signal state or 
  /// with the default sig state used in the jet finding.
  /// 
  /// IMPORTANT :
  /// D3PD do not contain the 'constituent state at jet finding step' information. 
  /// Nor is any constituents information.
  /// So ConstituentIterator will only work well if the ParticleContainer::declareConstituents() 
  /// method has been called on the to-be-iterated Particles.
  /// The declareConstituents must also be called with a ParticleContainer argument configured to retrieve constituents at the expected signal states (there's no way to do this 
  /// automatically from D3PD). In practice this only concerns TopoEM jet collections.
  /// ///////////////////////////////////////////////////////////  
  class ConstituentIterator : public LorentzVector_t {
  public:

    ConstituentIterator & operator++ ();
    ConstituentIterator & operator-- ();

    bool operator== (const ConstituentIterator& other) const ;
    bool operator!= (const ConstituentIterator& other) const ;

    const Particle* get_real_constit();

    static ConstituentIterator first(const Particle * jet);
    static ConstituentIterator first(const Particle * jet, P4SignalState::State s);
    static ConstituentIterator last(const Particle * jet);

  protected:

    ConstituentIterator(const Particle* p);

    void update4Mom();
    const ParticleContainer * m_constituentSource ; /// source container for constituents of Particles
    const std::vector<int> * m_indexVec;
    size_t m_index;
    bool m_useSigState;
    P4SignalState::State m_sigState;

  };

}
#endif
