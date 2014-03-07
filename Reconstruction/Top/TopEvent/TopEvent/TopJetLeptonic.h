/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETLEPTONIC_H
#define TOPJETLEPTONIC_H

#include "DataModel/ElementLink.h"
#include "TopEvent/TopJet.h"

#include <vector>
#include <set>

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

namespace TopRec {

  // Class for semileptonic "monojet" top candidates
  template<class LeptonCollection> 
  class TopJetLeptonic : public TopJet {
    
  public:
    typedef typename LeptonCollection::base_value_type Lepton;
    
    // constructors
    TopJetLeptonic(const JetCollection* jc, const unsigned & jindex,
		   const LeptonCollection* cc, const unsigned & lindex,
		   const float & x_lepton, const float & y_lepton, const float & yprime_lepton)
      : TopJet(jc,jindex), m_lepton(ElementLink<LeptonCollection>(*cc,lindex)), m_x_lepton(x_lepton), m_y_lepton(y_lepton), m_yprime_lepton(yprime_lepton) {} 
    
    TopJetLeptonic(const ElementLink<JetCollection>& jet,
		   const ElementLink<LeptonCollection>& lepton,
		   const float & x_lepton, const float & y_lepton, const float & yprime_lepton)
      : TopJet(jet), m_lepton(lepton), m_x_lepton(x_lepton), m_y_lepton(y_lepton), m_yprime_lepton(yprime_lepton) {}
    
    // needed for POOL and dictionaries
    TopJetLeptonic() {}
    
    
    // access to the lepton
    const Lepton& lepton() const { return **m_lepton; }
    ElementLink<LeptonCollection> leptonLink() const { return m_lepton; }
    
    
    // x_lepton
    float x_lepton() const {return m_x_lepton;}
    float y_lepton() const {return m_y_lepton;}
    float yprime_lepton() const {return m_yprime_lepton;}
   

  private:
    // store the elementlink to the lepton
    ElementLink<LeptonCollection>  m_lepton;
    
    // x_lepton
    float m_x_lepton;
    float m_y_lepton;
    float m_yprime_lepton;
    
  };

  //================================================================

  typedef TopJetLeptonic<ElectronContainer>         TopJetElectron;
  typedef TopJetLeptonic<Analysis::MuonContainer>   TopJetMuon;
  typedef TopJetLeptonic<Analysis::TauJetContainer> TopJetTau;
  typedef TopJetLeptonic<TruthParticleContainer>    TopJetTruth;
  
} // namespace TopRec

#endif // TOPJETLEPTONIC_H
