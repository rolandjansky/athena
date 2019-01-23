/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Ski.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_Module.h"
#include "SCT_SLHC_GeoModel/SCT_StaveSupport.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

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

namespace InDetDDSLHC {

SCT_Ski::SCT_Ski(const std::string & name,
		 int modulesPerSki,
		 int layer,
		 int stereoSign,
		 double length, 
		 const SCT_Module* module) 
  : SCT_UniqueComponentFactory(name), 
    m_modulesPerSki(modulesPerSki),
    m_layer(layer),
    m_stereoSign(stereoSign),
    m_length(length),
    m_module(module),
    m_staveSupport(0),
    m_staveSupport_pos(0),
    m_staveSupport_neg(0)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Ski::~SCT_Ski(){
  if (m_staveSupport_pos){
    delete m_staveSupport_pos; 
    m_staveSupport_pos=0;
  }
  if (m_staveSupport_neg){
    delete m_staveSupport_neg; 
    m_staveSupport_neg=0;
  }
  if (m_staveSupport){
    delete m_staveSupport; 
    m_staveSupport=0;
  }
}

void SCT_Ski::getParameters(){
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();

  m_radialSep     = parameters->skiRadialSep(m_layer);
  m_firstStagger  = parameters->skiFirstStagger();
  m_stereoOuter   = parameters->stereoOuter(m_layer);
  m_stereoInner   = parameters->stereoInner(m_layer);
  m_staveLayout   = parameters->staveLayout(m_layer);
  for(int iModule=0; iModule<m_modulesPerSki; iModule++){
    m_zPos.push_back(parameters->skiZPosition(m_layer, iModule));
    m_id.push_back(parameters->skiModuleIdentifier(m_layer, iModule));
  }
  m_safety = geometryManager()->generalParameters()->safety();
  if(m_staveLayout==2) m_cylInnerZMin = parameters->cylInnerZMin(m_layer);

}

const GeoLogVol* SCT_Ski::preBuild(){
  
  // Make stave support
  double supportWidth = 0;
  if (m_staveLayout) {

  
    if(m_staveLayout==2){
      

      m_staveSupport_pos = new SCT_StaveSupport("StaveSupport_pos"+intToString(m_layer), m_layer, m_length*0.5 - m_cylInnerZMin - m_safety);
      supportWidth = m_staveSupport_pos->width();
      m_staveSupport_neg = new SCT_StaveSupport("StaveSupport_neg"+intToString(m_layer), m_layer, m_length*0.5 - m_cylInnerZMin -  m_safety);
    }
    
    
    else { m_staveSupport = new SCT_StaveSupport("StaveSupport"+intToString(m_layer), m_layer, m_length - m_safety);
      supportWidth = m_staveSupport->width();
    }
  
  }    

  //calculate the ski envelope (it contains the stereo-rotated module)
  double stereoMax = 0;
  if (m_staveLayout) {
    stereoMax = std::max(std::abs(m_stereoOuter),std::abs(m_stereoInner));
  } else {
    stereoMax = std::abs(0.5*m_module->stereoAngle());
  }
  m_width = m_module->width()*cos(stereoMax) + m_module->length()*sin(stereoMax);
  m_width = std::max(m_width,supportWidth) + m_safety;
  m_thickness = m_module->thickness() + m_radialSep + m_safety;
  //RK: radial separation is measured from the middle of the two modules (check it!)
  if(m_radialSep != 0){
    if(m_radialSep < m_module->thickness()){
      std::cout<<"SCT_Ski.cxx: layer "<<m_layer<<", radial separation should be > than module thickness, "
	       <<"radSep = "<<m_radialSep<<", modthick = "<<m_module->thickness()
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
  }
  const GeoBox* skiEnvelopeShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  SCT_MaterialManager * materials = geometryManager()->materialManager();    
  GeoLogVol* skiLog = new GeoLogVol(getName(), skiEnvelopeShape, materials->gasMaterial());

  // Make stave support
  //if (m_staveLayout) {
  //  m_staveSupport = new SCT_StaveSupport("StaveSupport"+intToString(m_layer), m_layer, m_length - m_safety);
  //}    

  return skiLog;
}

GeoVPhysVol* SCT_Ski::build(SCT_Identifier id) const{
  //crate the physical volume
  GeoPhysVol* ski = new GeoPhysVol(m_logVolume);
  //RK: radial separation is measured from the middle of the modules
  int stagger_sign = m_firstStagger;
  for(int iModule=0; iModule<m_modulesPerSki; iModule++){
    //modules position
    //protections!
    if (m_staveLayout) {
      // stave layout
      // Outer 
      bool flip = false;
      placeModule(ski, id, iModule,  1, 0.5*m_radialSep,  m_zPos[iModule], m_stereoOuter, flip) ;
      // inner
      flip = true; // Rotate 180 deg around z axis
      placeModule(ski, id, iModule,  0, -0.5*m_radialSep, m_zPos[iModule], m_stereoInner, flip) ;
    } else {
      // module layout
      placeModule(ski, id, iModule,  0, stagger_sign*0.5*m_radialSep, m_zPos[iModule], 
		  0.5*m_stereoSign*m_module->stereoAngle(), false) ;
    }
    //alternate staggering
    stagger_sign = -stagger_sign;
  }

  // Place stave support (if its present)
  if (m_staveSupport && m_staveSupport->getVolume()) {
    // centered.
    ski->add(m_staveSupport->getVolume());
  }

   if ((m_staveLayout==2) && m_staveSupport_pos && m_staveSupport_pos->getVolume()) {
    // centered.
    GeoTrf::Translate3D modulePos_pos(0.0, 0.0, (m_length*0.5 - m_cylInnerZMin - m_safety)*0.5 + m_cylInnerZMin);
    GeoTransform* supportTransform_pos = new GeoTransform(modulePos_pos);
    ski->add(supportTransform_pos);
    ski->add(m_staveSupport_pos->getVolume());
  }
  
  if ((m_staveLayout==2) && m_staveSupport_neg && m_staveSupport_neg->getVolume()) {
    // centered.
    GeoTrf::Translate3D modulePos_neg(0.0, 0.0,-(m_length*0.5 - m_cylInnerZMin - m_safety)*0.5 - m_cylInnerZMin);
    GeoTransform* supportTransform_neg = new GeoTransform(modulePos_neg);
    ski->add(supportTransform_neg);
    ski->add(m_staveSupport_neg->getVolume());
  }
  return ski;
}

void
SCT_Ski::placeModule(GeoPhysVol * ski, SCT_Identifier id, int iModule, int side,  
		     double xModulePos,  double zModulePos, double stereoAngle, bool flip) const
{ 
  

  if(fabs(xModulePos)+0.5*m_module->thickness() > 0.5*m_thickness){
    geometryManager()->msg(MSG::ERROR)
      <<"SCT_Ski.cxx: layer "<<m_layer<<", module "<<iModule<<" X position "<<xModulePos<<" should be < than ski half thickness, "
      <<"modXpos+halfModThickness = "<<fabs(xModulePos)+0.5*m_module->thickness()
      <<", halfSkithickness = "<<0.5*m_thickness
      <<". exit athena!" << endmsg;
    exit(1);
  }
  
  double extendedLength  = m_module->length()*cos(stereoAngle) + m_module->length()*sin(std::abs(stereoAngle));
  
  if(fabs(zModulePos)+0.5*extendedLength > 0.5*m_length){
    geometryManager()->msg(MSG::ERROR)
      <<"SCT_Ski.cxx: layer "<<m_layer<<", module Z position problem: module edge Z position should be < than ski half length:\n"
      <<"module Z position:     "<<zModulePos<<"\n"
      <<"module half length:    "<<0.5*m_module->length()<<"\n"
      <<"module edge position:  "<<fabs(zModulePos)+0.5*m_module->length()<<".\n"
      <<"module extended edge position:  "<<fabs(zModulePos)+0.5*extendedLength<<".\n"
      <<"half Ski length:       "<<0.5*m_length<<"\n"
      <<"exit athena!"<< endmsg;
    //Don't exit as this breaks some existing geometries.
    //exit(1);
  }
  GeoTrf::Transform3D rot = GeoTrf::RotateX3D(stereoAngle);
  //the module is rotated, around X axis, one way or the other (u or v)
  if (flip) rot = rot * GeoTrf::RotateZ3D(180*Gaudi::Units::deg);
  GeoTrf::Translation3D pos(xModulePos, 0.0, zModulePos);
  GeoTrf::Transform3D modulePos = GeoTrf::Transform3D(pos*rot);

  //Add modules. 
  GeoAlignableTransform* moduleTransform = new GeoAlignableTransform(modulePos);
  ski->add(moduleTransform);
  //Make names 
  int idTag = m_id[iModule];
  if (m_staveLayout) {
    int sign = (idTag >= 0) ? 1: -1;
    idTag = idTag*2+sign*side;
    //std::cout << "Module copy number: " << idTag << std::endl;
  }
  std::string moduleName = "Module" + intToString(idTag);
  ski->add(new GeoNameTag(moduleName));
  ski->add(new GeoIdentifierTag(idTag));
  //this is for SCT idetifer gymnastics (obsolete with 64 bit Identifer)
  //if(m_id[iModule] < 0) id.setBarrelEC(-1);
  //else id.setBarrelEC(+1);
  id.setEtaModule(m_id[iModule]);
  if (m_staveLayout) {
    id.setSide(side);
  }
  GeoVPhysVol* modulePV = m_module->build(id);
  ski->add(modulePV);  
  //Store alignable transform
  detectorManager()->addAlignableTransform(1, id.getWaferId(), moduleTransform, modulePV);
  
}

}
