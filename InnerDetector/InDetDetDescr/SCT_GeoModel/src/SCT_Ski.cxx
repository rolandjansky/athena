/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Ski.h"
#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_Module.h"
#include "SCT_GeoModel/SCT_BaseBoard.h" 
#include "SCT_GeoModel/SCT_Dogleg.h"
#include "SCT_GeoModel/SCT_CoolingBlock.h"
#include "SCT_GeoModel/SCT_CoolingPipe.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

SCT_Ski::SCT_Ski(const std::string & name,
                 SCT_Module * module,
                 int stereoSign,
                 double tilt,
                 double length,
                 InDetDD::SCT_DetectorManager* detectorManager,
                 const SCT_GeometryManager* geometryManager,
                 SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_stereoSign(stereoSign),
    m_tilt(tilt), 
    m_length(length), 
    m_module(module)
{
  getParameters();
  m_logVolume = preBuild();
}


SCT_Ski::~SCT_Ski()
{
  if (m_refPointTransform) m_refPointTransform->unref();
  if (m_coolingPipePos) m_coolingPipePos->unref();
}


void
SCT_Ski::getParameters()
{

  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();

  m_safety       = generalParameters->safety();
  
  m_radialSep     = parameters->skiRadialSep();
  m_firstStagger  = parameters->skiFirstStagger();
  m_modulesPerSki = parameters->modulesPerSki();
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++){
    m_zPos.push_back(parameters->skiZPosition(iModule));
    m_id.push_back(parameters->skiModuleIdentifier(iModule));
  }

  // Various offset for the doglegs, cooling blocks and cooling pipe.
  // These are not currently needed and are all set to zero.
  m_doglegRadialOffset = 0;
  m_doglegPhiOffset = 0;
  m_coolingBlockPhiOffset = 0;
  m_coolingBlockRadialOffset = 0;
  m_coolingPipeRadialOffset = 0;
  m_coolingPipePhiOffset = 0;

  m_coolingBlockOffsetX = parameters->coolingBlockOffsetX();
  m_coolingBlockOffsetY = parameters->coolingBlockOffsetY();
  m_coolingBlockOffsetZ = parameters->coolingBlockOffsetZ();
  m_coolingPipeOffsetX = parameters->coolingPipeOffsetX();
  m_coolingPipeOffsetY = parameters->coolingPipeOffsetY();

  m_doglegOffsetX = parameters->doglegOffsetX();
  m_doglegOffsetY = parameters->doglegOffsetY();
}

const GeoLogVol * 
SCT_Ski::preBuild()
{
  const double rphiClearance = 0.5*Gaudi::Units::mm;
  const double radialClearance = 0.5*Gaudi::Units::mm;


  // Make components.
  m_dogleg = std::make_unique<SCT_Dogleg>(getName()+"Dogleg", m_detectorManager, m_geometryManager, m_materials);
  m_coolingBlock = std::make_unique<SCT_CoolingBlock>(getName()+"CoolingBlock",
                                        m_detectorManager, m_geometryManager, m_materials);
  m_coolingPipe = std::make_unique<SCT_CoolingPipe>(getName()+"CoolingPipe", m_length,
                                      m_detectorManager, m_geometryManager, m_materials);

  // We need the sign of the tilt in numerous places
  int tiltSign = (m_tilt < 0) ? -1 : +1;


  // The two modules are lined up in x in the unrotated ski. The ski then gets tilted
  // when placed in the layer.
  // So the x offset is just the raidal separation and the y offset is zero.

  double xModuleOffset = 0.5 * m_radialSep;

  double yModuleOffset = 0.0;


  //
  // Calculate position of cooling block
  //
  // Position the cooling block m_coolingBlockPhiOffset from the lower egde 
  // of the module and m_coolingBlockRadialOffset from surface of the module.

  double xCoolingBlockOffset = - m_coolingBlockRadialOffset + coolingBlockOffsetX();
  double yCoolingBlockOffset = tiltSign *(coolingBlockOffsetY() - m_coolingBlockPhiOffset);
  double zCoolingBlockOffset = coolingBlockOffsetZ(); 

  //
  // Calculate position of dogleg
  //

  double xDoglegOffset = doglegOffsetX() - m_doglegRadialOffset; 

  // NB length of dogleg runs in phi direction.
  double yDoglegOffset =  tiltSign * (doglegOffsetY() + m_doglegPhiOffset);

  double zDoglegOffset = coolingBlockOffsetZ();

  //
  // Calculate position of cooling pipe.
  //
  double xCoolingPipePos = coolingPipeOffsetX() - m_coolingPipeRadialOffset;
  double yCoolingPipePos = coolingPipeOffsetY() + m_coolingPipePhiOffset; 
  m_coolingPipePos = new GeoTransform(GeoTrf::Translate3D(xCoolingPipePos, yCoolingPipePos, 0));
  m_coolingPipePos->ref();
  //
  //
  //

  int stagger_sign = m_firstStagger;
  // Loop over modules in ski as we need their z positions.
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
    
    // For compatibility with AGE we need to offset the module in y by -0.5*sep*sin(tilt) so that center 
    // of module lines up with radial line. (Not sure if this is what is done in reality)

    // Not sure if this needs to be alignable
    double xModulePos = stagger_sign * xModuleOffset;
    double yModulePos = yModuleOffset;
    double zModulePos = m_zPos[iModule];
    
    // There is only one type of module and this is rotated one way or the other
    // to get the phi-v (-ve), u-phi (+ve) orientations
    GeoTrf::RotateX3D rot(0.5 * m_stereoSign * m_module->stereoAngle());
    GeoTrf::Translation3D pos(xModulePos, yModulePos, zModulePos);
    m_modulePos.push_back(GeoTrf::Transform3D(pos*rot));

    //
    // Calculate position of cooling block
    //

    double  xCoolingBlockPos = xCoolingBlockOffset + xModulePos;
    double  yCoolingBlockPos = yCoolingBlockOffset + yModulePos;
    double  zCoolingBlockPos = zCoolingBlockOffset + zModulePos;
    m_coolingBlockPos.push_back(new GeoTransform(GeoTrf::Translate3D(xCoolingBlockPos, yCoolingBlockPos, zCoolingBlockPos))); 

    
    //
    // Calculate position of dogleg
    //
    double xDoglegPos = xDoglegOffset + xModulePos; 
    double yDoglegPos = yDoglegOffset + yModulePos; 
    double zDoglegPos = zDoglegOffset + zModulePos;
    m_doglegPos.push_back(new GeoTransform(GeoTrf::Translate3D(xDoglegPos, yDoglegPos, zDoglegPos))); 

    // alternate staggering
    stagger_sign = - stagger_sign; 

   
  }

  //
  // calculate envelope for ski with just modules. Not all these are used.
  // These are coordinates of corners of module's envelopes.
  //

  GeoTrf::Vector3D c0(0.0,
                       -(m_module->env1RefPointVector()->y()) + 0.5*(m_module->env1Width()),
                       -(m_module->env1RefPointVector()->z()) + 0.5*(m_module->env1Length()));
  GeoTrf::Vector3D c1(0.0,
                       -(m_module->env1RefPointVector()->y()) - 0.5*(m_module->env1Width()),
                       -(m_module->env1RefPointVector()->z()) + 0.5*(m_module->env1Length()));
  GeoTrf::Vector3D c2(0.0,
                       -(m_module->env1RefPointVector()->y()) - 0.5*(m_module->env1Width()),
                       -(m_module->env1RefPointVector()->z()) - 0.5*(m_module->env1Length()));
  GeoTrf::Vector3D c3(0.0,
                       -(m_module->env1RefPointVector()->y()) + 0.5*(m_module->env1Width()),
                       -(m_module->env1RefPointVector()->z()) - 0.5*(m_module->env1Length()));
  GeoTrf::Vector3D c4(0.0,
                       -(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
                       -(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  GeoTrf::Vector3D c5(0.0,
                       -(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
                       -(m_module->env2RefPointVector()->z()) + 0.5*(m_module->env2Length()));
  GeoTrf::Vector3D c6(0.0,
                       -(m_module->env2RefPointVector()->y()) - 0.5*(m_module->env2Width()),
                       -(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));
  GeoTrf::Vector3D c7(0.0,
                       -(m_module->env2RefPointVector()->y()) + 0.5*(m_module->env2Width()),
                       -(m_module->env2RefPointVector()->z()) - 0.5*(m_module->env2Length()));

  double moduleYMax = c4.y();
  double moduleYMin = c5.y();
  c0 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c0;
  c1 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c1;
  c2 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c2;
  c3 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c3;
  c4 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c4;
  c5 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c5;
  c6 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c6;
  c7 = GeoTrf::RotateX3D(0.5 * m_stereoSign * m_module->stereoAngle())*c7;
  
  moduleYMax = std::max(std::max(c0.y(), c4.y()), std::max(c7.y(), c3.y()));
  moduleYMin = std::min(std::min(c1.y(), c5.y()), std::min(c6.y(), c2.y()));
  
  double skiWidth = moduleYMax - moduleYMin + 2*rphiClearance;

  //
  // NB. The envelope is centered on x.
  //
  double skiThickness = m_module->thickness() + 2 * std::abs(xModuleOffset) + radialClearance;
            
  // Due to the cooling block and dogleg being next to the module and
  // as the modules are staggered it is no longer possible to have a
  // simple envelope around the ski.

  // Calculate envelope needed. This is a boolean of two rectangles. 
  // 1. Contains the modules and dogleg.
  // 2. Contains the cooling pipe and cooling block.

  double xmax1 = 0.5*skiThickness;
  double xmin1 = -xModuleOffset + xDoglegOffset - 0.5*m_dogleg->thickness();
  double xmax2 = xmax1;  
  double xmin2 = xCoolingPipePos - m_coolingPipe->pipeRadius();

  // Take into account possiblity that the dogleg extends past the module.

  double ymax1 = moduleYMax + rphiClearance;
  double ymin1 = moduleYMin - rphiClearance;
  double ymin2,ymax2;
  if (tiltSign > 0) {
    ymax2 = std::max(-yModuleOffset + yCoolingBlockOffset + 0.5*m_coolingBlock->width(),
                     yCoolingPipePos + m_coolingPipe->pipeRadius());
    ymin2 = ymin1;
  } else {
    ymax2 = ymax1;
    ymin2 = std::min(-yModuleOffset + yCoolingBlockOffset - 0.5*m_coolingBlock->width(),
                     yCoolingPipePos - m_coolingPipe->pipeRadius());
  }
  

  // Add some safety
  xmin1 -= m_safety;
  xmax1 += m_safety;
  ymin1 -= m_safety;
  ymax1 += m_safety;

  xmin2 -= m_safety;
  xmax2 += m_safety;
  ymin2 -= m_safety;
  ymax2 += m_safety;

  double xCenter = 0.5*(xmin1+xmax1);
  double yCenter = 0.5*(ymin1+ymax1);
  double xShift2 = 0.5*(xmin2+xmax2) - xCenter;
  double yShift2 = 0.5*(ymin2+ymax2) - yCenter;

  m_refPointTransform = new GeoTransform(GeoTrf::Translate3D(-xCenter, -yCenter, 0));
  m_refPointTransform->ref();

  m_env1RefPointVector = std::make_unique<GeoTrf::Vector3D>(-xCenter, -yCenter, 0.0);
  m_env2RefPointVector = std::make_unique<GeoTrf::Vector3D>(-xShift2, -yShift2, 0.0);
  m_env1Thickness      = xmax1-xmin1;
  m_env1Width          = ymax1-ymin1;
  m_env2Thickness      = xmax2-xmin2;
  m_env2Width          = ymax2-ymin2;

  GeoBox * envelope1 = new GeoBox(0.5 * (xmax1-xmin1), 0.5 * (ymax1-ymin1), 0.5 * m_length);
  GeoBox * envelope2 = new GeoBox(0.5 * (xmax2-xmin2), 0.5 * (ymax2-ymin2), 0.5 * m_length);

  const GeoShape * skiEnvelopeShape = 0;
    
  const GeoShape & tmpShape = (*envelope1).
    add(*envelope2 << GeoTrf::Translate3D(xShift2, yShift2, 0));
  skiEnvelopeShape = &tmpShape;
  
  GeoLogVol * skiLog = new GeoLogVol(getName(), skiEnvelopeShape, m_materials->gasMaterial());

  // Make names once only so we don't recreate them again again.
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
    // Add identifier to name.
    std::ostringstream name;
    name << "Module#" << m_id[iModule];
    m_nameTag.push_back(new GeoNameTag(name.str()));
  }


  // Define thickness, width, and length. This is chosen as bounding box centered on the ski rotation axis
  // which contains the modules and dogleg (ie cooling blocks and cooling pipe is ignored.)
  m_thickness = 2 * std::abs(xmin1);
  m_width = skiWidth;


  // Calculate the clearances. Module envelope1 is the thickness up to the sensors. This is used for the module to 
  // module distance

  return skiLog;

}


GeoVPhysVol * 
SCT_Ski::build(SCT_Identifier id)
{
  GeoPhysVol * ski = new GeoPhysVol(m_logVolume);

 
  for (int iModule = 0; iModule < m_modulesPerSki; iModule++) {
  
    // Add modules. 
    ski->add(m_refPointTransform);
    GeoAlignableTransform * moduleTransform = new GeoAlignableTransform(m_modulePos[iModule]);
    ski->add(moduleTransform);
    ski->add(m_nameTag[iModule]);
    ski->add(new GeoIdentifierTag(m_id[iModule]));
    id.setEtaModule(m_id[iModule]); // Set identifier.
    GeoVPhysVol * modulePV = m_module->build(id);
    ski->add(modulePV);  
    
    // Store alignable transform
    m_detectorManager->addAlignableTransform(1, id.getWaferId(), moduleTransform, modulePV);
 
    // Add dogleg
    ski->add(m_refPointTransform);
    ski->add(m_doglegPos[iModule]);
    ski->add(m_dogleg->getVolume());

    // and coolingblock
    ski->add(m_refPointTransform);
    ski->add(m_coolingBlockPos[iModule]);
    ski->add(m_coolingBlock->getVolume());
  }

  // Add Cooling pipe
  ski->add(m_refPointTransform);
  ski->add(m_coolingPipePos);
  ski->add(m_coolingPipe->getVolume());
  
  return ski;
}
