/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Beamline.h"
#include "FPTracker/STLHelpers.h"
#include "FPTracker/IParticle.h"
#include "FPTracker/IBeamElement.h"
#include "FPTracker/SimpleLogger.h"
#include "boost/shared_ptr.hpp"
#include <algorithm>
#include <cassert>
#include <string>
#include <sstream>
#include <cstddef>
//#include <iostream>
#include <stdexcept>

namespace FPTracker{
  
  bool isEndMarker(const IBeamElement::ConstPtr_t& p){return p->isEndElement();}
  
  IBeamElement::Iter_t findBeamLineEnd(IBeamElement::Container_t& container) {
    IBeamElement::Iter_t ei = find_if(container.begin(),
				      container.end(),
				      isEndMarker
				      );
    
    if (ei != container.end()) ++ei;
    else {
	std::string s("Could not find the end of the beamline.");
	throw std::runtime_error(s);
    }
    return ei;
  }
  class zPosNextElement{
  public:
    bool operator()( const IBeamElement::ConstPtr_t& be, double zpos ){ 
      return   std::fabs(zpos) > be->zabspos();
    }
  };


  Beamline::Beamline(){}

  Beamline::Beamline(IBeamElement::ConstListIter_t begin, IBeamElement::ConstListIter_t end)
  { // exception aware: attemt to create as a temp, if no exception, do safe swap ....
    IBeamElement::Container_t temp(begin, end);
    IBeamElement::Iter_t tempIter = findBeamLineEnd(temp);
    //BeamElementContainer temp2(temp.begin(), endIter(temp));
    IBeamElement::Container_t temp2(temp.begin(), tempIter);
    
    m_elements.swap(temp2); 
  }

  Beamline::Beamline(const Beamline& rhs)
  {
    
    IBeamElement::Container_t temp = rhs.m_elements; // exception aware any exceptions occur here
    m_elements.swap(temp); //nothrow op
  }
  
  Beamline& Beamline::operator=(Beamline rhs) // make a temp copy then swap
  {
    this->swap(rhs);
    return *this;
  }

  void Beamline::swap(Beamline& other)
  {
    m_elements.swap(other.m_elements);
  }

  class ParticleTracker{
  public:
    ParticleTracker(IParticle& particle):m_particle(particle), m_element(0){
    }
    bool  operator()(const IBeamElement::ConstPtr_t& be){

      be->track(m_particle); // updates particle position
      /*
      std::string label = be->label();
      if (label[0] == 'v' or label[0] == 'h')
	{
	  std::cout<<m_particle<<" "<<++m_element<<" "<<be->side()<<" "<<be->frontFace()<<" "<<be->rearFace()<<" "<<label<<'\n';
	  }
      */
      return m_particle.isOutOfAperture();
    }
  private:
    IParticle& m_particle;
    int       m_element;

  };
    

  void Beamline::track(IParticle& particle) const {
    IBeamElement::ConstIter_t nextElement = std::lower_bound(m_elements.begin(), 
							     m_elements.end(), 
							     particle.z(),  
							     zPosNextElement() );

    SimpleLogger::getLogger()->reset();
    // pass the particle to succesive beam elements until either it goes out of aperture, or it reaches the end plane.
    std::find_if(nextElement,
		 m_elements.end(),
		 ParticleTracker(particle)
		 );
    
  }

  class Calibrator{
  public:
    Calibrator(IParticle& particle):m_particle(particle), m_element(0)
    {
    }
    
    bool  operator()(const IBeamElement::ConstPtr_t& be)
    {
      be->calibrate(m_particle);
      return m_particle.isOutOfAperture();
    }
  private:
    IParticle& m_particle;
    int       m_element;

  };


  void Beamline::calibrate(IParticle& particle) const
  {
    IBeamElement::ConstIter_t nextElement = std::lower_bound(m_elements.begin(), 
							     m_elements.end(), 
							     particle.z(),  
							     zPosNextElement() );

    // pass the particle to succesive beam elements until either it goes out of aperture, or it reaches the end plane.
    IBeamElement::ConstIter_t lastElement = std::find_if(nextElement,
							 m_elements.end(),
							 Calibrator(particle)
							 );
    if( lastElement != m_elements.end() )
      {
	std::ostringstream s;
	s<<"Calibration particle did not reach end of beamline\n"<<particle<<'\n';
	s<<"Particle stopped at beam element "<<**lastElement<<'\n';
	throw std::runtime_error(s.str());
      }
  
  }




  std::string fixLabel(const std::string& label){
    std::string s = label;
    std::size_t tsize = 30 - label.size(); 
    if (tsize>0){ s += std::string(tsize, ' ');}
    return s;
  }

  class Stringer{
  public:
    Stringer():m_ost(new std::stringstream){
      (*m_ost)<<"\n\n--Beamline--\n";
    }
    void operator()(const IBeamElement::ConstPtr_t& ib){
      (*m_ost)<<fixLabel(ib->label())<<" "<<ib->side()<<" "<<ib->frontFace()<<" "<<ib->rearFace()<<'\n';
    }
    std::string str() const {return m_ost->str();}
  private:
    boost::shared_ptr< std::stringstream > m_ost;
  };
    
  std::string Beamline::str() const{
    return (std::for_each(m_elements.begin(), m_elements.end(), Stringer())).str();    
  }

  std::ostream& operator<<(std::ostream& os, const Beamline& bl){
    os<<bl.str()<<std::endl;
    return os;
  }
}

