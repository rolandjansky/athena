/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARJJCONTAINER_H
#define TTBARJJCONTAINER_H

#include "TopEvent/TTbarJJ.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {
  class TTbarJJContainer : public std::vector<TTbarJJ> {
  public:
    TTbarJJContainer();
    virtual ~TTbarJJContainer() {;}
  };
}

CLASS_DEF( TopRec::TTbarJJContainer , 1238881605 , 1 )

#endif //TTBARJJCONTAINER_H
