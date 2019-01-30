/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdRing.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_FwdModule.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <sstream>
#include <cmath>

inline double sqr(double x) {return x*x;}

namespace InDetDDSLHC {

SCT_FwdRing::SCT_FwdRing(const std::string & name, 
			 int iWheel,
			 int iRing, double offset)
  : SCT_UniqueComponentFactory(name), 
    m_iWheel(iWheel),
    m_iRing(iRing),
    m_numModules(0), m_doubleSided(0), m_moduleStagger(0),m_stereoSign(0), m_refStartAngle(0),
    m_refFirstStagger(0), m_innerRadius(0), m_middleRadius(0),m_length(0),m_thickness(0), m_firstStagger(0){
    m_ringSide = (offset>0) ? 1 : -1;
    getParameters();
    m_logVolume = preBuild();
  }

void SCT_FwdRing::getParameters(){
  const SCT_ForwardParameters* parameters = geometryManager()->forwardParameters();
  m_innerRadius     = parameters->fwdRingInnerRadius(m_iRing);
  m_middleRadius    = parameters->fwdRingMiddleRadius(m_iRing);
  m_outerRadius     = parameters->fwdRingOuterRadius(m_iRing);
  m_numModules      = parameters->fwdRingNumModules(m_iRing);
  m_doubleSided     = parameters->fwdWheelDoubleSided(m_iRing); 
  m_moduleStagger   = parameters->fwdRingModuleStagger(m_iRing);
  m_refStartAngle   = parameters->fwdRingPhiOfRefModule(m_iRing);
  m_firstStagger    = parameters->fwdRingStaggerOfRefModule(m_iRing);

  //---m_stereoSign      = parameters->fwdWheelStereoType(m_iRing);
  m_stereoSign      = 1;
}

SCT_FwdRing::~SCT_FwdRing(){
  delete m_module;
}

const GeoLogVol* SCT_FwdRing::preBuild(){
  m_module = new SCT_FwdModule("FwdModule"+intToString(m_iRing), 
			       m_iRing, m_doubleSided); 

  //m_innerRadius = m_innerRadius - 0.51*Gaudi::Units::cm;//0.01mm safety necessary
  //m_outerRadius = m_outerRadius + 0.51*Gaudi::Units::cm;//0.01mm safety necessary
  m_innerRadius = m_innerRadius - 5*Gaudi::Units::mm;//0.01mm safety necessary
  m_outerRadius = m_outerRadius + 7*Gaudi::Units::mm;//0.01mm safety necessary
  m_thickness   = m_module->thickness() + m_moduleStagger + 0.01*Gaudi::Units::mm;//safety necessary
  m_length = m_outerRadius - m_innerRadius;
  //protection along R!
  if(m_length<m_module->length()){
    std::cout<<"SCT_FwdRing.cxx: Ring length should be > module length: "
	     <<"RingLength["<<m_iRing<<"] = "<<m_length<<", ModuleLength = "<<m_module->length()
	     <<". exit athena!"<<std::endl;
    exit(1);
  }

  //RK: module stagger is measured from the middle of the two modules (check it!)
  if(m_moduleStagger <= m_module->thickness()){
    std::cout<<"SCT_FwdRing.cxx: ring["<<m_iRing
	     <<"], module stagger should be > than module thickness: "
	     <<"modstagger = "<<m_moduleStagger<<", modthick = "<<m_module->thickness()
	     <<". exit athena!"<<std::endl;
    exit(1);
  }
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  const GeoTube* ringEnvelope = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_thickness);
  GeoLogVol* ringLog = new GeoLogVol(getName(), ringEnvelope, materials->gasMaterial());

  return ringLog;
}

GeoVPhysVol* SCT_FwdRing::build(SCT_Identifier id) const{

  // Physical volume for the half ring
  GeoPhysVol* ring = new GeoPhysVol(m_logVolume);
  double divisionAngle     = 360*Gaudi::Units::degree/m_numModules;
  bool   negativeEndCap    = (id.getBarrelEC() < 0);
  int    staggerUpperLower = m_firstStagger;
  for(int imod=0; imod<m_numModules; imod++){
        // The module is a Trapazoide (TRD) with length along z-axis (in the global frane). ie:
    //           _
    //        _   |
    //   _ -      | 
    //  |         |
    // -|-------  |-------->Z axis (Y is down the screen and X coming out the screen)
    //  |_        |
    //     -  _   |
    //           -
    // We need to rotate this so length is along the y-axis
    // This can be achieved with a 90 deg rotation around Y. 
    // we rotate about z by phi and the 0.5*stereo (ie the u-phi or v-phi orientation)

    double phi =  m_refStartAngle + imod*divisionAngle;
    //std::cerr<<"endcap "<<id.getBarrelEC()<<", ring "<<m_iRing<<", startAngle"<<m_refStartAngle<<", phi "<<phi<<std::endl;
    //put the module along the radius of the ring, along X for example (remeber, it is along Z)
    GeoTrf::Transform3D rot = GeoTrf::RotateY3D(90*Gaudi::Units::degree);
    if (negativeEndCap) {
      //rotate the module so that to keep the local frame orientation as in the positive end
      //rot.rotateZ(180*Gaudi::Units::degree);    
      //start in the oppsite phi and turn in the oppsite direction
      if(phi < Gaudi::Units::pi)
	phi = Gaudi::Units::pi - phi;
      else
	phi = 3*Gaudi::Units::pi - phi;
    }
    rot = GeoTrf::RotateZ3D(phi) * rot;
    //std::cerr<<"endcap "<<id.getBarrelEC()<<", wheel "<<m_iWheel<<", ring "<<m_iRing<<", mod "<<imod<<", phi "<<phi<<", startAng "<<m_refStartAngle<<std::endl;
    //the module stagger is opposite for oposite rings?! 
    double Zpos = staggerUpperLower*m_ringSide*0.5*m_moduleStagger;
    //protection along Z!
    if(fabs(Zpos)+0.5*m_module->thickness() > 0.5*m_thickness){
      std::cout<<"SCT_FwdRing.cxx: module position should be < than ring half thickness, "
	       <<"modZpos+halfmodthick = "<<fabs(Zpos)+0.5*m_module->thickness()
	       <<", halfringthick["<<m_iRing<<"] = "<<0.5*m_thickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    GeoTrf::Vector3D xyz(m_middleRadius, 0, Zpos);
    xyz = GeoTrf::RotateZ3D(phi)*xyz;
    GeoTrf::Transform3D modulePos = GeoTrf::Translate3D(xyz.x(),xyz.y(),xyz.z())*rot;
    //protection along R!
    const double epsilon = 0.0001*Gaudi::Units::mm; //beyound meansurment precision?!
    if(m_innerRadius-epsilon>m_module->innerRadius() || 
       m_outerRadius+epsilon<m_module->outerRadius()){
      std::cout<<"SCT_FwdRing.cxx: problem with module position along R: "
	       <<"RingInnerRadius["<<m_iRing<<"] = "<<m_innerRadius<<" <? ModuleInnerRadius = "
	       <<m_module->innerRadius()
	       <<" RingOuterRadius["<<m_iRing<<"] = "<<m_outerRadius<<" >? ModuleOuterRadius = "
	       <<m_module->outerRadius()
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    // Add the module
    std::string moduleName = "FwdModuleR" + intToString(m_iRing) + "#" + intToString(imod);
    ring->add(new GeoNameTag(moduleName));
    ring->add(new GeoIdentifierTag(imod));
    GeoAlignableTransform* moduleTransform = new GeoAlignableTransform(modulePos);
    ring->add(moduleTransform);
    //std::cerr<<", mod "<<imod<<std::endl;
    id.setPhiModule(imod);
    GeoVPhysVol* modulePV = m_module->build(id);
    ring->add(modulePV);
    // Store alignable transform
    detectorManager()->addAlignableTransform(1, id.getWaferId(), moduleTransform, modulePV);  

    staggerUpperLower = -staggerUpperLower;  
  }//end of for(int imod=0; ...)
  return ring;
}
}
