/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/EmptySpaceElement.h"
#include "FPTracker/Particle.h"
#include <sstream>
#include <cmath>
#include <iostream>

namespace FPTracker{

  EmptySpaceElement::EmptySpaceElement(double front, double back, const TransversePoint& beamAxis, Side side):
    m_frontFace(front), m_rearFace(back),
    m_signedLength(m_rearFace-m_frontFace),
    m_absLength(std::fabs(m_signedLength)),
    m_pos( beamAxis.x(), beamAxis.y(), front+0.5*m_signedLength),
    m_label("Spacer"), m_side(side){
  } 

  EmptySpaceElement::EmptySpaceElement(const IBeamElement::ConstPtr_t& p, const IBeamElement::ConstPtr_t& c):
    m_frontFace(p->rearFace()), m_rearFace(c->frontFace()), m_signedLength(m_rearFace-m_frontFace),
    m_absLength(std::fabs(m_signedLength)),
    m_pos( p->position() ),
    m_label("Spacer"), m_side(c->side()){ 
  }

  IBeamElement::ConstPtr_t EmptySpaceElement::clone() const{
    IBeamElement::ConstPtr_t  pbe(new EmptySpaceElement(*this));
    return pbe;
  }

  double EmptySpaceElement::frontFace()  const {return m_frontFace;}
  double EmptySpaceElement::rearFace()   const {return m_rearFace;}
  std::string EmptySpaceElement::label() const {return m_label;}
  double EmptySpaceElement::zsignedpos() const {return m_frontFace + /*0.5* */m_signedLength;}//Added dpelikan @ 110308
  double EmptySpaceElement::zabspos()    const {return std::fabs(this->zsignedpos());}
  Point EmptySpaceElement::position()    const {return m_pos;}
  Side EmptySpaceElement::side()         const {return m_side;}
  bool EmptySpaceElement::isEndElement() const {return false;}

  void EmptySpaceElement::track(IParticle& particle) const 
  {
    //Added dpelikan @ 110308
    double TransLength=m_absLength-(std::abs(particle.position().z())-std::abs(m_frontFace));
    //std::cout<<m_absLength<<"\t"<<TransLength<<std::endl;
    
    particle.setBeamCoordinateShift(this);
    


    //particle.position()    += m_absLength*particle.direction();
    particle.position()    += TransLength*particle.direction();
    particle.position()[2] = m_rearFace;

  }

  void EmptySpaceElement::calibrate(IParticle& particle) const 
  {
    return this->track(particle);
  }

  std::string EmptySpaceElement::str() const {

    std::ostringstream ost;
    ost <<" \n\n-- EmptySpaceElement  --\n"
	<< "frontFace  "  << m_frontFace <<'\n'
	<< "rearFace   "  << m_rearFace  <<'\n';
    return ost.str();
  }

  EmptySpaceElement::~EmptySpaceElement(){}
  /*
  double EmptySpaceElement::rearFace() const {return 0.;}
  double EmptySpaceElement::frontFace() const {return 0.;}
  std::string EmptySpaceElement::label() const {return "";}
  double EmptySpaceElement::zsignedpos() const {return 0.;}
  double EmptySpaceElement::zabspos()    const {return 0.;}
  Point EmptySpaceElement::position()    const {return Point(0., 0., 0.);}
  int EmptySpaceElement::side()          const {return 0;}
  bool EmptySpaceElement::isEndElement() const {return false;}
  void EmptySpaceElement::track(Particle& particle) const {}

  std::string EmptySpaceElement::str() const {return " ";}
  */

}

