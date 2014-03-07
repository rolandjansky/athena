/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJMETCONTAINER_H
#define TTBARLJMETCONTAINER_H

#include "TopEvent/TTbarLJMet.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // Compensate the lack of templated typedefs
  
  template<class LeptonCollection>
  class TTbarLJMetContainer : public std::vector< TTbarLJMet<LeptonCollection> > {
  public:
    TTbarLJMetContainer();
    virtual ~TTbarLJMetContainer() {;}
  };

  template<class LeptonCollection>
  TTbarLJMetContainer<LeptonCollection>::TTbarLJMetContainer():std::vector< TTbarLJMet<LeptonCollection> >(){;}

  //================================================================

  typedef TTbarLJMetContainer<ElectronContainer>         TTbarEJMetContainer;
  typedef TTbarLJMetContainer<Analysis::MuonContainer>   TTbarMuJMetContainer;
  typedef TTbarLJMetContainer<Analysis::TauJetContainer> TTbarTauJMetContainer;
  typedef TTbarLJMetContainer<TruthParticleContainer>    TTbarTruthJMetContainer;
  typedef TTbarLJMetContainer<JetCollection>             TTbarJeJMetContainer;
}

CLASS_DEF( TopRec::TTbarEJMetContainer , 1225937153 , 1 )
CLASS_DEF( TopRec::TTbarMuJMetContainer , 1289557355 , 1 )
CLASS_DEF( TopRec::TTbarTauJMetContainer , 1146727778 , 1 )
CLASS_DEF( TopRec::TTbarTruthJMetContainer , 1271638523 , 1 )
CLASS_DEF( TopRec::TTbarJeJMetContainer , 1242709368 , 1 )

#endif //TTBARLJMETCONTAINER_H
