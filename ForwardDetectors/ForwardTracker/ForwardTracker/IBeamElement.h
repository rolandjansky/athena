/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_IBEAMELEMENT_H
#define FORWARDTRACKER_IBEAMELEMENT_H

#include "ForwardTracker/ForwardTrackerConstants.h"

#include "boost/shared_ptr.hpp"

#include <vector>
#include <list>

namespace ForwardTracker {

  class IParticle;
  class Point;
  
  class IBeamElement {

  public:
  
    virtual ~IBeamElement() {};
        
    virtual std::string label()        const = 0;
    virtual Side        side()         const = 0;
    virtual double      frontFace()    const = 0;
    virtual double      rearFace()     const = 0;
    virtual Point       position()     const = 0;
    virtual bool        isEndElement() const = 0;

    virtual void track(IParticle&) const = 0;
    
    virtual boost::shared_ptr<const IBeamElement> clone() const = 0;

    typedef boost::shared_ptr<const IBeamElement> ConstPtr_t;
    typedef std::vector<ConstPtr_t>               Container_t;
    typedef Container_t::iterator                 Iter_t;
    typedef Container_t::const_iterator           ConstIter_t;
    typedef std::list<ConstPtr_t>                 List_t;
    typedef List_t::iterator                      ListIter_t;
    typedef List_t::const_iterator                ConstListIter_t;

    virtual std::string str() const = 0;
  };

  std::ostream& operator<<(std::ostream& os, const IBeamElement& be);
}

#endif
