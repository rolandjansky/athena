/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJCONTAINER_H
#define TTBARLJCONTAINER_H

#include "TopEvent/TTbarLJ.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // Compensate the lack of templated typedefs
  
  template<class LeptonCollection>
  class TTbarLJContainer : public std::vector< TTbarLJ<LeptonCollection> > {
  public:
    TTbarLJContainer();
    virtual ~TTbarLJContainer() {;}
  };

  template<class LeptonCollection>
  TTbarLJContainer<LeptonCollection>::TTbarLJContainer():std::vector< TTbarLJ<LeptonCollection> >(){;}

  //================================================================

  typedef TTbarLJContainer<ElectronContainer>         TTbarEJContainer;
  typedef TTbarLJContainer<Analysis::MuonContainer>   TTbarMuJContainer;
  typedef TTbarLJContainer<Analysis::TauJetContainer> TTbarTauJContainer;
  typedef TTbarLJContainer<TruthParticleContainer>    TTbarTruthJContainer;  
  typedef TTbarLJContainer<JetCollection>    TTbarJeJContainer;  
}

CLASS_DEF( TopRec::TTbarEJContainer , 1140502407 , 1 )
CLASS_DEF( TopRec::TTbarMuJContainer , 1109175855 , 1 )
CLASS_DEF( TopRec::TTbarTauJContainer , 1265691326 , 1 )
CLASS_DEF( TopRec::TTbarTruthJContainer , 1164785380 , 1 )
CLASS_DEF( TopRec::TTbarJeJContainer , 1127450977 , 1 )

#endif //TTBARLJCONTAINER_H
