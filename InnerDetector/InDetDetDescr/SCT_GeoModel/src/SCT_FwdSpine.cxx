/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// Simulation for the COMBINED TESTBEAM 2004 //
//                                           //
//       Zdenka.Broklova@matfyz.cz           //
//       Carlos.Escobar@ific.uv.es           //
//       Peter.Kodys@ific.uv.es              //
//        January 23, 2004                   //
///////////////////////////////////////////////

#include "SCT_GeoModel/SCT_FwdSpine.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

SCT_FwdSpine::SCT_FwdSpine(const std::string & name,
         int ringType)
  : SCT_SharedComponentFactory(name), m_ringType(ringType)
{
  getParameters();
  m_physVolume = build();
}



void
SCT_FwdSpine::getParameters()
{
  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();
 
  m_materialName = parameters->fwdSpineMaterial(m_ringType);

  //Main spine
  m_thickness   = parameters->fwdSpineThickness(m_ringType);
  m_width       = parameters->fwdSpineWidth(m_ringType);


  m_moduleCenterToEndTmp = parameters->fwdSpineEndToModuleCenter(m_ringType);
  // FIXME: More accurate definition is
  m_moduleCenterToEnd = parameters->fwdModuleDistBtwMountPoints(m_ringType) 
    - parameters->fwdModuleMountPoint(m_ringType)
    + parameters->fwdSpineEndLocatorToEndMount(m_ringType);
  //m_length      = parameters->fwdSpineEndToModuleCenter(m_ringType) 
  //  + parameters->fwdModuleMountPoint(m_ringType) 
  //  - parameters->fwdHybridMountPointToInnerEdge()
  //  - parameters->fwdModuleHybridEdgeToSpine();
  // FIXME: Better definition would be
  m_length = parameters->fwdModuleDistBtwMountPoints(m_ringType) + parameters->fwdSpineEndLocatorToEndMount(m_ringType)
    - parameters->fwdHybridMountPointToInnerEdge()
    - parameters->fwdModuleHybridEdgeToSpine(m_ringType);
   
  // (zssp[m_ringType].spndox + zsmi[m_ringType].mountd - zhyb->hybysh - zhyb->hybgap0) * Gaudi::Units::cm;

}

GeoVPhysVol *
SCT_FwdSpine::build()
{

 
  // Build the spine. Just a simple box. Its orientated for building a module in a trapezoid so the spine
  // has its length along the z axis, width along the y axis and thickness along the x axis.
  
  const GeoBox * spineShapeMain = new GeoBox( 0.5*m_thickness, 0.5*m_width, 0.5*m_length);
 
  double position;

  int HybPos;
  if (m_ringType == 0) {  HybPos = -1; } // outer module, hybrid in inner side
                  else {  HybPos = 1;  } //hybrid in outer side

  // shift between module center and spine center.
  position = (0.5*m_length - m_moduleCenterToEndTmp) *  HybPos; 
  //position = zsmi[m_ringType].mountd - zhyb->hybysh - zhyb->hybgap0 - zssp[m_ringType].spndox;
  
  //position = position / 2. * HybPos;
  
  SCT_MaterialManager materials;

  const GeoShape & spinePosMain = (*spineShapeMain << GeoTrf::TranslateZ3D(position) );
  m_material = materials.getMaterialForVolume(m_materialName, spineShapeMain->volume());  
  GeoLogVol * spineLog = new GeoLogVol(getName(), &spinePosMain, m_material);
  GeoPhysVol * spine = new GeoPhysVol(spineLog);

  return spine;
}
