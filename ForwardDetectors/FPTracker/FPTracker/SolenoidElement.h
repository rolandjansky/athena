/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

UNFINISHED
UNFINISHED
UNFINISHED

#ifndef FPTACKER_SOLENOIDELEMENT_H
#define FPTACKER_SOLENOIDELEMENT_H

#include "IBeamElement.h"
#include "FPTrackerConstants.h"
#include "Point.h"
#include <ostream>

/*
 * simumation of the efect of the solenoid for FPTracker. PS 13/07/2011
 * using inputfrom Daniel Pelikan
 *
 * Daniel gives the transfer matrix as:
 *
 * x       |   | C^2   S.C/k     SC      S^2/k   0   0   | |x_0        |
 * x'      |   |-kSC     C^2  -kS^2         SC   0   0   | |x'_0       |
 * y       | = | -SC  -S^2/k    C^2       SC/k   0   0   | |y_0        |
 * y'      |   |kS^2     -SC   -kSC        C^2   0   0   | |y_0'       | 
 * delta_l |   |   0       0      0          0   1 l/g^2 | |delta_l0   |
 * Delp/p  |   |   0       0      0          0   0     1 | |Delp_0/p_0 |
 *
 * FPTracker calculates x, x', y, y' from x_0, x_0', y_0, y_0'
 * so needs the numbers C, S, k calculate the matrix elements
 * which will be stored as t_ij.
 */

namespace FPTracker{
  class IParticle;
  class IBender;
  class Point;
  class TransversePoint;
  class SolenoidElement:public IBeamElement{

  public:

    SolenoidElement(double front,
		    double back,
		    const TransversePoint& beamAxis,
		    Side side,
		    double C,
		    double S,
		    double k);
    SolenoidElement(
		      const IBeamElement::ConstPtr_t&,
		      const IBeamElement::ConstPtr_t&
		      );

    IBeamElement::ConstPtr_t clone() const;
    
    ~SolenoidElement();
    double frontFace()    const;
    double rearFace()     const;
    double zabspos()      const;
    double zsignedpos()   const;
    Point  position()     const;
    Side   side()         const;

    bool   isEndElement() const;
    std::string str()     const;
    
    std::string label() const;

    void track(IParticle&) const;
    void calibrate(IParticle&) const;

    typedef boost::shared_ptr< const SolenoidElement >  ConstPtr_t;

  private:

    double m_frontFace;
    double m_rearFace;
    double m_signedLength;
    double m_absLength;
    Point  m_pos;
    std::string m_label;
    Side m_side;

    double t_00;
    double t_01;
    double t_02;
    double t_03;

    double t_10;
    double t_11;
    double t_12;
    double t_13;

    double t_20;
    double t_21;
    double t_22;
    double t_23;

    double t_30;
    double t_31;
    double t_32;
    double t_33;

  };
}
#endif
