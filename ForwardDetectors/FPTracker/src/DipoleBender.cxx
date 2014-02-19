/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/DipoleBender.h"
#include "FPTracker/IParticle.h"
#include "FPTracker/Point.h"
#include "FPTracker/FPTrackerConstants.h"
#include <cmath>
#include <cassert>
#include <iostream>

namespace FPTracker{
  DipoleBender::DipoleBender(
			     Bendplane bendplane, 
			     double    magnetLength, 
			     double    magnetStrength,  
			     double    pbeam0,
			     int side
			     ):
    m_bendplane(bendplane), 
    m_length(magnetLength), 
    m_strength(magnetStrength), 
    m_pbeam0(pbeam0),
    m_dside(side==0 ? -1.:1.){
  }

  IBender::ConstPtr_t DipoleBender::clone() const{
    IBender::ConstPtr_t ib(new DipoleBender(*this));
    return ib;
  }

  void DipoleBender::bend(IParticle& particle) const {

    // --                       DIPOLE MAGNET
    // tandir is in x,y,z bearing in mind the sign of z/
    // But the magnet strength is stored as its effect on dx/ds or dy/ds.
    // bend =  magnet strength, corrected for momentum, = bend for this proton. 
    // dzc  =  length of magnet, signed
    // position[i] =  New displ. = old + effect of gradient + effect of bend.
    // position[i-1] = old displacement + effect of gradient in non-bending plane.

    // calcualtes new x, y coordinates and direction after deflection

    Point& position  = particle.position();
    Point& direction = particle.direction();

    double tandir    =  direction[m_bendplane]/direction[2] ;
    double bend      =  m_strength*m_pbeam0/particle.momentum();
    double dzc       =  m_length*m_dside;

    //std::cout<<"dir [2] from DipoleBender "<<direction[2]<<'\n';

    position[m_bendplane]   +=   dzc*tandir +(dzc/m_dside)*std::tan(0.5*bend) ;
    position[1-m_bendplane] +=   dzc*direction[1-m_bendplane]/direction[2];
    direction[m_bendplane]   =   (tandir*m_dside +std::tan(bend))/(1.-m_dside*tandir*std::tan(bend)); //update part. angles
    direction[m_bendplane]   =   direction[m_bendplane]/std::sqrt(1. + direction[m_bendplane]*direction[m_bendplane])  ;
    
    // tan(new angle) = tan(sum of orig. angle + angle of bend)
    // Then we turn it into the sine, e.g. dx/ds (perhaps unncessarily)

    //  If main bending magnet, the local coord system must be redefined.
    //  Rotate wrt the local lab coords if necessary, and return to lab frame.
    //  The main bends are curved, so we must do this before evaluating the aperture.
    if(m_length > 10.){ // Identifies MB magnet.
      direction[0]  -= m_strength;         // Rotates the coords.
      position[0] -= 0.5*m_strength*m_length;
      // Shifts coord system laterally.
    }

  }

  const std::string DipoleBender::s_label = "DiplBender";
  std::string DipoleBender::label() const {return s_label;}
}
