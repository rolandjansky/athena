/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/Beamline.h"

#include <iostream>

namespace ForwardTracker {
  
  bool isEndMarker(const IBeamElement::ConstPtr_t& be) { return be->isEndElement(); }
  
  IBeamElement::Iter_t findBeamLineEnd(IBeamElement::Container_t& container) {
  
    IBeamElement::Iter_t ei = find_if(container.begin(), container.end(), isEndMarker);
    
    if (ei != container.end()) ++ei;
    else throw std::runtime_error("Could not find the end of the beamline.");
    
    return ei;
  }
  
  class zPosNextElement {
    
  public:
    
    bool operator() (const IBeamElement::ConstPtr_t& be, double zpos) { return std::fabs(zpos) > std::fabs(be->position().z()); }
  };
  
  Beamline::Beamline(IBeamElement::ConstListIter_t begin, IBeamElement::ConstListIter_t end) { 

    IBeamElement::Container_t temp(begin, end);
    IBeamElement::Iter_t      tempIter = findBeamLineEnd(temp);
    IBeamElement::Container_t temp2(temp.begin(), tempIter);
    m_elements.swap(temp2); 
  }
  
  Beamline::Beamline(const Beamline& rhs) {
     
    IBeamElement::Container_t temp = rhs.m_elements; 
    m_elements.swap(temp);                           
  }
  
  class ParticleTracker {

  public:
    
    ParticleTracker(IParticle& particle): m_particle(particle), m_element(0) {}
    
    bool operator()(const IBeamElement::ConstPtr_t& be) { 
      
      be->track(m_particle); 

      if (m_particle.debug()) std::cout << std::setw(12) << be->label() << m_particle << std::endl;

      return m_particle.isOutOfAperture(); 
    }
    
  private:
    
    IParticle& m_particle;
    int        m_element;
  };

  void Beamline::track(IParticle& particle) const { // Pass the particle to beam elements until out of aperture or endPlane
    
    IBeamElement::ConstIter_t nextElement = std::lower_bound(m_elements.begin(), m_elements.end(), particle.z(), zPosNextElement());

    std::find_if(nextElement, m_elements.end(), ParticleTracker(particle));
  }
  
  class Stringer {
    
  public:
    
    Stringer(): m_ost(new std::stringstream) { (*m_ost) << "----------------- Beamline -----------------\n"; }
    
    void operator()(const IBeamElement::ConstPtr_t& be) {
      
      (*m_ost) << std::fixed << std::left
	       << std::setw(15) << be->label() 
 	       << std::setw(10) << be->side() << std::right
 	       << std::setw(12) << be->frontFace() 
 	       << std::setw(12) << be->rearFace()
 	       << "   pos"      << be->position() 
	       << "\n";
    }
    
    std::string str() const { return m_ost->str(); }

  private:
    
    boost::shared_ptr<std::stringstream> m_ost;
  };
  
  std::string Beamline::str() const { return (std::for_each(m_elements.begin(), m_elements.end(), Stringer())).str(); }
  
  std::ostream& operator<<(std::ostream& os, const Beamline& bl) { os << bl.str(); return os; }
}
