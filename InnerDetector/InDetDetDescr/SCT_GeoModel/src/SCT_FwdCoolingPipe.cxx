/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdCoolingPipe.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <cmath>

inline double sqr(double x) {return x * x;}

SCT_FwdCoolingPipe::SCT_FwdCoolingPipe(const std::string & name, 
                                       int numPipes, 
                                       double innerRadius, 
                                       double startPos, 
                                       double endPos)
  : SCT_SharedComponentFactory(name), m_numPipes(numPipes), m_innerRadius(innerRadius)
{
  m_length = std::abs(endPos - startPos);
  m_zPosition = 0.5 * (startPos + endPos);
  
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdCoolingPipe::getParameters()
{ 
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  SCT_MaterialManager materials;
    
  m_material    = materials.getMaterial(parameters->fwdCoolingPipeMaterial());
  m_pipeRadius  = parameters->fwdCoolingPipeRadius();
}

GeoVPhysVol * 
SCT_FwdCoolingPipe::build() 
{
  // Calculate the dimensions.
  // area = Gaudi::Units::pi*(pipeRadius)^2 * numPipes
  // also area = 2*Gaudi::Units::pi*r_ave*delta_r approx= 2 * Gaudi::Units::pi * rMin * delta_r
  // solve for delta_r
  // m_thickness = delta_r

  double area = Gaudi::Units::pi * sqr(m_pipeRadius) * m_numPipes;
  m_thickness = area/(2. * Gaudi::Units::pi * m_innerRadius);
  m_outerRadius = m_innerRadius +  m_thickness;


  // Make the support disk. A simple tube.
  const GeoTube * pipeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * pipeLog = new GeoLogVol(getName(), pipeShape, m_material);

  GeoPhysVol * pipe = new GeoPhysVol(pipeLog);

  return pipe;
}

