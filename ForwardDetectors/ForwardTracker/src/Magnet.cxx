/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/IBender.h"
#include "ForwardTracker/Magnet.h"
#include "ForwardTracker/Particle.h"
#include "ForwardTracker/TransversePoint.h"

namespace ForwardTracker {

  Magnet::Magnet(double              x,
		 double              y,
		 double              z,
		 double              length,
		 std::string         aperType,
		 double              aper1,
		 double              aper2,
		 double              aper3,
		 double              aper4,
		 Side                side,
		 IBender::ConstPtr_t bender, 
		 std::string         label):
    m_label    (label),
    m_side     (side),
    m_frontFace(z>0 ? z - 0.5*length : z + 0.5*length),
    m_rearFace (z>0 ? z + 0.5*length : z - 0.5*length),
    m_position (x, y, z),
    m_aperType (aperType),
    m_A1       (aper1),
    m_A2       (aper2),
    m_A3       (aper3),
    m_A4       (aper4),
    m_bender   (bender) 
  {}
  
  void Magnet::track(IParticle& particle) const {

    particle.setBeamCoordinateShift(this);
    
    if (particle.isOutOfAperture()) return;
    
    m_bender->bend(particle);

    particle.z() = m_rearFace;
    
    if (this->isOutOfAperture(particle)) particle.setOutOfAperture(true);
  }
  
  bool Magnet::isOutOfAperture(IParticle& particle) const {

    TransversePoint offCenter = particle.displacement();
    
    if      (m_aperType == "\"NONE\"") return false;
    else if (m_aperType == "\"CIRCLE\"") { 
      
      if (m_A1 == 0) return false;
      
      if (offCenter.mag2() > m_A1*m_A1) {
	
	std::ostringstream ost; ost << m_label << " isOutOfAperture ";
	
	ost << "radius: " << offCenter.mag2() << " > A1*A1: " << m_A1*m_A1;
	particle.addMsg(ost.str());
	
	return true;
      }
     
      return false;
    }
    else if (m_aperType == "\"RECTELLIPSE\"") {
      
      double xx = (m_A3) ? offCenter.x()/m_A3 : throw std::runtime_error("ERROR: m_A3 == 0");
      double yy = (m_A4) ? offCenter.y()/m_A4 : throw std::runtime_error("ERROR: m_A4 == 0");
      double rr = xx*xx + yy*yy;

      if (rr > 1) {
	
	std::ostringstream ost; ost << m_label << " isOutOfAperture ";
	
	ost << " x/A3: " << xx << " y/A4: " << yy << " (Squared sum: " << rr << " > 1)";
	particle.addMsg(ost.str());

	return true;
      }
      
      if (m_A1 > 0) {
	
	if (std::fabs(offCenter.x()) > m_A1) {

	  std::ostringstream ost; ost << m_label << " isOutOfAperture ";

	  ost << " |x|: " << std::fabs(offCenter.x()) << " > A1: " << m_A1;
	  particle.addMsg(ost.str());
	  
	  return true;
	}
      }
	
      if (m_A2 > 0) {

	if (std::fabs(offCenter.y()) > m_A2) {

	  std::ostringstream ost; ost << m_label << " isOutOfAperture ";

	  ost << " |y|: " << std::fabs(offCenter.y()) << " > A2: " << m_A2;
	  particle.addMsg(ost.str());

	  return true;
	}
      }
      
      return false;
    }
    else if (m_aperType == "\"OCTAGON\"") {
	
      if ( m_A1 > 0. ) {
	if( std::fabs(offCenter.x())>m_A1 ) {
	
	  std::ostringstream ost; ost << m_label << " isOutOfAperture ";

	  ost << " |x|: " << std::fabs(offCenter.x()) << " > A1: " << m_A1;
	  particle.addMsg(ost.str());
	  return true;
	}
      }
      if ( m_A2 > 0. ) {
	if( std::fabs(offCenter.y())>m_A2) {
	
	  std::ostringstream ost; ost << m_label << " isOutOfAperture ";

	  ost << " |y|: " << std::fabs(offCenter.y()) << " > A2: " << m_A2;
	  particle.addMsg(ost.str());
	  return true;
	}
      }
      if ( m_A3 > 0. && m_A4 > 0. ) {
	
	float B3=std::tan(m_A3)*m_A1;
	float B4=m_A2/std::tan(m_A4);
	float d=(m_A1-std::fabs(offCenter.x()))/(m_A1-B4)+(m_A2-std::fabs(offCenter.y()))/(m_A2-B3);
	    
	if(d<1.) {
	  std::ostringstream ost; ost << m_label << " isOutOfAperture ";
	
	  ost<<" x: "<<offCenter.x()<<" y: "<<offCenter.y()<<"  m_A1 "<<m_A1<<"  m_A2 "<<m_A2<<"  m_A3 "<<m_A3<<"  m_A4 "<<m_A4<<"  d: "<<d;
	  particle.addMsg( ost.str() );
	  return true;
	}
      }
	 
      return false; 
    }
    else {

      std::stringstream sstr; sstr << " Unknown magnet aperture type " << m_aperType << "\n";
      throw std::runtime_error(sstr.str().c_str());
    }

    return false;
  }

  std::string Magnet::str() const {

    std::ostringstream ost;

    ost << "m_label     " << m_label     << "\n"
	<< "m_side      " << m_side      << "\n"
	<< "m_frontFace " << m_frontFace << "\n"
	<< "m_rearFace  " << m_rearFace  << "\n"
	<< "m_position  " << m_position  << "\n"
	<< "m_aperType  " << m_aperType  << "\n"
	<< "m_A1        " << m_A1        << "\n"
	<< "m_A2        " << m_A2        << "\n"
	<< "m_A3        " << m_A3        << "\n"
	<< "m_A4        " << m_A4        << "\n";
    
    return ost.str();
  }
  
  std::ostream& operator<<(std::ostream& os, const Magnet& mag) { os << mag.str(); return os; }
}
