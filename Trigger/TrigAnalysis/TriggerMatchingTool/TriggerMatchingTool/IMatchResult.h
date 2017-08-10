// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMATCHRESULT_H
#define IMATCHRESULT_H

#include <memory>
#include <vector>

namespace xAOD{
  class IParticle;
}

namespace Trig {
  
  class IMatchResult {
  public:
    virtual ~IMatchResult(){}; //so we can delete properly via base pointer
    virtual bool isMatched() = 0;
    virtual std::vector<const xAOD::IParticle*> bestMatchedObjects() = 0;
  };
  
}

#endif
