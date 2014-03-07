/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJTOPCONTAINER_H
#define TTBARLJTOPCONTAINER_H

#include "TopEvent/TTbarLJTop.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {
  //================================================================
  // Compensate the lack of templated typedefs
  
  template<class LeptonCollection>
  class TTbarLJTopContainer : public std::vector< TTbarLJTop<LeptonCollection> > {
  public:
    TTbarLJTopContainer();
    virtual ~TTbarLJTopContainer() {;}
    TTbarEJTop m_test;
  };

  template<class LeptonCollection>
  TTbarLJTopContainer<LeptonCollection>::TTbarLJTopContainer():std::vector< TTbarLJTop<LeptonCollection> >(){;}

  //================================================================

  typedef TTbarLJTopContainer<ElectronContainer>         TTbarEJTopContainer;
  typedef TTbarLJTopContainer<Analysis::MuonContainer>   TTbarMuJTopContainer;
  typedef TTbarLJTopContainer<Analysis::TauJetContainer> TTbarTauJTopContainer;
  typedef TTbarLJTopContainer<TruthParticleContainer>    TTbarTruthJTopContainer;
}

CLASS_DEF( TopRec::TTbarEJTopContainer , 1246186143 , 1 )
CLASS_DEF( TopRec::TTbarMuJTopContainer , 1187730555 , 1 )
CLASS_DEF( TopRec::TTbarTauJTopContainer , 1304020040 , 1 )
CLASS_DEF( TopRec::TTbarTruthJTopContainer , 1236367646 , 1 )

#endif //TTBARLJTOPCONTAINER_H
