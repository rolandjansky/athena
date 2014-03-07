/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETLEPTONICCONTAINER_H
#define TOPJETLEPTONICCONTAINER_H

#include "TopEvent/TopJetLeptonic.h"
#include "SGTools/CLASS_DEF.h"

#include <vector>

namespace TopRec {

  //================================================================
  // Compensate the lack of templated typedefs
  
  template<class LeptonCollection>
  class TopJetLeptonicContainer : public std::vector< TopJetLeptonic<LeptonCollection> > {
  public:
    TopJetLeptonicContainer();
    virtual ~TopJetLeptonicContainer() {;}
  };

  template<class LeptonCollection>
  TopJetLeptonicContainer<LeptonCollection>::TopJetLeptonicContainer():std::vector< TopJetLeptonic<LeptonCollection> >(){;}
  
  //================================================================

  typedef TopJetLeptonicContainer<ElectronContainer>         TopJetElectronContainer;
  typedef TopJetLeptonicContainer<Analysis::MuonContainer>   TopJetMuonContainer;
  typedef TopJetLeptonicContainer<Analysis::TauJetContainer> TopJetTauContainer;
  typedef TopJetLeptonicContainer<TruthParticleContainer>    TopJetTruthContainer;
} // end namespace TopRec

CLASS_DEF( TopRec::TopJetElectronContainer , 1145952953 , 1 )
CLASS_DEF( TopRec::TopJetMuonContainer , 1157701964 , 1 )
CLASS_DEF( TopRec::TopJetTauContainer , 1169818722 , 1 )
CLASS_DEF( TopRec::TopJetTruthContainer , 1077739335 , 1 )

#endif //TOPJETLEPTONICCONTAINER_H

