/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/Magnet.h"
#include "FPTracker/Particle.h"
#include "FPTracker/TransversePoint.h"
#include "FPTracker/IBender.h"
#include <sstream>
#include <cmath>
//#include <iostream>
#include <cassert>

namespace FPTracker{

  std::string Magnet::label() const{
    return m_label+":"+m_bender->label();
  }

  Magnet::Magnet(
		 double x,
		 double y,
		 double center,
		 double strength,
		 double length,
		 int apertype,  
		 double aper_A1,
		 double aper_A2,
		 double aper_A3,
		 double aper_A4,
		 double xb,
		 Side side,
		 IBender::ConstPtr_t  bender, 
		 std::string label):
    m_center(x, y, center),
    m_frontFace(center>0 ? center-0.5*length:center+0.5*length),
    m_rearFace(center>0 ? center+0.5*length:center-0.5*length),
    m_strength(strength),
    m_apertype(apertype),
    m_aper_A1(aper_A1),
    m_aper_A2(aper_A2),
    m_aper_A3(aper_A3),
    m_aper_A4(aper_A4),
    m_xb(xb),
    m_side(side),
    m_bender(bender),
    m_label(label){
    if (m_apertype == 1){ assert(m_aper_A1 != 0.);}
    if (m_apertype == 2 || m_apertype == 3){ assert(m_aper_A3 != 0. and  m_aper_A4 != 0.);}
  }
  IBeamElement::ConstPtr_t Magnet::clone() const{
    IBeamElement::ConstPtr_t pm(new Magnet(*this));
    return pm;
  }

  double Magnet::s_y0IP  = 0.;
  double Magnet::s_ay0IP = 0.;
  
  double Magnet::s_x0IP  = 0.;
  double Magnet::s_ax0IP = 0.;
  
  
  double Magnet::frontFace()  const {return m_frontFace;}
  double Magnet::rearFace()   const {return m_rearFace;}
  double Magnet::zsignedpos() const {return m_center.z();}
  double Magnet::zabspos()    const {return std::fabs(m_center.z());}
  Point  Magnet::position()   const {return m_center;}
  Side   Magnet::side()       const {return m_side;}
  

  void Magnet::set_y0IP(double v) {s_y0IP  = v;}
  void Magnet::set_ay0IP(double v){s_ay0IP = v;}
  void Magnet::set_x0IP(double v) {s_x0IP  = v;}
  void Magnet::set_ax0IP(double v){s_ax0IP = v;}
  
  bool Magnet::isEndElement() const{return false;}
  
  void Magnet::track(IParticle& particle) const{
  
    particle.setBeamCoordinateShift(this);
    if ( particle.isOutOfAperture() ){return;}
    
    
    // check if particle x,y coordinates wrt to the magnet are in the aperture. 
    if( this->isOutOfAperture( particle ) ){
      //std::cout<<"lost at front face "<<std::endl;
      particle.setOutOfAperture(true);
      return;
    }
    
    // update the particle position and direction affected by the bending of the magnet.
    m_bender->bend(particle);
    particle.z() = m_rearFace;

    /*
    std::cout<<"Magnet::track after bend   "<<this->m_frontFace<<" "<<this->label()<<'\n'
	     <<"\n position    "<<particle.position()
	     <<"\n p dir       "<<particle.direction()
	     <<"\n m pos       "<<this->position()<<'\n'<<std::endl;
    

    std::cout<<particle<<'\n';
    */

    if ( this->isOutOfAperture( particle ) )
      {
	particle.setOutOfAperture(true);
      } 

  }
  
  void Magnet::calibrate(IParticle& particle) const
  {
    return this->track(particle);
  }

  bool Magnet::isOutOfAperture(IParticle& particle) const{

    TransversePoint offCenter = particle.displacement();
    if(m_apertype == 1)
      { 
	
	double radius  = m_aper_A1;
	bool out =  offCenter.mag2() > radius*radius; 
	
	if(out)
	  {
	    std::ostringstream ost;
	    ost<<"pos "<<offCenter.mag2()<<" radiusSq "<<radius*radius<<'\n';
	    particle.addMsg( ost.str() );
	    return true;
	  }
	
	return false;
      }

    if(m_apertype == 2) 
      {
	double part_x = offCenter.x(); 
	double part_y = offCenter.y(); 
	
	double xx = part_x/m_aper_A3;
	double yy = part_y/m_aper_A4;
	
	bool out  =  xx*xx + yy*yy >1.;
	if(out)
	  {
	    std::ostringstream ost;
	    ost<<"outside for aperture type 2 x xx, xaper "<<part_x<<" "<<xx<<" "<<m_aper_A3<<" y yy yaper "<<part_y<<" "<<yy<<" "<<m_aper_A4<<'\n';
	    particle.addMsg( ost.str() );
	    return true;
	  }
	
	
	if ( m_aper_A1>0.)
	  {
	    if( std::fabs(part_x)>m_aper_A1 )
	      {
		std::ostringstream ost;
		ost<<"outside for aperture type 2 x test: particle x  "<<part_x<<" m_aper_A1 "<<m_aper_A1<<"\n";
		particle.addMsg( ost.str() );
		return true;
	      }
	  }
	if ( m_aper_A2>0.)
	  {
	    if( std::fabs(part_y)>m_aper_A2)
	      {
		std::ostringstream ost;
		ost<<"outside for aperture type 2 y test: particle y  "<<part_y<<" m_aper_A2 "<<m_aper_A2<<"\n";
		particle.addMsg( ost.str() );
		return true;
	      }
	  }
	 
	return false;
      }
    if(m_apertype == 3) 
      {
	double part_x = offCenter.x(); 
	double part_y = offCenter.y(); 
	
	if ( m_aper_A1 > 0. )
	  {
	    if( std::fabs(part_x)>m_aper_A1 )
	      {
		std::ostringstream ost;
		ost<<"outside for aperture type 3 x test: particle x  "<<part_x<<" m_aper_A1 "<<m_aper_A1<<"\n";
		particle.addMsg( ost.str() );
		return true;
	      }
	  }
	if ( m_aper_A2 > 0. )
	  {
	    if( std::fabs(part_y)>m_aper_A2)
	      {
		std::ostringstream ost;
		ost<<"outside for aperture type 3 y test: particle y  "<<part_y<<" m_aper_A2 "<<m_aper_A2<<"\n";
		particle.addMsg( ost.str() );
		return true;
	      }
	  }
	if ( m_aper_A3 > 0. && m_aper_A4 > 0. )
	  {
	    float B3=std::tan(m_aper_A3)*m_aper_A1;
	    float B4=m_aper_A2/std::tan(m_aper_A4);
	    float d=(m_aper_A1-std::fabs(part_x))/(m_aper_A1-B4)+(m_aper_A2-std::fabs(part_y))/(m_aper_A2-B3);
	    
	    if(d<1.)
	    {
	      std::ostringstream ost;
	      ost<<"outside for aperture type 3 test: particle x  "<<part_x<<" particle y  "<<part_y<<"; m_aper_A1 "<<m_aper_A1<<"  m_aper_A2 "<<m_aper_A2<<"  m_aper_A3 "<<m_aper_A3<<"  m_aper_A4 "<<m_aper_A4<<"\n";
	      particle.addMsg( ost.str() );
	      return true;
	    }
	  }
	 
	return false;
      }
    
    // aperture is checked anly if aperture type i s 1, 2, or 3
    return false;
  }


  std::string Magnet::str() const {
    std::ostringstream ost;
    ost <<" \n\n-- Magnet  --\n"
	<< "label      "  << m_label     <<'\n'
	<< "center     "  << m_center    <<'\n'
	<< "frontFace  "  << m_frontFace <<'\n'
	<< "rearFace   "  << m_rearFace  <<'\n'
	<< "m_strength "  << m_strength  <<'\n'
	<< "m_apertyp  "  << m_apertype  <<'\n'
	<< "m_aper_A1  "  << m_aper_A1   <<'\n'
	<< "m_aper_A2  "  << m_aper_A2   <<'\n'
	<< "m_aper_A3  "  << m_aper_A3   <<'\n'
	<< "m_aper_A4  "  << m_aper_A4   <<'\n'
	<< "m_xb       "  << m_xb        <<'\n'
	<< "m_side     "  << m_side      <<'\n'
	<< "s_y0IP     "  << s_y0IP      <<'\n'
	<< "s_ay0IP    "  << s_ay0IP     <<'\n'
	<< "s_x0IP     "  << s_x0IP      <<'\n'
	<< "s_ax0IP    "  << s_ax0IP     <<'\n';
    return ost.str();
  }
  std::ostream& operator<<(std::ostream& os, const Magnet& mag){
    os << mag.str();
    return os;
    }
}

