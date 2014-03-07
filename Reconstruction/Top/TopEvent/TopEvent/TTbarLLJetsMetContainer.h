/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLJETSMETCONTAINER_H
#define TTBARLLJETSMETCONTAINER_H

#include "TopEvent/TTbarLLJetsMet.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // type generator
  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLLJetsMetContainer : public std::vector< TTbarLLJetsMet<LeptonCollection1,LeptonCollection2> > {
  public:
    TTbarLLJetsMetContainer();
    virtual ~TTbarLLJetsMetContainer() {;}
  };

  template<class LeptonCollection1, class LeptonCollection2>
  TTbarLLJetsMetContainer<LeptonCollection1,LeptonCollection2>::TTbarLLJetsMetContainer():std::vector< TTbarLLJetsMet<LeptonCollection1,LeptonCollection2> >(){;}
  
  //================================================================

  typedef TTbarLLJetsMetContainer<ElectronContainer,ElectronContainer>                 TTbarEEJetsMetContainer;
  typedef TTbarLLJetsMetContainer<ElectronContainer,Analysis::MuonContainer>           TTbarEMuJetsMetContainer;
  typedef TTbarLLJetsMetContainer<ElectronContainer,Analysis::TauJetContainer>         TTbarETauJetsMetContainer;
  typedef TTbarLLJetsMetContainer<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrackJetsMetContainer;
  typedef TTbarLLJetsMetContainer<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMuJetsMetContainer;
  typedef TTbarLLJetsMetContainer<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTauJetsMetContainer;
  typedef TTbarLLJetsMetContainer<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrackJetsMetContainer;
  typedef TTbarLLJetsMetContainer<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTauJetsMetContainer;
  typedef TTbarLLJetsMetContainer<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruthJetsMetContainer;
}

CLASS_DEF( TopRec::TTbarEEJetsMetContainer , 1251173282 , 1 )
CLASS_DEF( TopRec::TTbarEMuJetsMetContainer , 1276801852 , 1 )
CLASS_DEF( TopRec::TTbarETauJetsMetContainer , 1327161827 , 1 )
CLASS_DEF( TopRec::TTbarETrackJetsMetContainer , 1217616638 , 1 )
CLASS_DEF( TopRec::TTbarMuMuJetsMetContainer , 1206272626 , 1 )
CLASS_DEF( TopRec::TTbarMuTauJetsMetContainer , 1321134165 , 1 )
CLASS_DEF( TopRec::TTbarMuTrackJetsMetContainer , 1139898148 , 1 )
CLASS_DEF( TopRec::TTbarTauTauJetsMetContainer , 1181788732 , 1 )
CLASS_DEF( TopRec::TTbarTruthTruthJetsMetContainer , 1257830606 , 1 )

#endif //TTBARLLJETSMETCONTAINER_H
