/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Particle.h"
#include "FPTracker/IBeamElement.h"
#include "FPTracker/FPTrackerConstants.h"
#include <sstream>
#include <cmath>
#include <iomanip>
#include <iostream>
namespace FPTracker{
  Particle::Particle():
    m_position(0., 0., 0.), 
    m_momentum(0), 
    m_direction( Point(0., 0., 0.) ), 
    m_outOfAperture(false)
  {
  }

  Particle::Particle(double x, double y, double z, double px, double py, double pz):
    m_position(x, y, z), 
    m_momentum(std::sqrt(px*px+py*py+pz*pz)), 
    m_direction(Point(px/m_momentum, py/m_momentum, pz/m_momentum)), 
    m_outOfAperture(m_position.perp2()>FPTrackerConstants::beamPipeRadius2 ? true:false){
  }
  
  Point  Particle::position()         const { return m_position; }
  Point& Particle::position()               { return m_position; }
  Point  Particle::positionRelBeam()  const { return m_position +  m_beamCoordinateShift; }

  TransversePoint  Particle::displacement() const {
    return m_position.transverse()+m_beamCoordinateShift;
  }

   void Particle::updatePositionFromDisplacement(const TransversePoint& disp )  {
     m_position[0] = disp[0] - m_beamCoordinateShift[0]; // when the disp from mag center = beam axis, part. position = 0 in global coor
    m_position[1] = disp[1] - m_beamCoordinateShift[1];
  }

  void  Particle::setBeamCoordinateShift(const TransversePoint&  p)  {
    m_beamCoordinateShift = p;
  }

  void  Particle::setBeamCoordinateShift(const IBeamElement* be)  {
    m_beamCoordinateShift = -1.*(be->position()).transverse();
  }

  TransversePoint Particle::transversePosition() const{
    return m_position.transverse();
  }
  Point  Particle::direction()        const {return m_direction;}
  Point& Particle::direction()              {return m_direction;}
  double Particle::z()                const {return m_position.z();}
  double& Particle::z()                     {return m_position[2];}
  double Particle::momentum()         const {return m_momentum;}
  bool   Particle::isOutOfAperture()  const {return m_outOfAperture;}
  
  void Particle::setOutOfAperture(bool out){if(out){m_outOfAperture = true;}} //once out, always out
  void Particle::addMsg( const std::string& msg){ m_msg += msg; }
  std::string Particle::str() const{
    std::ostringstream ost;
    ost<<"Part: pos:"<<std::setprecision(4)<<std::fixed<<std::boolalpha//<<std::right
       <<" x   "<<std::setw(9)<<std::setprecision(6)<<m_position.x()
       <<" y   "<<std::setw(9)<<m_position.y()
       <<" z   "<<std::setw(7)<<std::setprecision(2)<<m_position.z()
       <<" dir "<<std::setprecision(6)
       <<" xp "<<std::setw(9)<<m_direction.x()
       <<" yp "<<std::setw(9)<<m_direction.y()
       <<" zp "<<std::setw(9)<<m_direction.z()
       << " mom: "<<std::setprecision(0)<<m_momentum
       <<" lost "<<m_outOfAperture
       <<" offx "<<std::setprecision(6)<<std::setw(9)<<m_beamCoordinateShift.x()
       <<" offy "<<std::setprecision(6)<<std::setw(9)<<m_beamCoordinateShift.y()
       <<'\n'
       <<" xrb "<<std::setw(9)<<std::setprecision(6)<<this->positionRelBeam().x()
       <<" yrb "<<std::setw(9)<<this->positionRelBeam().y()
       <<" zrb "<<std::setw(7)<<std::setprecision(2)<<this->positionRelBeam().z();
    if( not m_msg.empty() )
      {
	ost<<"\n msg " <<m_msg;
      }
    return ost.str();
  }
}
