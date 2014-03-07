/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLLCONTAINER_H
#define TTBARLLCONTAINER_H

#include "TopEvent/TTbarLL.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // type generator

  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLLContainer : public std::vector< TTbarLL<LeptonCollection1,LeptonCollection2> > {
  public:
    TTbarLLContainer();
    virtual ~TTbarLLContainer() {;}
  };

  template<class LeptonCollection1, class LeptonCollection2>
  TTbarLLContainer<LeptonCollection1,LeptonCollection2>::TTbarLLContainer():std::vector< TTbarLL<LeptonCollection1,LeptonCollection2> >(){;}
  
  //================================================================

  typedef TTbarLLContainer<ElectronContainer,ElectronContainer>                 TTbarEEContainer;
  typedef TTbarLLContainer<ElectronContainer,Analysis::MuonContainer>           TTbarEMuContainer;
  typedef TTbarLLContainer<ElectronContainer,Analysis::TauJetContainer>         TTbarETauContainer;
  typedef TTbarLLContainer<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrackContainer;
  typedef TTbarLLContainer<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMuContainer;
  typedef TTbarLLContainer<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTauContainer;
  typedef TTbarLLContainer<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrackContainer;
  typedef TTbarLLContainer<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTauContainer;
  typedef TTbarLLContainer<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruthContainer;
}

CLASS_DEF( TopRec::TTbarEEContainer , 1247786054 , 1 )
CLASS_DEF( TopRec::TTbarEMuContainer , 1167429614 , 1 )
CLASS_DEF( TopRec::TTbarETauContainer , 1157914461 , 1 )
CLASS_DEF( TopRec::TTbarETrackContainer , 1339755447 , 1 )
CLASS_DEF( TopRec::TTbarMuMuContainer , 1339686150 , 1 )
CLASS_DEF( TopRec::TTbarMuTauContainer , 1195019405 , 1 )
CLASS_DEF( TopRec::TTbarMuTrackContainer , 1278492505 , 1 )
CLASS_DEF( TopRec::TTbarTauTauContainer , 1212151972 , 1 )
CLASS_DEF( TopRec::TTbarTruthTruthContainer , 1205673511 , 1 )

#endif //TTBARLLCONTAINER_H
