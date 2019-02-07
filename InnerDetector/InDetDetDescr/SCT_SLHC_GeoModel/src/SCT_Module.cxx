/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_Module.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_InnerSide.h"
#include "SCT_SLHC_GeoModel/SCT_OuterSide.h"
#include "SCT_SLHC_GeoModel/SCT_BaseBoard.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>

namespace InDetDDSLHC {

SCT_Module::SCT_Module(const std::string & name, bool doubleSided, bool staveLayout, int moduleType)
  : SCT_UniqueComponentFactory(name),
    m_doubleSided(doubleSided),
    m_staveLayout(staveLayout),
    m_moduleType(moduleType),
    m_innerSide(0),
    m_outerSide(0),
    m_baseBoard(0)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Module::~SCT_Module(){
  // Clean up
  delete m_innerSide;
  delete m_outerSide; // Initialized to zero so safe to delete if not used.
  delete m_baseBoard; //   ditto
}


void SCT_Module::getParameters(){
  const SCT_BarrelModuleParameters* parameters = geometryManager()->barrelModuleParameters();
  m_stereoAngle   = parameters->moduleStereoAngle(m_moduleType);
  m_interSidesGap = parameters->moduleInterSidesGap(m_moduleType);
}


const GeoLogVol* SCT_Module::preBuild(){
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  m_innerSide = new SCT_InnerSide("InnerSide", m_moduleType, m_staveLayout);
  if(!m_staveLayout) {
    m_baseBoard = new SCT_BaseBoard("BaseBoard", m_moduleType);
  }
  if(!m_staveLayout && m_doubleSided) {
    m_outerSide = new SCT_OuterSide("OuterSide", m_moduleType);
  }
  //calculate the dimensions of the module envelope (contain the rotated sensors):
  //1) Calculate the width 
  //side dimensions (module dimensions before stereo rotation)
  double sideWidth, sideLength;
  double half_stereo = 0.5*m_stereoAngle;
  if(m_staveLayout){
    //coverity investigation sroe 7/11/2014:
    //How can this be sensible? : The m_baseboard parameter is only defined if (!m_staveLayout)
    //but the code here dereferences it  and only executes if (m_staveLayout)
    // Sensor only if placing sensors directly on stave
    m_width   = m_innerSide->width();
    m_length  = m_innerSide->length();
    m_thickness = m_innerSide->thickness() + 0.01*Gaudi::Units::mm;// Not really necessary but doesn't hurt
  } else if(m_doubleSided){
    sideWidth  = std::max(m_innerSide->width(), m_outerSide->width());
    sideLength = std::max(m_innerSide->length(), m_outerSide->length());
    m_width    = std::max(sideWidth*cos(half_stereo/Gaudi::Units::radian) + sideLength*sin(half_stereo/Gaudi::Units::radian),
			  m_baseBoard->width());
    m_length   = std::max(sideWidth*sin(half_stereo/Gaudi::Units::radian) + sideLength*cos(half_stereo/Gaudi::Units::radian), 
			  m_baseBoard->length());
    double interSidesGap = std::max(m_baseBoard->thickness(), m_interSidesGap);
    m_thickness = m_innerSide->thickness() + m_outerSide->thickness() + interSidesGap + 0.01;//0.01mm safety necessary
    //std::cerr<<"----interSidesGap "<<interSidesGap<<"  m_thickness "<< m_thickness<<std::endl;
  }else{
    m_width   = std::max(m_innerSide->width(), m_baseBoard->width());
    m_length  = std::max(m_innerSide->length(), m_baseBoard->length());
    m_thickness = m_innerSide->thickness()+m_baseBoard->thickness() + 0.01;//0.01mm safety necessary
  }
   //Make an envelope for the whole module.
  // No need for an extra envelope for stave layout.
  GeoLogVol* moduleLog = 0;
  if (!m_staveLayout) {
    GeoBox* moduleEnvelopeShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
    moduleLog = new GeoLogVol(getName(), moduleEnvelopeShape, materials->gasMaterial());
  }
  return moduleLog;
}


GeoVPhysVol* SCT_Module::build(SCT_Identifier id) const{

  if(!m_staveLayout) {
    //We make these fullPhysVols for the alignment code.
    if (not m_baseBoard){
    	std::cerr<<"baseboard pointer in function 'build' of \n" 
    	<<"InnerDetector/InDetDetDescr/SCT_SLHC_GeoModel/src/SCT_Module.cxx\n"
    	<<" is NULL." <<std::endl;
    	return NULL;
    }
    GeoFullPhysVol* module = new GeoFullPhysVol(m_logVolume); 
    //first, calculate the module components position
    GeoTrf::Transform3D innerSidePos(GeoTrf::Transform3D::Identity());
    GeoTrf::Transform3D baseBoardPos(GeoTrf::Transform3D::Identity());
    GeoTrf::Transform3D outerSidePos(GeoTrf::Transform3D::Identity());
    if (m_doubleSided){
      //inner side position (shift this side towards the intreaction point, ie X negative)
      GeoTrf::Transform3D inner_Rot = GeoTrf::RotateX3D(-0.5*m_stereoAngle)*GeoTrf::RotateZ3D(180*Gaudi::Units::deg);
      double interSidesGap = std::max(m_baseBoard->thickness(), m_interSidesGap);
      double Xpos = -0.5*(interSidesGap + m_innerSide->thickness());
      //std::cerr<<"inner Xpos "<<Xpos<<" thickness "<<m_innerSide->thickness()<<std::endl;
      //protection
      if(fabs(Xpos)+0.5*m_innerSide->thickness() > 0.5*m_thickness){
	      std::cout<<"SCT_Module.cxx: sensor position should be < than module half thickness, "
		    <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_innerSide->thickness()
		    <<", halfmodthick = "<<0.5*m_thickness
		    <<". exit athena!"<<std::endl;
	      exit(1);
      }
      GeoTrf::Translation3D  inner_Xpos(Xpos, 0., 0.);
      innerSidePos = GeoTrf::Transform3D(inner_Xpos*inner_Rot);
      //basebord position (no shift)
      //outer side (shift towards X positive)
      GeoTrf::RotateX3D outer_Rot(+0.5*m_stereoAngle);
      Xpos = 0.5*(interSidesGap + m_outerSide->thickness());
      //std::cerr<<"inner Xpos "<<Xpos<<" thickness "<<m_outerSide->thickness()<<std::endl;
      //protection
      if(fabs(Xpos)+0.5*m_outerSide->thickness() > 0.5*m_thickness){
	      std::cout<<"SCT_Module.cxx: sensor position should be < than module half thickness, "
		    <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_outerSide->thickness()
		    <<", halfmodthick = "<<0.5*m_thickness
		    <<". exit athena!"<<std::endl;
	      exit(1);
      }
      GeoTrf::Translation3D outer_Xpos(Xpos, 0., 0.);
      outerSidePos = GeoTrf::Transform3D(outer_Xpos*outer_Rot);
    } else {
      //inner side position (shift this side towards the intreaction point, ie X negative)
      GeoTrf::RotateZ3D inner_Rot(180*Gaudi::Units::deg);
      double Xpos = -0.5*m_baseBoard->thickness();
      //protection
      if(fabs(Xpos)+0.5*m_innerSide->thickness() > 0.5*m_thickness){
	      std::cout<<"SCT_Module.cxx: sensor position should be < than module half thickness, "
		    <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_innerSide->thickness()
		    <<", halfmodthick = "<<0.5*m_thickness
		    <<". exit athena!"<<std::endl;
	     exit(1);
      }
      GeoTrf::Translation3D  inner_Xpos(Xpos, 0., 0.);
      innerSidePos = GeoTrf::Transform3D(inner_Xpos*inner_Rot);
      //basebord position (shift towards X positive)
      Xpos = 0.5*m_innerSide->thickness();
      //protection!
      if(fabs(Xpos)+0.5*m_baseBoard->thickness() > 0.5*m_thickness){
	      std::cout<<"SCT_Module.cxx: sensor position should be < than module half thickness, "
		      <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_baseBoard->thickness()
		      <<", halfmodthick = "<<0.5*m_thickness
		      <<". exit athena!"<<std::endl;
	      exit(1);
      }
      baseBoardPos = GeoTrf::Translate3D(Xpos, 0.0, 0.0);
    }
    
    //Add physical innerside to the module
    id.setSide(0); // If stave layout the side number is set from the ski.
    GeoVPhysVol* innerSide = m_innerSide->build(id);
    Identifier innerId = id.getWaferId();
    GeoAlignableTransform* innerTransform = new GeoAlignableTransform(innerSidePos);
    module->add(innerTransform);
    module->add(new GeoNameTag("Side#0"));
    module->add(new GeoIdentifierTag(0));
    module->add(innerSide);  
    //Store alignable transform
    detectorManager()->addAlignableTransform(0, innerId, innerTransform, innerSide);
    
    //Add Baseboard
    if (m_baseBoard) {
      GeoTransform* baseBoardTransform = new GeoTransform(baseBoardPos);
      module->add(baseBoardTransform);
      module->add(m_baseBoard->getVolume());
    }

    //Add outerside
    if(m_outerSide){
      id.setSide(1);
      GeoVPhysVol* outerSide = m_outerSide->build(id);
      Identifier outerId = id.getWaferId();
      GeoAlignableTransform* outerTransform = new GeoAlignableTransform(outerSidePos);
      module->add(outerTransform);
      module->add(new GeoNameTag("Side#1")); 
      module->add(new GeoIdentifierTag(1));
      module->add(outerSide);
      //Store alignable transform
      detectorManager()->addAlignableTransform(0, outerId, outerTransform, outerSide);
    }

    return module;

  } else {
    // Stave layout
    // No need for an extra envelope.  
    return m_innerSide->build(id);
  }
}

}
