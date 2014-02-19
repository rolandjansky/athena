/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTACKER_MAGNET_H
#define FPTACKER_MAGNET_H

#include "IBeamElement.h"
#include "Point.h"
#include "FPTrackerConstants.h"
#include "IBender.h"
#include "Particle.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#include <iosfwd>

namespace FPTracker{
  class IParticle;
  class TransversePoint;
  class Magnet:public IBeamElement{
  public:
    Magnet(double x,
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
	   IBender::ConstPtr_t bender, 
	   std::string label
	   );
    /*
    Magnet(const Magnet&);
    Magnet& operator=(Magnet);
    */

    double frontFace()    const;
    double rearFace()     const;
    double zabspos()      const;
    double zsignedpos()   const;
    double centerZ()      const; //magnet center
    Point  position()     const;
    Side   side()         const;

    bool   isEndElement() const;
    std::string str()     const;

    static void set_y0IP(double);
    static void set_ay0IP(double);
    static void set_x0IP(double);
    static void set_ax0IP(double);
    
    std::string label()              const;

    void track(IParticle&)           const;
    void calibrate(IParticle&)       const;

    IBeamElement::ConstPtr_t clone() const;


    typedef boost::shared_ptr< const Magnet >     ConstPtr_t;
    typedef std::vector< ConstPtr_t >             Container_t;
    
    enum Type {hbDipole, hfQuadrupole, vfQuadrupole, vbDipole, notAMagnet};




  private:
    static double s_y0IP;
    static double s_ay0IP;
    
    static double s_x0IP ;
    static double s_ax0IP;

    
    Point               m_center;
    double              m_frontFace;
    double              m_rearFace;
    double              m_strength;
    int                 m_apertype;
    double              m_aper_A1;
    double              m_aper_A2;
    double              m_aper_A3;
    double              m_aper_A4;
    double              m_xb;
    Side                m_side;
    IBender::ConstPtr_t m_bender;
    std::string m_label;


    bool isOutOfAperture( IParticle& ) const;
    double frontface()                 const;
    void swap(Magnet&); 
  };


  std::ostream& operator<<(std::ostream&, const Magnet&);
  
}
#endif
