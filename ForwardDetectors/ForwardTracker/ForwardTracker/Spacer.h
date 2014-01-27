/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_EMPTYSPACEELEMENT_H
#define FORWARDTRACKER_EMPTYSPACEELEMENT_H

#include "ForwardTrackerConstants.h"
#include "IBeamElement.h"
#include "Point.h"

namespace ForwardTracker {

  class IParticle;
  class IBender;
  class Point;
  class TransversePoint;
  
  class Spacer: public IBeamElement {

  public:
    
    Spacer(double front, double back, Side side);
    Spacer(const IBeamElement::ConstPtr_t& previous, const IBeamElement::ConstPtr_t& current);
    ~Spacer() {};

    IBeamElement::ConstPtr_t clone() const { IBeamElement::ConstPtr_t pbe(new Spacer(*this)); return pbe; }
    
    std::string label()        const { return m_label; }
    Side        side()         const { return m_side; }    
    double      frontFace()    const { return m_frontFace; }
    double      rearFace()     const { return m_rearFace; }
    Point       position()     const { return m_position; }
    bool        isEndElement() const { return false; }
    
    void track(IParticle&) const;

    typedef boost::shared_ptr<const Spacer> ConstPtr_t;

    std::string str() const;    

  private:

    std::string m_label;
    Side        m_side;
    double      m_frontFace;
    double      m_rearFace;
    Point       m_position;
  };
}
#endif
