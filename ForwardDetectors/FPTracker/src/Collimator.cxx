/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Collimator.h"
#include "FPTracker/IParticle.h"
#include "FPTracker/TransversePoint.h"
#include "FPTracker/beamlineXPosition.h"
#include "FPTracker/FPTrackerConstants.h"
#include "FPTracker/SimpleLogger.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <limits>
//#include <cassert>

namespace FPTracker{
  
  const std::string Collimator::s_label="Collimator";

  Collimator::Collimator(double zpos, double xaperture, Side side):
    m_position( 
	       beamlineXPosition(zpos).x(),
	       beamlineXPosition(zpos).y(),
	       zpos
		),
    m_xaperture(xaperture),
    m_xouter( std::numeric_limits<double>::max() ), 
    m_xinner( -1*std::numeric_limits<double>::max() ),
    m_side(side)
  {
  }

  IBeamElement::ConstPtr_t Collimator::clone() const{
    IBeamElement::ConstPtr_t pbe(new Collimator(*this));
    return pbe;
  }

  double Collimator::frontFace()       const{ return m_position.z(); }
  double Collimator::rearFace()        const{ return m_position.z(); }
  double Collimator::zabspos()         const{ return std::fabs(m_position.z()); }
  double Collimator::zsignedpos()      const{ return m_position.z(); }
  Point  Collimator::position()        const{ return m_position; }
  Side   Collimator::side()            const{ return m_side; }
  std::string Collimator::label()      const{ return s_label; }
 
  std::string Collimator::str() const {
    std::stringstream ost;
    ost <<"-- Collimator --"<<'\n'
	<<"postion   " <<m_position<<'\n'
	<<"xouter    " <<m_xouter<<'\n'
	<<"xinner    " <<m_xinner<<'\n'
	<<"xaperture " <<m_xaperture<<'\n'
	<<"side      " <<m_side<<'\n';
    return ost.str();
  }

  bool Collimator::isEndElement() const{ return false; } 

  bool Collimator::isOutOfAperture(const TransversePoint& point) const {
    double xp = point.x();
    return  (xp>m_xouter or xp<m_xinner);
  }

  void Collimator::track(IParticle& particle) const
  {
    
    particle.setBeamCoordinateShift(this);

    if (particle.isOutOfAperture()){return;}

    if ( isOutOfAperture( particle.transversePosition() ) )
      {
	particle.setOutOfAperture(true);
      }

  } 

  void Collimator::calibrate(IParticle& particle) const
  {						
    // the collimator jaws start wide open.
    this->track(particle);
    double partX =particle.position().x();
    m_xouter = partX+m_xaperture;
    m_xinner = partX-m_xaperture;
      
  } 
  
  std::ostream& operator<<(std::ostream& os, const Collimator& coll){
    os<<coll.str();
    return os;
  }
  

}
