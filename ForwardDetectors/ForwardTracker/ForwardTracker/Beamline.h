/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_BEAMLINE_H
#define FORWARDTRACKER_BEAMLINE_H

#include "IBeamElement.h"
#include "IParticle.h"
#include "Point.h"

namespace ForwardTracker {

  class IParticle;

  class Beamline {

  public:

    Beamline() {};
    Beamline(IBeamElement::ConstListIter_t, IBeamElement::ConstListIter_t);
    Beamline(const Beamline&);

    Beamline& operator=(Beamline rhs) { this->swap(rhs); return *this; }
    
    void track(IParticle&) const;

    std::string str() const;

  private:

    IBeamElement::Container_t m_elements;
    
    void swap(Beamline& other) { m_elements.swap(other.m_elements); }
  };
  
  std::ostream& operator<<(std::ostream&, const Beamline&);
}

#endif
