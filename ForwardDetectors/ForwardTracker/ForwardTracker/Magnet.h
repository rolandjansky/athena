/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_MAGNET_H
#define FORWARDTRACKER_MAGNET_H

#include "ForwardTrackerConstants.h"
#include "IBeamElement.h"
#include "IBender.h"
#include "Particle.h"
#include "Point.h"

namespace ForwardTracker {

  class IParticle;
  class TransversePoint;

  class Magnet: public IBeamElement {

  public:
    
    Magnet(double              x,
	   double              y,
	   double              z,
	   double              length,
	   std::string         aperType,  
	   double              A1,
	   double              A2,
	   double              A3,
	   double              A4,
	   Side                side,
	   IBender::ConstPtr_t bender, 
	   std::string         label);
    
    enum Type {hbDipole, hfQuadrupole, vfQuadrupole, vbDipole, notAMagnet};

    typedef boost::shared_ptr<const Magnet> ConstPtr_t;
    typedef std::vector<ConstPtr_t>         Container_t;
    
    std::string label()        const { return m_label; }
    Side        side()         const { return m_side; }
    double      frontFace()    const { return m_frontFace; }
    double      rearFace()     const { return m_rearFace; }
    Point       position()     const { return m_position; }
    bool        isEndElement() const { return false; }
    
    IBeamElement::ConstPtr_t clone() const { IBeamElement::ConstPtr_t pm(new Magnet(*this)); return pm; }

    void track(IParticle&) const;

    std::string str() const;

  private:

    std::string         m_label;
    Side                m_side;
    double              m_frontFace;
    double              m_rearFace;
    Point               m_position;
    std::string         m_aperType;
    double              m_A1;
    double              m_A2;
    double              m_A3;
    double              m_A4;
    IBender::ConstPtr_t m_bender;

    bool isOutOfAperture(IParticle&) const;
  };

  std::ostream& operator<<(std::ostream&, const Magnet&);
  
}
#endif
