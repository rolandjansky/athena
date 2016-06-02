/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutLoI/GeoPixelLadderLoI.h"
#include "PixelLayoutLoI/GeoPixelStaveSupportLoI.h"
#include "PixelLayoutLoI/PixelLoIStaveXMLHelper.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelGeoModel/PixelGeoBuilder.h"
/*#include "TrkDetDescrTools/TrkGeoTemplates.h"*/
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "PathResolver/PathResolver.h"

using std::max;

GeoPixelLadderLoI::GeoPixelLadderLoI(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, 
				     int iLayer, HepGeom::Transform3D trf):
  PixelGeoBuilder(basics),							
  m_staveTmp(staveTmp),
  m_layer(iLayer),
  m_localTrf(trf),
  m_sector(0),
  m_barrelModule(0),
  m_staveSupport(0),
  m_ladderShape(0),
  m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
  m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc")

{

  // Build stave support and module
  preBuild();
}


// The module has X axis (depth axis) out of the plane pointing
// towards the chips. Z axis is the eta direction.  
// For the module at global phi ~= 0 the module X,Y,Z axis are
// approximately aligned with the global X,Y,Z (ignoring the tilts)

// Looking from IP at global phi ~=0  
//
//
//      
//    Z ^
//      |
//      |
//  Y<--x
//
// X axis into page
//
// MCC side
// 
//            _ _ 0   F        ^z
//  pigtail  |_ _ .   .        |
//                .   .        |  
//                .   .    <-- x     Module ref system (Hit frame)
//                7   8    y
//
//


void GeoPixelLadderLoI::preBuild( ) {

  // Retrieve the service in charge of building the modules
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
      std::cout << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << std::endl;
  else 
      std::cout << "Pixel module builder tool retrieved: " << m_pixelModuleSvc << std::endl;
  
  // Access xml file				
  PixelLoIStaveXMLHelper staveDBHelper(m_layer, getBasics());

  printf("************** BUILD LADDER for layer  %d\n", m_layer);

  std::string staveType = m_staveTmp->name;
  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;

  m_moduleTilt = 0.;

  m_barrelModuleDZ = staveDBHelper.getBarrelModuleDZ();
  m_staggerDist =  staveDBHelper.getStaggerDist();
  m_staggerSign =  staveDBHelper.getStaggerSign();
  m_moduleCenterShift =  staveDBHelper.getCenterShift();

  std::cout<<"-- Barrel modules : "<<m_barrelModuleNumber<<" "<<m_barrelModuleType<<" / "<<staveType<<"  "<<m_barrelModuleDZ<<std::endl;

  //  Get module from PixelServiceModule
  std::cout<<"xxxxxxxxxxxxx Get barrel module from builder : "<<m_barrelModuleType<<" / "<<staveType<<std::endl;
  m_barrelModule = m_pixelModuleSvc->getModule(getBasics(),0,m_layer,m_barrelModuleType);
  m_barrelModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_barrelModuleType);

  std::cout<<"xxxxxxxxxxxxx Build stave support for layer : "<<m_layer<<std::endl;
  m_staveSupport = new GeoPixelStaveSupportLoI(getBasics(),m_layer, *m_barrelModule); 

  m_thicknessN = m_staveSupport->thicknessN();
  m_thicknessP = m_staveSupport->thicknessP();
  m_width = m_staveSupport->width();

  const HepGeom::Transform3D &staveTrf = m_staveSupport->transform();
  double staveXoffset = staveTrf.getTranslation().x();

  m_length = 2.*m_staveTmp->support_halflength;

  std::cout<<"LADDER size LxWxT "<<m_length<<" "<<m_width<<"  "<<m_thicknessP+m_thicknessN<<std::endl;

  double thicknessN_tot = m_thicknessN+staveXoffset+m_barrelModule->Thickness();
  double thicknessP_tot = m_thicknessP;

  // Build the ladder box
  double halfThickness = 0.5*(thicknessP_tot+thicknessN_tot);
  double shift = 0.5*(thicknessP_tot-thicknessN_tot);
  GeoBox * box = new GeoBox(halfThickness, m_width/2.+.001, m_length/2.);
  const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
  m_ladderShape = &shiftedBox;  

  const GeoMaterial* air = matMgr()->getMaterial("std::Air");
  m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);

  // Register the number of mopdules defined for a stave
  getBasics()->getDetectorManager()->numerology().setNumEtaModulesForLayer(m_layer,m_barrelModuleNumber);

  // ----------------------------------------------------------------------------
  // Ladder max size
  // ----------------------------------------------------------------------------

  double safety = 0.01*CLHEP::mm; 

  m_width = calcWidth();
  m_thicknessP = 0.5 * staveDBHelper.getStaveSupportThick();
  m_thicknessN = thicknessN_tot;
  m_envLength = m_length;


  // If thickness from database is 0. Then we need to calculate the thickness 
  if (m_thicknessP <= 0.001) {
    m_thicknessP = 0.5* calcThickness() + safety;
    m_thicknessN = m_thicknessP;
  }

  //  std::cout<<"Ladder thickness "<<m_thicknessN<<" "<<m_thicknessP<<std::endl;

  // Increase if stave is present and supplies thickness.
  if (m_staveSupport) {
    m_thicknessP = std::max(m_thicknessP, m_staveSupport->thicknessP()+safety); 
    m_thicknessN = std::max(m_thicknessN, m_staveSupport->thicknessN()+safety); 
  }

  // If upper and lower thicknesses area within 100 um. Make them the same.
  if (std::abs(m_thicknessP - m_thicknessN) < 0.1*CLHEP::mm) {
    m_thicknessP = std::max(m_thicknessP,m_thicknessN); 
    m_thicknessN = m_thicknessP;

    std::cout<<"Ladder thickness "<<m_thicknessN<<" "<<m_thicknessP<<std::endl;

    double halfThickness = m_thicknessP;
    m_ladderShape = new GeoBox(halfThickness, m_width*.5, m_envLength*.5);
  } 
  

}


GeoVPhysVol* GeoPixelLadderLoI::Build( ) {

  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);
  int iModuleCmpt = 0; 
  iModuleCmpt = (m_barrelModuleNumber%2>0) ? -(m_barrelModuleNumber-1)/2 : -m_barrelModuleNumber/2;
  //  std::cout<<"FIRST module number "<<m_barrelModuleNumber<<"  "<<iModuleCmpt<<std::endl;

  //------------------------------------------------------
  // Place the barrel modules
  //------------------------------------------------------
  double zNegStavePos= 0;
  double deltaZpos = 0;
  if(m_barrelModuleDZ<0) {
    zNegStavePos = (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5;
    deltaZpos=(m_barrelModule->Length()*.5+m_barrelModuleGap);
  }
  else {
    zNegStavePos = (m_barrelModuleNumber*m_barrelModuleDZ)*0.5;    
    deltaZpos=m_barrelModuleDZ;
  }
  zNegStavePos *=-1.;


   double xposShift = 0.;
   double yposShift = 0.;

   int etaModule = -m_barrelModuleNumber/2;
   for(int ii = 0; ii < m_barrelModuleNumber; ii++) 
     {

// // FIXME : complex transforms (stagged, inclined, local shifts not available yet
//        double xpos = gmt_mgr->PixelModuleStaggerDistance() *  gmt_mgr->PixelModuleStaggerSign(etaModule)
// 	 + gmt_mgr->PixelModuleDrDistance()*gmt_mgr->PixelModuleShiftFlag(etaModule)/2.; 
//        //       double ypos = 0.;
//        double zpos = gmt_mgr->PixelModuleZPosition(etaModule);

       double xpos = m_staggerDist * m_staggerSign + m_moduleCenterShift * ((m_barrelModuleNumber%2)==1&&etaModule==0);
       double zpos = m_barrelModuleDZ*(ii - .5*(m_barrelModuleNumber-1));
       CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);

       //  Rotate if module is inclined.
       //
       CLHEP::HepRotation rm;
       //       rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
       rm.rotateY(0.);

       // Build the Module and register it as SiDetectorElement
       //
       GeoFullPhysVol* modulePhys = m_barrelModule->Build(0, m_layer, m_sector, iModuleCmpt);
       Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
       InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_barrelModuleDesign, 
 									     modulePhys, getBasics()->getCommonItems());
       getBasics()->getDetectorManager()->addDetectorElement(element);
	
       // Place the module
       //
       std::ostringstream nameTag; 
       nameTag << "ModuleBrl" << etaModule;
       GeoNameTag * tag = new GeoNameTag(nameTag.str());
       GeoAlignableTransform* xform;
       xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,modulepos));
       ladderPhys->add(tag);
       ladderPhys->add(new GeoIdentifierTag(etaModule) );
       ladderPhys->add(xform);
       ladderPhys->add(modulePhys);
       
       // Now store the xform by identifier:
       getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

       zpos+=deltaZpos;
       iModuleCmpt++;
       if(iModuleCmpt==0&&(m_barrelModuleNumber%2)==0) iModuleCmpt++;

       etaModule++;
       if((m_barrelModuleNumber%2)==0&&etaModule==0)etaModule++;

     }

   //Add the TMT or other stave support
   if (m_staveSupport) {
     ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
     ladderPhys->add(m_staveSupport->getPhysVol()); 
   }
   
   return ladderPhys;
}


double GeoPixelLadderLoI::calcThickness() {
  //
  // The module thickness is copied from the module. Should improve this
  // to avoid duplication of code
  //

  PixelLoIStaveXMLHelper staveDBHelper(m_layer, getBasics());

  double clearance = staveDBHelper.getClearance();
  double staggerDist = staveDBHelper.getStaggerDist();
  double centerDist =  staveDBHelper.getCenterShift();

  std::cout<<"Ladder thickness : "<<clearance<<" "<<staggerDist<<" "<<centerDist<<std::endl;
  
  const double safety = 0.01*CLHEP::mm; 
  clearance = std::max(clearance, safety);
  
  double thickn = 0.5 * m_barrelModule->getModuleSensorThick() + m_barrelModule->getModuleHybridThick() + clearance;
  double thickp = 0.5 * m_barrelModule->getModuleSensorThick() + m_barrelModule->getModuleChipThick() + m_barrelModule->getModuleChipGap() + clearance;
  double thick = std::max(thickn, thickp); 

  std::cout<<"Ladder thickness : "<<thickn<<" "<<thickp<<" "<<thick<<std::endl;
  
  double length =  std::max(m_barrelModule->getModuleSensorLength(), std::max(m_barrelModule->getModuleHybridLength(), m_barrelModule->getModuleChipLength())); 
  double tiltThick = 0.5*length * sin(std::abs(m_moduleTilt) + thick * cos(m_moduleTilt));
				      
  // take into account stagger and tilt
  //PixelModuleDrDistance can be -ve
  double thick1 = std::max(thick + 0.5*centerDist, thick - 0.5*centerDist);
  double thick2 = tiltThick + staggerDist;
		    
  thick = std::max(thick1,thick2);
		    
  return 2*thick;
}


double GeoPixelLadderLoI::calcWidth() {
  //
  // The width is the maximum among the component widths
  //
  PixelLoIStaveXMLHelper staveDBHelper(m_layer, getBasics());
  
  double clearance = staveDBHelper.getClearance();

  const double clearanceRphi = clearance; // From Grant : no clash with big pigtail part
  double width = max( max(
			  m_barrelModule->getModuleSensorWidth(),
			  m_barrelModule->getModuleHybridWidth()),
		      m_barrelModule->getModuleChipWidth());
  //width += 5.8; // New DC3 ! to allow module movement
  width += 2*clearanceRphi; // New DC3 ! to allow module movement
  return width;
}
