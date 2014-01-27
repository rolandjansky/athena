/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/MagnetFactory.h"
#include "ForwardTracker/IBender.h"
#include "ForwardTracker/BenderDipole.h"
#include "ForwardTracker/BenderQuadrupole.h"

#include <string>
#include <stdexcept>

namespace ForwardTracker {

  std::string label(Magnet::Type type) {

    if (type == Magnet::hbDipole)     return "hbDipole";
    if (type == Magnet::vbDipole)     return "vbDipole";
    if (type == Magnet::hfQuadrupole) return "hfQuadrupole";
    if (type == Magnet::vfQuadrupole) return "vfQuadrupole";

    return "NONE";
  } 
  
  IBender::ConstPtr_t benderFactory(Magnet::Type type,
				    Side         side,
				    double       length,
				    double       strength,
				    double       beamEnergy) {

    if (type == Magnet::hbDipole)     return IBender::ConstPtr_t(new BenderDipole    (side, length, strength, beamEnergy, hBendplane));
    if (type == Magnet::vbDipole)     return IBender::ConstPtr_t(new BenderDipole    (side, length, strength, beamEnergy, vBendplane));
    if (type == Magnet::hfQuadrupole) return IBender::ConstPtr_t(new BenderQuadrupole(side, length, strength, beamEnergy, hBendplane));
    if (type == Magnet::vfQuadrupole) return IBender::ConstPtr_t(new BenderQuadrupole(side, length, strength, beamEnergy, vBendplane));
    
    throw std::logic_error("unknown magnet type");
  }  
  
  Magnet::ConstPtr_t MagnetFactory(double       x,
				   double       y,
				   double       z,
				   double       strength,
				   double       length,
				   std::string  aperType,
				   double       aper1,
				   double       aper2,
				   double       aper3,
				   double       aper4,
				   double       beamEnergy,
				   Side         side,
				   Magnet::Type type) {
    
    IBender::ConstPtr_t bender(benderFactory(type, side, length, strength, beamEnergy));
    
    return Magnet::ConstPtr_t(new Magnet(x, y, z, length, aperType, aper1, aper2, aper3, aper4, side, bender, label(type)));
  }  
}
