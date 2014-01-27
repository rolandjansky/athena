/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_COLLIMATOR_H
#define FORWARDTRACKER_COLLIMATOR_H

#include "IBeamElement.h"
#include "Point.h"

#include <limits>

namespace ForwardTracker {
  
  class IParticle;
  class TransversePoint;

  class Collimator: public IBeamElement {
  
  public:

    Collimator(double, double, Side);
    
    std::string label()        const { return m_label; }
    Side        side()         const { return m_side; }
    double      frontFace()    const { return m_frontFace; }
    double      rearFace()     const { return m_rearFace; }
    Point       position()     const { return m_position; }
    bool        isEndElement() const { return false; } 

    IBeamElement::ConstPtr_t clone() const { IBeamElement::ConstPtr_t pbe(new Collimator(*this)); return pbe; }

    void track(IParticle&) const;
    
    typedef boost::shared_ptr< const Collimator > ConstPtr_t;
    typedef std::vector< ConstPtr_t >             Container_t;
    
    std::string str() const;

  private:
    
    std::string    m_label;
    Side           m_side;
    double         m_frontFace;
    double         m_rearFace;
    Point          m_position;
    double         m_xaperture;
    mutable double m_xouter;
    mutable double m_xinner;
    
    bool isOutOfAperture(const TransversePoint& p) const { return (p.x() > m_xouter or p.x() < m_xinner); }
  };
  
  std::ostream& operator<<(std::ostream&, const Collimator&);
}

#endif
