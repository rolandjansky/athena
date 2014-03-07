/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLJETSCONTAINER_H
#define TTBARLLJETSCONTAINER_H

#include "TopEvent/TTbarLLJets.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // type generator
  
  template<class LeptonCollection1, class LeptonCollection2>
  class TTbarLLJetsContainer : public std::vector< TTbarLLJets<LeptonCollection1,LeptonCollection2> > {
  public:
    TTbarLLJetsContainer();
    virtual ~TTbarLLJetsContainer() {;}
  };

  template<class LeptonCollection1, class LeptonCollection2>
  TTbarLLJetsContainer<LeptonCollection1,LeptonCollection2>::TTbarLLJetsContainer():std::vector< TTbarLLJets<LeptonCollection1,LeptonCollection2> >(){;}
  
  //================================================================

  typedef TTbarLLJetsContainer<ElectronContainer,ElectronContainer>                 TTbarEEJetsContainer;
  typedef TTbarLLJetsContainer<ElectronContainer,Analysis::MuonContainer>           TTbarEMuJetsContainer;
  typedef TTbarLLJetsContainer<ElectronContainer,Analysis::TauJetContainer>         TTbarETauJetsContainer;
  typedef TTbarLLJetsContainer<ElectronContainer,Rec::TrackParticleContainer>       TTbarETrackJetsContainer;
  typedef TTbarLLJetsContainer<Analysis::MuonContainer,Analysis::MuonContainer>     TTbarMuMuJetsContainer;
  typedef TTbarLLJetsContainer<Analysis::MuonContainer,Analysis::TauJetContainer>   TTbarMuTauJetsContainer;
  typedef TTbarLLJetsContainer<Analysis::MuonContainer,Rec::TrackParticleContainer> TTbarMuTrackJetsContainer;
  typedef TTbarLLJetsContainer<Analysis::TauJetContainer,Analysis::TauJetContainer> TTbarTauTauJetsContainer;
  typedef TTbarLLJetsContainer<TruthParticleContainer,TruthParticleContainer>       TTbarTruthTruthJetsContainer;
}

CLASS_DEF( TopRec::TTbarEEJetsContainer , 1228941034 , 1 )
CLASS_DEF( TopRec::TTbarEMuJetsContainer , 1169146070 , 1 )
CLASS_DEF( TopRec::TTbarETauJetsContainer , 1085754757 , 1 )
CLASS_DEF( TopRec::TTbarETrackJetsContainer , 1303765087 , 1 )
CLASS_DEF( TopRec::TTbarMuMuJetsContainer , 1212117474 , 1 )
CLASS_DEF( TopRec::TTbarMuTauJetsContainer , 1302968449 , 1 )
CLASS_DEF( TopRec::TTbarMuTrackJetsContainer , 1177839685 , 1 )
CLASS_DEF( TopRec::TTbarTauTauJetsContainer , 1340066712 , 1 )
CLASS_DEF( TopRec::TTbarTruthTruthJetsContainer , 1167512255 , 1 )

#endif //TTBARLLJETSCONTAINER_H
