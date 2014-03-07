/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJDITOPSCONTAINER_H
#define TTBARLJDITOPSCONTAINER_H

#include "TopEvent/TTbarLJDiTops.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {

  //================================================================
  // Compensate the lack of templated typedefs

  template<class LeptonCollection>
  class TTbarLJDiTopsContainer : public std::vector< TTbarLJDiTops<LeptonCollection> > {
  public:
    TTbarLJDiTopsContainer();
    virtual ~TTbarLJDiTopsContainer() {;}
  };

  template<class LeptonCollection>
  TTbarLJDiTopsContainer<LeptonCollection>::TTbarLJDiTopsContainer():std::vector< TTbarLJDiTops<LeptonCollection> >(){;}

  //================================================================

  typedef TTbarLJDiTopsContainer<ElectronContainer>         TTbarEJDiTopsContainer;
  typedef TTbarLJDiTopsContainer<Analysis::MuonContainer>   TTbarMuJDiTopsContainer;
  typedef TTbarLJDiTopsContainer<Analysis::TauJetContainer> TTbarTauJDiTopsContainer;
  typedef TTbarLJDiTopsContainer<TruthParticleContainer>    TTbarTruthJDiTopsContainer;
  typedef TTbarLJDiTopsContainer<JetCollection>    TTbarJeJDiTopsContainer;
}

CLASS_DEF( TopRec::TTbarEJDiTopsContainer , 1108934999 , 1 )
CLASS_DEF( TopRec::TTbarMuJDiTopsContainer , 1292521049 , 1 )
CLASS_DEF( TopRec::TTbarTauJDiTopsContainer , 1233796814 , 1 )
CLASS_DEF( TopRec::TTbarTruthJDiTopsContainer , 1141877565 , 1 )
CLASS_DEF( TopRec::TTbarJeJDiTopsContainer , 1083824512 , 1 )

#endif //TTBARLJDITOPSCONTAINER_H
