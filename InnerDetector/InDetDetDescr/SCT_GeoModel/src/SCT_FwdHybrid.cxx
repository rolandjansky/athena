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

#include "SCT_GeoModel/SCT_FwdHybrid.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelKernel/GeoDefinitions.h"

#include <cmath>


SCT_FwdHybrid::SCT_FwdHybrid(const std::string & name, int ringType)
  : SCT_SharedComponentFactory(name), m_ringType(ringType) 
{
  getParameters();
  m_physVolume = build();
}



void 
SCT_FwdHybrid::getParameters()
{

  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();

  m_materialName  = parameters->fwdHybridMaterial();

  //double Gaudi::Units::radlength;
  //Gaudi::Units::radlength = 18.8 * Gaudi::Units::cm;
  // [Gaudi::Units::cm] for carbon (Partickle Physics Booklet)

  m_thickness  = parameters->fwdHybridThickness();
  m_thickness2 = m_thickness;

  m_length = parameters->fwdHybridLength();
  
  // TRD part
  m_innerWidth = parameters->fwdHybridInnerWidth();
  m_outerWidth = parameters->fwdHybridOuterWidth(); // Width at outer edge
  m_length1    = parameters->fwdHybridLengthToCorner(); 

  // rectangular part.
  m_length2 = m_length  - parameters->fwdHybridLengthToCorner(); 
  m_width2  = m_innerWidth;

  if (m_width2  < m_outerWidth) m_width2  = m_outerWidth;

  m_mountPointToInnerEdge = parameters->fwdHybridMountPointToInnerEdge();
  m_mountPointToOuterEdge = m_length - m_mountPointToInnerEdge;

}

GeoVPhysVol * SCT_FwdHybrid::build() 
{

  const SCT_ForwardModuleParameters * parameters = geometryManager()->forwardModuleParameters();

  // Make a hybrid. This is a TRD + BOX 
  const GeoTrd * hybridShape1 = new GeoTrd(0.5 * m_thickness, 0.5 * m_thickness, 
                                           0.5 * m_innerWidth, 0.5 * m_outerWidth, 
                                           0.5 * m_length1);

  const GeoBox * hybridShape2 = new GeoBox(0.5 * m_thickness2, 0.5 * m_width2, 0.5 * m_length2);
  
  double position = parameters->fwdModuleMountPoint(m_ringType) - (m_mountPointToInnerEdge)
    + 0.5*(m_length2);
  //  double position = zsmi[m_ringType].mountd - zhyb->hybysh + (zhyb->hyby - zhyb->hybycor) / 2.;
  if (m_ringType == 0) { position = -1 * position;  };  // outer module, hybrid in inner side
 
  const GeoShape & hybridPos1 = (*hybridShape2 << GeoTrf::TranslateZ3D(position));
  
  position = parameters->fwdModuleMountPoint(m_ringType) - (m_mountPointToInnerEdge) 
    + m_length - 0.5 * parameters->fwdHybridLengthToCorner();
  //  position = zsmi[m_ringType].mountd - zhyb->hybysh + zhyb->hyby - zhyb->hybycor / 2.;
  if (m_ringType == 0) {  // outer module, hybrid in inner side
    position = -1 * position;  };
  
  double rotation = 0.;
  if (m_ringType == 0)  rotation = 180. * Gaudi::Units::deg;  
  
  const GeoShape & hybridPos2 = (*hybridShape1 << GeoTrf::RotateX3D(rotation)
                      << GeoTrf::TranslateZ3D(position) );
  

  SCT_MaterialManager materials;
  const GeoShapeUnion & hybridShape = hybridPos1.add(hybridPos2);
  // error getting volume directly.
  m_material = materials.getMaterialForVolume(m_materialName, hybridShape1->volume()+hybridShape2->volume());
  const GeoLogVol * hybridLog = new GeoLogVol(getName(), &hybridShape, m_material);
  GeoPhysVol * hybrid = new GeoPhysVol(hybridLog);

  return hybrid;

}

  

