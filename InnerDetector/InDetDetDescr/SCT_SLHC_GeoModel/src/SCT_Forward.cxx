/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Forward.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_FwdWheel.h"
#include "SCT_SLHC_GeoModel/SCT_ServiceBuilder.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/Units.h"

#include <sstream>
#include <cmath>

namespace InDetDDSLHC {

SCT_Forward::SCT_Forward(const std::string & name)
  : SCT_UniqueComponentFactory(name),
    m_serviceBuilder(0)
{
  getParameters();
  m_logVolume = preBuild();
}

void 
SCT_Forward::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
    
  m_numWheels    = parameters->fwdNumWheels();
  m_innerRadius  = parameters->fwdInnerRadius();
  m_intermediateRadius  = parameters->fwdIntermediateRadius();
  m_outerRadius  = parameters->fwdOuterRadius();
  m_zMin         = parameters->fwdZMin();
  m_zIntermediate  = parameters->fwdZIntermediate();
  m_zMax         = parameters->fwdZMax();
  m_length = m_zMax - m_zMin;  

  // Set numerology
  detectorManager()->numerology().setNumDisks(m_numWheels);
}



const GeoLogVol* SCT_Forward::preBuild(){

  SCT_MaterialManager * materials = geometryManager()->materialManager();
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  int layoutType = parameters->layoutType();
  const GeoLogVol* forwardLog;
  if(layoutType == 1){//projective layout
    //enevelope for small disks
    double len_1 = m_zIntermediate-m_zMin;
    const GeoTube* sct_fwd_1 = new GeoTube(m_innerRadius, m_intermediateRadius, 0.5*len_1);
    //envelope for large disks
    double len_2 = m_zMax-m_zIntermediate;
    const GeoTube* sct_fwd_2 = new GeoTube(m_innerRadius, m_outerRadius, 0.5*len_2);
    //make a single envelope for sct fwd: shift the two parts so that the midle stays at Z=0
    const GeoShape & forwardEnvelopeShape = (*sct_fwd_1 << GeoTrf::TranslateZ3D(-0.5*len_2)).
      add(*sct_fwd_2 << GeoTrf::TranslateZ3D(0.5*len_1));
    forwardLog =  new GeoLogVol(getName(), &forwardEnvelopeShape, materials->gasMaterial());
  }else if(layoutType == 2){//same length barrels
    const GeoTube* forwardEnvelopeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_length);
    forwardLog =  new GeoLogVol(getName(), forwardEnvelopeShape, materials->gasMaterial());
  }else{
    std::cerr<<"---SCT_Forward::preBuild(), ERROR: layoutType "<<layoutType<<" not known! exit athena."<<std::endl;
    exit(1);
  }
  return forwardLog;
}

GeoVPhysVol* SCT_Forward::build(SCT_Identifier id) const{
  GeoFullPhysVol* forward = new GeoFullPhysVol(m_logVolume);
  
  for (int iWheel=0; iWheel<m_numWheels; iWheel++){
    SCT_FwdWheel wheel("Wheel"+intToString(iWheel), iWheel);
    forward->add(new GeoNameTag("Wheel#"+intToString(iWheel))); 
    forward->add(new GeoIdentifierTag(iWheel)); 
    //std::cerr<<"endcap "<<id.getBarrelEC()<<", wheel "<<iWheel;
    id.setLayerDisk(iWheel);
    double zpos = wheel.zPosition() - zCenter();
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::TranslateZ3D(zpos));
    forward->add(transform);
    GeoVPhysVol* wheelPV = wheel.build(id);
    forward->add(wheelPV);
    // Store the alignable transform
    detectorManager()->addAlignableTransform(2, id.getWaferId(), transform, wheelPV);
  } // end loop over wheels

  // Add common/general services
  std::string region = (id.getBarrelEC() > 0) ? "A" : "C";
  if (m_serviceBuilder && m_serviceBuilder->getServMatBuilder()) {
    m_serviceBuilder->getServMatBuilder()->buildAndPlace(region, forward, zCenter());
  }

  return forward;
}
}
