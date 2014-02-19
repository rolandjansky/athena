/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/IBender.h"
#include "FPTracker/QuadrupoleBender.h"
#include "FPTracker/IParticle.h"
#include "FPTracker/Point.h"
#include "FPTracker/TransversePoint.h"
#include "FPTracker/FPTrackerConstants.h"
#include "FPTracker/IQuadFocuser.h"

#include <vector> // for std::swap...
#include <cmath>
#include <iostream>
#include <iomanip>

namespace FPTracker{
  QuadrupoleBender::QuadrupoleBender(IQuadFocuser::Ptr_t focuser, 
				     double magnetLength,
				     double magnetStrength,
				     int side):
    m_focuser(focuser),
    m_length(magnetLength), 
    m_bendConstant(FPTrackerConstants::qkConst*magnetStrength), 
    m_dside(FPTrackerConstants::aside[side]){
  }


  void QuadrupoleBender::swap(QuadrupoleBender& other){
    std::swap(m_focuser,      other.m_focuser);
    std::swap(m_length,       other.m_length);
    std::swap(m_bendConstant, other.m_bendConstant);
    std::swap(m_dside,        other.m_dside);
  }


  void QuadrupoleBender::bend(IParticle& particle) const {
    // calcualtes new x, y coordinates and direction after deflection

    //particle is at magnet front face. This method moves it to the rear face.


    double qk  = std::sqrt(m_bendConstant/particle.momentum()) ; // ---- basic formula is m Te GeV 
    //qk  = 0.2997925* Magnet_strength[magnet][side]/p ;
    
    
    Point& direction = particle.direction();
    //                                  
    double qkl = qk * m_length*m_dside  ;       
    double qkc = qk *direction[2] ;
    
    m_focuser->focus(qk, qkl, qkc, particle.displacement(), direction);
 

    /*
    std::cout <<std::setprecision(8)
      <<" quad xe "  << m_focuser->xe()
      <<" quad xae " << m_focuser->xae()
      <<" quad  ye " << m_focuser->ye()
      <<" quad yae " << m_focuser->yae()<<'\n';
    */
    
    particle.updatePositionFromDisplacement(TransversePoint(m_focuser->xe(), m_focuser->ye()));
    
    direction[0]  = m_focuser->xae();
    direction[1]  = m_focuser->yae();

    direction[2]  = std::sqrt(1. - (direction[0]*direction[0]) - (direction[1]*direction[1]))*m_dside;
    direction[0]  = direction[0]*direction[2] ;
    direction[1]  = direction[1]*direction[2] ;
    
  }

  
  const std::string QuadrupoleBender::s_label = "QuadBender";
  std::string QuadrupoleBender::label() const {
    return s_label+":"+m_focuser->label();
  }
}
