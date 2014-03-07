/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETHADRONICCONTAINER_H
#define TOPJETHADRONICCONTAINER_H

#include "TopEvent/TopJetHadronic.h"
#include "SGTools/CLASS_DEF.h"

#include <vector>

namespace TopRec {
  class TopJetHadronicContainer : public std::vector<TopJetHadronic> {
  public:
    TopJetHadronicContainer();
    virtual ~TopJetHadronicContainer() {;}
  };
}

CLASS_DEF( TopRec::TopJetHadronicContainer , 1260834327 , 1 )

#endif //TOPJETHADRONICCONTAINER_H

