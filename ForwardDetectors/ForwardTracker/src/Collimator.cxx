/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Collimator.h"
#include "ForwardTracker/IParticle.h"
#include "ForwardTracker/TransverseShift.h"

namespace ForwardTracker {
  
  Collimator::Collimator(double z, double xaperture, Side side):
    m_label    ("Collimator"),
    m_side     (side),
    m_frontFace(z),
    m_rearFace (z),
    m_position (TransverseShift(z).x(),
		TransverseShift(z).y(),
		z),
    m_xaperture(xaperture),
    m_xouter   (   std::numeric_limits<double>::max()), 
    m_xinner   (-1*std::numeric_limits<double>::max())
  {}

  void Collimator::track(IParticle& particle) const {
    
    particle.setBeamCoordinateShift(this);

    if (particle.isOutOfAperture()) return;
    
    if (this->isOutOfAperture(particle.transversePosition())) particle.setOutOfAperture(true);
  } 

  std::string Collimator::str() const {

    std::stringstream ost; 

    ost << "m_label     " << m_label     << "\n"
	<< "m_side      " << m_side      << "\n"
	<< "m_frontFace " << m_frontFace << "\n"
	<< "m_rearFace  " << m_rearFace  << "\n"
	<< "m_postion   " << m_position  << "\n"
	<< "m_xaperture " << m_xaperture << "\n"
	<< "m_xouter    " << m_xouter    << "\n"
	<< "m_xinner    " << m_xinner    << "\n";
    
    return ost.str();
  }
    
  std::ostream& operator<<(std::ostream& os, const Collimator& coll) { os << coll.str(); return os; } 
}
