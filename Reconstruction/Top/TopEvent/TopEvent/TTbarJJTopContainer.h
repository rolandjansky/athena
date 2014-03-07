/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARJJTOPCONTAINER_H
#define TTBARJJTOPCONTAINER_H

#include "TopEvent/TTbarJJTop.h"
#include "SGTools/CLASS_DEF.h"
#include <vector>

namespace TopRec {
  class TTbarJJTopContainer : public std::vector<TTbarJJTop> {
  public:
    TTbarJJTopContainer();
    virtual ~TTbarJJTopContainer() {;}
  };
}

CLASS_DEF( TopRec::TTbarJJTopContainer , 1279821033 , 1 )

#endif/*TTBARJJTOPCONTAINER_H*/
