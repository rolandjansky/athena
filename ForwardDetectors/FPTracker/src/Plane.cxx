/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Plane.h"
#include "FPTracker/Particle.h"
#include "FPTracker/beamlineXPosition.h"
#include "FPTracker/FPTrackerConstants.h"
#include <cmath>
#include <sstream>
namespace FPTracker{

  const std::string Plane::s_label="Plane";
  std::string       Plane::label() const{return s_label;}

  //Plane::Plane(double zpos, double xinnerp, const Magnet* mp, int side):
  Plane::Plane(double zpos, Side side):
    m_position( 0., 0., 0. ), m_side(side)
  {
    assert(zpos >= 0.);  
    zpos = side == beam2 ? -zpos:zpos;
    TransversePoint shift = beamlineXPosition(zpos);
    m_position = Point(shift.x(), shift.y(), zpos);

  }

  IBeamElement::ConstPtr_t Plane::clone() const{
    IBeamElement::ConstPtr_t pbe( new Plane(*this) );
    return pbe;
  }

  double Plane::frontFace()  const{return m_position.z();}
  double Plane::rearFace()  const{return m_position.z();}
  double Plane::zabspos()    const{return std::fabs(m_position.z());}
  double Plane::zsignedpos() const{return m_position.z();}
  Point  Plane::position()   const{return m_position;}
  Side   Plane::side()       const{return m_side;}


  void Plane::track(IParticle& particle) const
  {
    particle.setBeamCoordinateShift(this);
    if (particle.isOutOfAperture()){return;}
    if (particle.displacement().mag2() > FPTrackerConstants::beamPipeRadius2){
      particle.setOutOfAperture(true);
    }

  } 

  void Plane::calibrate(IParticle& particle) const
  {
    return this->track(particle);
  }

  std::string Plane::str() const {
    std::ostringstream ost;
    ost<<"\n-- Plane --\n";
    ost<<" position           "<<m_position<<'\n';
    ost<<" side               "<<m_side<<'\n';
    return ost.str();
  }

  bool Plane::isEndElement() const{return true;} 

std::ostream& operator<<(std::ostream& os, const Plane& p){
    os<<p.str();
    return os;
  }
}
