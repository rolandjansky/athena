/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJDITOPSFITTEDCONTAINER_H
#define TTBARLJDITOPSFITTEDCONTAINER_H

#include "TopEvent/TTbarLJDiTopsFitted.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // Compensate the lack of templated typedefs

  template<class LeptonCollection>
  class TTbarLJDiTopsFittedContainer : public std::vector< TTbarLJDiTopsFitted<LeptonCollection> > {
  public:
    TTbarLJDiTopsFittedContainer();
    virtual ~TTbarLJDiTopsFittedContainer() {;}
  };

  template<class LeptonCollection>
  TTbarLJDiTopsFittedContainer<LeptonCollection>::TTbarLJDiTopsFittedContainer():std::vector< TTbarLJDiTopsFitted<LeptonCollection> >(){;}

  //================================================================

  typedef TTbarLJDiTopsFittedContainer<ElectronContainer>         TTbarEJDiTopsFittedContainer;
  typedef TTbarLJDiTopsFittedContainer<Analysis::MuonContainer>   TTbarMuJDiTopsFittedContainer;
  typedef TTbarLJDiTopsFittedContainer<Analysis::TauJetContainer> TTbarTauJDiTopsFittedContainer;
  typedef TTbarLJDiTopsFittedContainer<TruthParticleContainer>    TTbarTruthJDiTopsFittedContainer;
  typedef TTbarLJDiTopsFittedContainer<JetCollection>             TTbarJeJDiTopsFittedContainer;
}

CLASS_DEF( TopRec::TTbarEJDiTopsFittedContainer , 1101051213 , 1 )
CLASS_DEF( TopRec::TTbarMuJDiTopsFittedContainer , 1169269435 , 1 )
CLASS_DEF( TopRec::TTbarTauJDiTopsFittedContainer , 1093771280 , 1 )
CLASS_DEF( TopRec::TTbarTruthJDiTopsFittedContainer , 1259358403 , 1 )
CLASS_DEF( TopRec::TTbarJeJDiTopsFittedContainer , 1145434958 , 1 )

#endif //TTBARLJDITOPSFITTEDCONTAINER_H
