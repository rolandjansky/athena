/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelExtendedRef/GeoPixelLadderIBeamRef.h"
#include "BarrelExtendedRef/GeoPixelStaveSupportExtRef.h"
#include "BarrelExtendedRef/PixelExtRefStaveXMLHelper.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelGeoModel/PixelGeoBuilder.h"
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
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "PathResolver/PathResolver.h"

using std::max;

GeoPixelLadderIBeamRef::GeoPixelLadderIBeamRef(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, 
					       int iLayer, HepGeom::Transform3D trf, std::string mode):
  PixelGeoBuilder(basics),							
  m_staveTmp(staveTmp),
  m_layer(iLayer),
  m_localTrf(trf),
  m_sector(0),
  m_barrelModule(0),
  m_staveSupport(0),
  m_ladderShape(0),
  m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
  m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc"),
  m_IDserviceTool("InDetservicesTool/PixelServicesTool"),
  m_mode(mode)
{

  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::DEBUG) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

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


void GeoPixelLadderIBeamRef::preBuild( ) {

  // Get access to the service that defines the modules
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    msg(MSG::WARNING) << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." <<endmsg;
  else 
    msg(MSG::DEBUG) << "Pixel module builder tool retrieved: " << m_pixelModuleSvc <<endmsg;
  
  // Access stave description xml file				
  PixelExtRefStaveXMLHelper staveDBHelper(m_layer, getBasics());

  msg(MSG::DEBUG) << "************** BUILD LADDER for layer" << m_layer<<endmsg;

  std::string staveType = m_staveTmp->type;

  // ----------------------------------------------------------------------------
  // Barrel module
  // ----------------------------------------------------------------------------
  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;

  m_moduleTilt = 0.;

  //  m_barrelModuleDZ = staveDBHelper.getBarrelModuleDZ();
  m_staggerDist =  staveDBHelper.getStaggerDist();
  m_staggerSign =  staveDBHelper.getStaggerSign();
  m_moduleCenterShift =  staveDBHelper.getCenterShift();


  //  double phiOfSTaveZero = 0.;
  msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Get barrel module from builder : "<<m_barrelModuleType<<" / "<<staveType<<endmsg;
  m_barrelModule = m_pixelModuleSvc->getModule(getBasics(),0,m_layer,m_barrelModuleType);
  m_barrelModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_barrelModuleType);

  m_barrelModuleDZ = m_barrelModule->Length()+m_barrelModuleGap;

  msg(MSG::DEBUG)<<"-- Barrel modules : "<<m_barrelModuleNumber<<" "<<m_barrelModuleType<<" / "<<staveType<<"  "<<m_barrelModuleDZ<<endmsg;

  // ----------------------------------------------------------------------------
  // Stave module service thickness
  // ----------------------------------------------------------------------------
  m_moduleSvcThickness = fmax(staveDBHelper.getRadialBeamThickness(),0.5*CLHEP::mm);
  
  // ----------------------------------------------------------------------------
  // Stave support service thickness
  // ----------------------------------------------------------------------------
  msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Build stave support for layer : "<<m_layer<<endmsg;
  m_staveSupport = new GeoPixelStaveSupportExtRef(getBasics(),m_layer, *m_barrelModule); 

  // stave support thicknesses & width
  double thicknessN = m_staveSupport->thicknessN();
  double thicknessP = m_staveSupport->thicknessP();
  m_width = m_staveSupport->width();

  const HepGeom::Transform3D &staveTrf = m_staveSupport->transform();
  double staveXoffset = m_mode == "inner" ? staveTrf.getTranslation().x() : - staveTrf.getTranslation().x();
  //  double staveYoffset = staveTrf.getTranslation().y();

  m_length = 2.*m_staveTmp->support_halflength;

  double thicknessN_tot = m_barrelModule->Thickness()*.5;
  //double thicknessP_tot = m_barrelModule->Thickness()*.5+staveXoffset+thicknessN+thicknessP+m_moduleSvcThickness;
  double thicknessP_tot = m_barrelModule->Thickness()*.5+staveXoffset+thicknessN+thicknessP;   // services moved to middle beam

  // Defined the ladder box (centered around the point that correspond to the middle of the sensor
  double halfThickness = 0.5*(thicknessP_tot+thicknessN_tot);
  double shift = m_staveSupport->routing()=="outer" ? 0.5*(thicknessP_tot-thicknessN_tot) :  -0.5*(thicknessP_tot-thicknessN_tot) ;
  GeoBox * box = new GeoBox(halfThickness, m_width/2.+.001, m_length/2.);
  const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
  m_ladderShape = &shiftedBox;  

  const GeoMaterial* air = matMgr()->getMaterial("std::Air");
  m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);

  msg(MSG::DEBUG)<<"LADDER size LxWxT "<<m_length<<" "<<m_width<<"  "<<2.*halfThickness<<endmsg;

  // ----------------------------------------------------------------------------
  // Register the number of mopdules defined for a stave
  // ----------------------------------------------------------------------------
  getBasics()->getDetectorManager()->numerology().setNumEtaModulesForLayer(m_layer,m_barrelModuleNumber);

  // ----------------------------------------------------------------------------
  // Ladder max size
  // ----------------------------------------------------------------------------
  //double safety = 0.01*CLHEP::mm; 
  m_width = calcWidth();
  m_thicknessP = thicknessP_tot;
  m_thicknessN = thicknessN_tot;
  m_envLength = m_length;

}


GeoVPhysVol* GeoPixelLadderIBeamRef::Build() {

  // build service volume
  double halfSvcThickness = 0.5*m_moduleSvcThickness;
  // radial extent 
  double halfSvcRad = 0.5*(0.5*m_rSize-m_thicknessP-m_thicknessN-2*halfSvcThickness*sin(fabs(m_tilt)));
  double rs = m_thicknessP+m_thicknessN;
  // 
  double dr  = halfSvcThickness/m_rMid*halfSvcThickness;

  GeoSimplePolygonBrep *svcLadd = new GeoSimplePolygonBrep(m_length*.5);
  if (m_staveSupport->routing()=="outer") { 
    double svc1x = rs+halfSvcThickness*sin(m_tilt),  svc1y = -halfSvcThickness;
    double svc4x = rs-halfSvcThickness*sin(m_tilt),  svc4y =  halfSvcThickness ;
    double svc3x = 0.5*m_rSize-dr-0.001,             svc3y =  halfSvcThickness;
    double svc2x = 0.5*m_rSize-dr-0.001,             svc2y = -halfSvcThickness;
    // Construct global object 2D envelope
    svcLadd->addVertex(svc1x,svc1y);
    svcLadd->addVertex(svc2x,svc2y);
    svcLadd->addVertex(svc3x,svc3y);
    svcLadd->addVertex(svc4x,svc4y);
  } else {
    dr = 0.;
    double svc2x = -(rs-halfSvcThickness*sin(m_tilt)),  svc2y = -halfSvcThickness;
    double svc3x = -(rs+halfSvcThickness*sin(m_tilt)),  svc3y =  halfSvcThickness ;
    double svc4x = -0.5*m_rSize+0.001,                  svc4y =  halfSvcThickness;
    double svc1x = -0.5*m_rSize+0.001,                  svc1y = -halfSvcThickness;
    // Construct global object 2D envelope
    svcLadd->addVertex(svc1x,svc1y);
    svcLadd->addVertex(svc2x,svc2y);
    svcLadd->addVertex(svc3x,svc3y);
    svcLadd->addVertex(svc4x,svc4y);
  }
  const GeoMaterial* air = matMgr()->getMaterial("std::Air");
  const GeoLogVol* svcVol= new GeoLogVol("SvcLadder",svcLadd,air);

  double svcShift = m_staveSupport->routing()=="outer" ?
    m_thicknessP+m_thicknessN+halfSvcRad+2*halfSvcThickness*sin(fabs(m_tilt))- 0.5*dr :
    -(m_thicknessP+m_thicknessN+halfSvcRad+2*halfSvcThickness*sin(fabs(m_tilt))) ;

  m_serviceVol = new GeoPhysVol(svcVol);

  // ladder
  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);
  int iModuleCmpt = 0; 

  iModuleCmpt = (m_barrelModuleNumber%2>0) ? -(m_barrelModuleNumber-1)/2 : -m_barrelModuleNumber/2;

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
  int nbSvcModule = m_barrelModuleNumber/2;
  int svcModuleCmpt = -1;
  for(int ii = 0; ii < m_barrelModuleNumber; ii++) 
    {

// //
// // FIXME : complex transforms (stagged, inclined, local shifts not available yet

      double xpos = m_staggerDist * m_staggerSign + m_moduleCenterShift * ((m_barrelModuleNumber%2)==1&&etaModule==0);
      //       double ypos = 0.;
      double zpos = m_barrelModuleDZ*(ii - .5*(m_barrelModuleNumber-1));
      CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);
      
      //
      //  Rotate if module is inclined : FIXME 
      //
      CLHEP::HepRotation rm;
      //       rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
      rm.rotateY(0.);
      
      //
      // Get the module with correct identifier / build and register detector element
      //
      int planarModTag = 500;
      std::ostringstream modName; 
      modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
      GeoFullPhysVol* modulePhys = m_barrelModule->Build(0, m_layer, m_sector, iModuleCmpt, planarModTag, modName.str());
      
      Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
      InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_barrelModuleDesign, 
									    modulePhys, getBasics()->getCommonItems());
      getBasics()->getDetectorManager()->addDetectorElement(element);
      
      // Build and place the module
      std::ostringstream nameTag; 
      nameTag << "ModuleBrl" << etaModule;
      GeoNameTag * tag = new GeoNameTag(nameTag.str());
      HepGeom::Transform3D modTrans = (m_staveSupport->routing()=="inner") ? 
	   HepGeom::RotateZ3D(180.*CLHEP::deg)*HepGeom::Transform3D(rm,modulepos) : HepGeom::Transform3D(rm,modulepos);
      GeoAlignableTransform* xform = new GeoAlignableTransform(modTrans);
      ladderPhys->add(tag);
      ladderPhys->add(new GeoIdentifierTag(etaModule) );
      ladderPhys->add(xform);
      ladderPhys->add(modulePhys);
      
      // Now store the xform by identifier (alignement studies )
      getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

      // Place the stave module service in the radial beam)

      if(m_IDserviceTool->svcRouteAuto()){
	GeoBox * svcBox = new GeoBox(halfSvcRad-0.5*dr-0.001, halfSvcThickness-0.001, m_barrelModule->Length()*.5);
	std::string matName = m_IDserviceTool->getLayerModuleMaterialName(m_layer ,nbSvcModule);   // material name stored in PixelServicesTool (material are built there)
	std::ostringstream wg_matName;  
	wg_matName<<matName<<"L"<<m_layer<<"M"<<ii;
	GeoMaterial* svcMat = 0;  // do not redefine material if already done for sector 0
	if(m_sector==0)
	  svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matName, svcBox->volume(), m_barrelModule->Length(),wg_matName.str()));  // define material
	else
	  svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial(wg_matName.str()));   // material already defined

	GeoLogVol* svcLog = new GeoLogVol("ModuleSvc",svcBox,svcMat);
	GeoPhysVol* svcPhys = new GeoPhysVol(svcLog);
	
	double xPos_svc = svcShift; 
	GeoTransform* xform_svc = new GeoTransform(HepGeom::TranslateX3D(xPos_svc)*HepGeom::Transform3D(rm,modulepos));
	m_serviceVol->add(xform_svc);
	m_serviceVol->add(svcPhys);

	//Place first section : from beginning fo stave to first module, last module to end of stave
	if(ii==0||ii== m_barrelModuleNumber-1)  {
	  double z0 = (ii==0)?-m_length*.5:zpos+m_barrelModule->Length()*.5;
	  double z1 = (ii==0)?zpos-m_barrelModule->Length()*.5:m_length*.5;
	  double length = z1-z0;
	  
	  if(length>0) {
	    std::string prefix=(ii==0)?"neg":"pos";
	    
	    GeoBox * svcBox = new GeoBox(halfSvcRad-0.001, halfSvcThickness-0.001, length*.5);
	    std::ostringstream wg_matName;  
	    wg_matName<<matName<<"L"<<m_layer<<"M"<<prefix;
	    GeoMaterial* svcMat = 0;   // do not redefine material if already done for sector 0
	    if(m_sector==0)
	      svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matName, svcBox->volume(), length ,wg_matName.str()));  // define material
	    else
	      svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial(wg_matName.str()));   // material already defined
	    GeoLogVol* svcLog = new GeoLogVol("ModuleSvc",svcBox,svcMat);
	    GeoPhysVol* svcPhys = new GeoPhysVol(svcLog);
	    
	    CLHEP::Hep3Vector modulepos_svc(xpos+xposShift,yposShift,(z0+z1)*.5);
	    GeoAlignableTransform* xform_svc = new GeoAlignableTransform(HepGeom::TranslateX3D(svcShift)*HepGeom::Transform3D(rm,modulepos_svc));
	    m_serviceVol->add(xform_svc);
	    m_serviceVol->add(svcPhys);
	  }
	}
      }

      // Increment/decrement the number of module which services run on the top of the stave
      nbSvcModule += svcModuleCmpt;
      if(nbSvcModule==0)
	{ nbSvcModule=1; svcModuleCmpt=1; }
      
      // Update the module counting parameter and z position 
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


double GeoPixelLadderIBeamRef::calcThickness() {
  //
  // The module thickness is copied from the module. Should improve this
  // to avoid duplication of code
  //

  PixelExtRefStaveXMLHelper staveDBHelper(m_layer,getBasics());

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


double GeoPixelLadderIBeamRef::calcWidth() {
  //
  // The width is the maximum among the component widths
  //
  PixelExtRefStaveXMLHelper staveDBHelper(m_layer,getBasics());
  
  double clearance = staveDBHelper.getClearance();

  const double clearanceRphi = clearance; // From Grant : no clash with big pigtail part
  double width = max( max(
			  m_barrelModule->getModuleSensorWidth(),
			  m_barrelModule->getModuleHybridWidth()),
		      m_barrelModule->getModuleChipWidth());

  width += 2*clearanceRphi; // New DC3 ! to allow module movement
  return width;
}
