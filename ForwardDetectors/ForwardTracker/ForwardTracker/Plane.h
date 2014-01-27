/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_PLANE_H
#define FORWARDTRACKER_PLANE_H

#include "ForwardTrackerConstants.h"
#include "IBeamElement.h"
#include "Point.h"

namespace ForwardTracker {
  
  class IParticle;

  class Plane: public IBeamElement {

  public:

    Plane(double, Side);

    IBeamElement::ConstPtr_t clone() const { IBeamElement::ConstPtr_t pbe(new Plane(*this)); return pbe; }

    std::string label()        const { return m_label; }
    Side        side()         const { return m_side; }
    double      frontFace()    const { return m_frontFace; }
    double      rearFace()     const { return m_rearFace; }
    Point       position()     const { return m_position; }
    bool        isEndElement() const { return true; } 

    void track(IParticle&) const;
  
    typedef boost::shared_ptr<const Plane> ConstPtr_t;
    typedef std::vector<ConstPtr_t>        Container_t;

    std::string str() const;

  private:
    
    std::string m_label;
    Side        m_side;
    double      m_frontFace;
    double      m_rearFace;
    Point       m_position;
  };

  std::ostream& operator<<(std::ostream&, const Plane&);
}

#endif
