/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds one trapezoid
//
//
#include "PixelGeoModel/GeoPixelFluid.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include<cmath>
#include <sstream>

GeoPixelFluid::GeoPixelFluid(int type)
{
  m_index    = gmt_mgr->PixelFluidIndex(type);
  double z1  = gmt_mgr->PixelFluidZ1(m_index);
  double z2  = gmt_mgr->PixelFluidZ2(m_index);
  m_posX     = gmt_mgr->PixelFluidX(m_index);
  m_posY     = gmt_mgr->PixelFluidY(m_index);
  m_posZ     = 0.5*(z1+z2);
}

GeoVPhysVol* GeoPixelFluid::Build()
{
  //
  // Dimensions
  //
  double z1       = gmt_mgr->PixelFluidZ1(m_index);
  double z2       = gmt_mgr->PixelFluidZ2(m_index);
  double thick1   = gmt_mgr->PixelFluidThick1(m_index);
  double thick2   = gmt_mgr->PixelFluidThick2(m_index);
  double width    = gmt_mgr->PixelFluidWidth(m_index);

  double length = std::abs(z2 - z1);
  double thickAtZmin = thick1;
  //  double thickAtZmax = thick2;
  if (z1 > z2) {
    thickAtZmin = thick2;
    //    thickAtZmax = thick1;
  }    
  
  std::ostringstream o;
  o << "Fluid" << gmt_mgr->PixelFluidType(m_index);
  std::string logName = o.str();

  const GeoMaterial* fluidMat = mat_mgr->getMaterial(gmt_mgr->PixelFluidMat(m_index));
  const GeoTrd* fluidShape  = new GeoTrd(0.5*thickAtZmin, 0.5*thickAtZmin, 0.5*width, 0.5*width, 0.5*length);
  GeoLogVol * fluidLV    = new GeoLogVol(logName, fluidShape, fluidMat);
  GeoPhysVol * fluidPhys = new GeoPhysVol(fluidLV);

  return fluidPhys;
}
