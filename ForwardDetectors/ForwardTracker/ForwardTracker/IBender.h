/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_IBENDER_H
#define FORWARDTRACKER_IBENDER_H

#include "boost/shared_ptr.hpp"

namespace ForwardTracker {

  class IParticle;

  class IBender {

  public:
    
    virtual void bend(IParticle&) const = 0;

    typedef boost::shared_ptr<IBender> ConstPtr_t;
  };
}

#endif
