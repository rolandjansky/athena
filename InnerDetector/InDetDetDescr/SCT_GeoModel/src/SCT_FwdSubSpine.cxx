/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// Simulation for the COMBINED TESTBEAM 2004 //
//                                           //
//       Zdenka.Broklova@matfyz.cz           //
//       Carlos.Escobar@ific.uv.es           //
//       Peter.Kodys@ific.uv.es              //
//        January 23, 2004                   //
///////////////////////////////////////////////

#include "SCT_GeoModel/SCT_FwdSubSpine.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

#include "GeoModelKernel/Units.h"

#include "GeoModelKernel/GeoDefinitions.h"



#include <cmath>


SCT_FwdSubSpine::SCT_FwdSubSpine(const std::string & name,
                                 int ringType, int spineSide,
                                 InDetDD::SCT_DetectorManager* detectorManager,
                                 const SCT_GeometryManager* geometryManager,
                                 SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials),
    m_ringType(ringType), m_spineSide(spineSide)
{
  getParameters();
  m_physVolume = build();
}



void
SCT_FwdSubSpine::getParameters()
{
  const SCT_ForwardModuleParameters * parameters = m_geometryManager->forwardModuleParameters();

  m_material  = m_materials->getMaterial(parameters->fwdSubSpineMaterial(m_ringType));

  // Width is in direction of module width (ie long axis of arm)
  // Length is in direction of module length (ie short axis of arm)

  //Inner subspine
  m_thickness1   = parameters->fwdSpineThickness(m_ringType);
  m_width1       = parameters->fwdSubSpineInnerWidth(m_ringType);
  m_length1      = parameters->fwdSubSpineInnerLength(m_ringType);
  //Middle subspine
  m_thickness2   = parameters->fwdSpineThickness(m_ringType);
  m_width2       = parameters->fwdSubSpineMiddleWidth(m_ringType);
  m_length2      = parameters->fwdSubSpineMiddleLength(m_ringType);
  //Outer subspine
  m_thickness3   = parameters->fwdSpineThickness(m_ringType);
  m_width3       = parameters->fwdSubSpineOuterWidth(m_ringType);
  m_length3      = parameters->fwdSubSpineOuterLength(m_ringType);
}

GeoVPhysVol * SCT_FwdSubSpine::build()
{
  // x,y,z are in the geomodel module coordinates

  const SCT_ForwardModuleParameters * parameters = m_geometryManager->forwardModuleParameters();
  
  const GeoBox * spineShape1 = new GeoBox( m_thickness1/2., m_width1/2., m_length1/2.);
  const GeoBox * spineShape2 = 0;
  if (m_ringType != 3) spineShape2 = new GeoBox( m_thickness2/2., m_width2/2., m_length2/2.);
  const GeoBox * spineShape3 = new GeoBox( m_thickness3/2., m_width3/2., m_length3/2.);


  double position, position2;

  int HybPos;
  
  if (m_ringType == 0) {  HybPos = -1; } // outer module, hybrid in inner side
                  else {  HybPos = 1;  } //hybrid in outer side
     

  double widthMain =  parameters->fwdSpineWidth(m_ringType);  // width of main spine

  
  const GeoShape * subSpineMiddle = 0;
  const GeoShape * subSpineShape = 0;

  // Inner sub spine
  double mountPosition = parameters->fwdModuleMountPoint(m_ringType); // mount point to physics center
  position  = mountPosition - parameters->fwdSubSpineInnerRefDist(m_ringType) - 0.5*m_length1;
  position = position * HybPos;
  position2 = (widthMain + m_width1) / 2.;
  const GeoShape & spinePos1 = (*spineShape1 << GeoTrf::TranslateY3D(m_spineSide * position2) << GeoTrf::TranslateZ3D(position) );
  

  // Middle sub spine
  // Not defined for inner module.
  if (m_ringType != 3) {
    position  = mountPosition - parameters->fwdSubSpineMiddleRefDist(m_ringType) - 0.5*m_length2;
    position = position * HybPos;
    position2 = (widthMain + m_width2) / 2.;
    const GeoShape & spinePos2 = (*spineShape2 << GeoTrf::TranslateY3D(m_spineSide * position2) << GeoTrf::TranslateZ3D(position) );
    subSpineMiddle = &spinePos2;
  }

  // Outer sub spine
  position  = mountPosition - parameters->fwdSubSpineOuterRefDist(m_ringType) - 0.5*m_length3;
  position = position * HybPos;
  position2 = (widthMain + m_width3) / 2.;
  const GeoShape & spinePos3 = (*spineShape3 << GeoTrf::TranslateY3D(m_spineSide * position2) << GeoTrf::TranslateZ3D(position) );

  const GeoShapeUnion & spineTmp  = (spinePos1).add(spinePos3);
  if (m_ringType != 3) {
    const GeoShapeUnion & spineTmp2  = spineTmp.add(*subSpineMiddle);
    subSpineShape = &spineTmp2;
  } else { 
    subSpineShape = &spineTmp;
  } 

  GeoLogVol * spineLog = new GeoLogVol(getName(), subSpineShape, m_material);
  GeoPhysVol * spine = new GeoPhysVol(spineLog);

  return spine;
}
