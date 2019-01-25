/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Layer.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_Module.h"
#include "SCT_SLHC_GeoModel/SCT_Ski.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

namespace InDetDDSLHC {

inline double sqr(double x) {return x * x;}

SCT_Layer::SCT_Layer(const std::string & name, int iLayer, double barr_lay_diff)
  : SCT_UniqueComponentFactory(name), m_iLayer(iLayer), m_cylLength(barr_lay_diff){
    //first get the parameters (which module type...etc)
    getParameters();
    m_module = new SCT_Module("Module", m_doubleSided, m_staveLayout, m_moduleType);
    m_logVolume = preBuild();
}

SCT_Layer::~SCT_Layer(){
  delete m_module;
  delete m_ski;
}

void SCT_Layer::getParameters(){
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  const SCT_BarrelModuleParameters * moduleParameters = geometryManager()->barrelModuleParameters();
  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();
  
  m_safety       = generalParameters->safety();
  m_radius       = parameters->radius(m_iLayer);
  m_doubleSided  = parameters->doubleSided(m_iLayer);
  m_staveLayout  = parameters->staveLayout(m_iLayer);
  //below, the -m_cylLentgh is for the shield thikenss. see the constructor
  m_cylLength    = parameters->cylLength(m_iLayer)-m_cylLength;
  m_moduleType   = parameters->moduleType(m_iLayer);
  m_modulesPerSki= parameters->modulesPerSki(m_iLayer);
  m_skisPerLayer = parameters->skisPerLayer(m_iLayer);
  m_tilt         = parameters->tilt(m_iLayer);
  m_stereoSign   = parameters->layerStereoSign(m_iLayer);
  //support stuff
  m_supportInnerRadius  = parameters->supportCylInnerRadius(m_iLayer);
  m_supportOuterRadius  = parameters->supportCylOuterRadius(m_iLayer);
  //change material density (carbon fiber here)
  //---m_supportMaterial = materials->getMaterial(parameters->supportCylMaterial(m_iLayer));
  //0.189 is taken from oracle database (CFiberSupport)
  double materialIncreaseFactor = parameters->materialIncreaseFactor(m_iLayer);
  //double cf_density = 0.189*materialIncreaseFactor*GeoModelKernelUnits::g/Gaudi::Units::cm3;
  //m_supportMaterial = materials->getMaterial(parameters->supportCylMaterial(m_iLayer), cf_density);
  m_supportMaterial = materials->getMaterialScaled(parameters->supportCylMaterial(m_iLayer), materialIncreaseFactor);

  int numSegments = moduleParameters->barrelModelSideSegments(m_moduleType);

  std::cout<<"SCT ski per layer : "<<m_skisPerLayer<<std::endl;

  // Set numerology
  // The sensor can be segmented, in which case each segement is considered a separate module for
  // counting purposes
  detectorManager()->numerology().setNumPhiModulesForLayer(m_iLayer,m_skisPerLayer);
  detectorManager()->numerology().setNumEtaModulesForLayer(m_iLayer,m_modulesPerSki*numSegments);

}

const GeoLogVol* SCT_Layer::preBuild(){

  //Create the logical volume: a sphape + material
  std::string layerNumStr = intToString(m_iLayer);
  //Calculations to make the ski(s)
  double divisionAngle  = 360*Gaudi::Units::degree/m_skisPerLayer;
  m_skiPhiStart = 0.5*divisionAngle;
  //Make the ski: this is made from modules 
  m_ski = new SCT_Ski("Ski"+layerNumStr, m_modulesPerSki, m_iLayer, 
		      m_stereoSign, m_cylLength, m_module);
  //calulate the active layer thickness after tilting the ski
  //double layer_thickness = m_ski->thickness()*cos(m_tilt) + m_ski->width()*sin(m_tilt);
  //m_activeInnerRadius = m_radius - 0.5*layer_thickness;
  //m_activeOuterRadius = m_radius + 0.5*layer_thickness;
  activeEnvelopeExtent(m_activeInnerRadius, m_activeOuterRadius);
  m_activeInnerRadius -= m_safety;
  m_activeOuterRadius += m_safety;
  double layer_thickness = m_activeOuterRadius - m_activeInnerRadius; 
  if(m_activeInnerRadius < m_supportOuterRadius){
    m_supportOuterRadius = m_supportOuterRadius - 0.5*layer_thickness - 8.0; // to be save, shift support with 8.0 mm 
    m_supportInnerRadius = m_supportInnerRadius - 0.5*layer_thickness - 8.0;
    std::cout<<"SCT_Layer.cxx ERROR OR WARNING: overlap between active layer (R="<<m_activeInnerRadius
	     <<") and support layer (R="<<m_supportOuterRadius
	     <<"). this is rectified by a hard-coded shift!"<<std::endl;
    if(m_activeInnerRadius < m_supportOuterRadius){
      std::cout<<"SCT_Layer.cxx ERROR OR WARNING: overlap between active layer (R="<<m_activeInnerRadius
	     <<") and support layer (R="<<m_supportOuterRadius
	     <<"). correction not enough, exit athena!"<<std::endl;
      exit(1);
    }
  }
  //make layer envelope (containing both support and active layer)
  m_innerRadius = m_supportInnerRadius;
  m_outerRadius = m_activeOuterRadius;
  // Make envelope for layer
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  const GeoTube* layerEnvelope = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_cylLength);
  //make the logical volume for this layer
  GeoLogVol * layerLog = new GeoLogVol(getName(), layerEnvelope, materials->gasMaterial());
  return layerLog;
}

GeoVPhysVol* SCT_Layer::build(SCT_Identifier id) const{
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  //We make this a fullPhysVol for alignment code.
  GeoFullPhysVol* layer = new GeoFullPhysVol(m_logVolume);
  double divisionAngle  = 360*Gaudi::Units::degree/m_skisPerLayer;
  //Make envelope for active layer
  const GeoTube* activeLayerEnvelope = new GeoTube(m_activeInnerRadius, 
							m_activeOuterRadius, 0.5*m_cylLength);
  GeoLogVol* activeLayerLog = new GeoLogVol(getName()+"Active", activeLayerEnvelope, 
					    materials->gasMaterial());
  GeoPhysVol* activeLayer = new GeoPhysVol(activeLayerLog);
  //build the ski(s)
  for (int iSki=0; iSki<m_skisPerLayer; iSki++){
    std::ostringstream name; name << "Ski#" << iSki;
    double phi = m_skiPhiStart + iSki*divisionAngle;
    //first, rotate the ski to have the right orientation (by default the ski is put in 
    //the center of the layer cylinder with thickness along X, width along Y and length
    //along Z. the rotation is done along the cylinder Z axis.
    GeoTrf::Transform3D rot = GeoTrf::RotateZ3D(m_tilt)*GeoTrf::RotateZ3D(phi);
    //then translate the ski to (R, Phi) position
    GeoTrf::Vector3D pos(m_radius, 0, 0);
    pos = GeoTrf::RotateZ3D(phi)*pos;
    //Because the ski envelope center is not positioned at the rotation axis for the ski
    //we must first apply the inverse of refPointTransform() of the ski.
    //---GeoTrf::Transform3D trans(GeoTrf::Transform3D(rot,pos)*m_ski->getRefPointTransform()
    //--- ->getTransform().inverse());
    GeoTrf::Transform3D trans = GeoTrf::Translate3D(pos.x(),pos.y(),pos.z())*rot;
    activeLayer->add(new GeoAlignableTransform(trans));
    activeLayer->add(new GeoNameTag(name.str()));
    activeLayer->add(new GeoIdentifierTag(iSki));
    id.setPhiModule(iSki);
    activeLayer->add(m_ski->build(id));
  }
  //Make envelope for support cylinder (gas)
  if(m_staveLayout!=2){
    const GeoTube* supportEnvelope = new GeoTube(m_supportInnerRadius, m_supportOuterRadius, 
						 0.5*m_cylLength); 
    GeoLogVol* supportLayerLog = new GeoLogVol(getName()+"Support", supportEnvelope, 
					       materials->gasMaterial());
    GeoPhysVol* supportLayer = new GeoPhysVol(supportLayerLog);
    //build the support cylinder (real support)
    const GeoTube* supportCylEnv = new GeoTube(m_supportInnerRadius, m_supportOuterRadius, 
					       0.5*m_cylLength);
    const GeoLogVol* supportCylLog = new GeoLogVol(getName(), supportCylEnv, m_supportMaterial);
    GeoPhysVol * supportCyl = new GeoPhysVol(supportCylLog);
    //put the cyl in the evelope
    supportLayer->add(supportCyl);
    // Now place all the sub layers into the overall layer.
    
    layer->add(supportLayer);
  }
  layer->add(activeLayer);
  
  
  return layer;
}

void SCT_Layer::activeEnvelopeExtent(double & rmin, double & rmax){
  // We need the radial extent for which we need the coordinates of the corners
  // of the ski envelope after rotation by the tilt angle.
  // NB x and y are swapped wrt to the convention used in other parts of the
  // code. y is in the radial direction and x is in the phi direction.
  
  double thickness = 0.5*m_ski->thickness();
  double width = 0.5*m_ski->width();
  double tilt = std::abs(m_tilt);
  double width_rot = width * cos(tilt/Gaudi::Units::radian) - thickness * sin(tilt/Gaudi::Units::radian);
  
  double thickness_rot = width * sin(tilt/Gaudi::Units::radian) + thickness * cos(tilt/Gaudi::Units::radian);

  rmax = sqrt(sqr(m_radius + thickness_rot) + sqr(width_rot)); 
  rmin = sqrt(sqr(m_radius - thickness_rot) + sqr(width_rot));
}

}
