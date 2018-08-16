/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelLadder.h"
#include "PixelGeoModel/GeoPixelStaveSupport.h"
#include "PixelGeoModel/GeoPixelModule.h"
#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

using std::max;

GeoPixelLadder::GeoPixelLadder(GeoPixelSiCrystal& theSensor,
			       GeoPixelStaveSupport* staveSupport) :
  m_theLadder(0),
  m_theSensor(theSensor),
  m_staveSupport(staveSupport)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //

  //   
  // Length of the ladder is in the db
  //
  double length = gmt_mgr->PixelLadderLength();
  double safety = 0.01*CLHEP::mm; 

  m_width = calcWidth();
  m_thicknessP = 0.5 * gmt_mgr->PixelLadderThickness();
  m_thicknessN = m_thicknessP;

  // If thickness from database is 0. Then we need to calculate the thickness 
  if (m_thicknessP <= 0) {
    m_thicknessP = 0.5* calcThickness() + safety;
    m_thicknessN = m_thicknessP;
  }

  // Increase if stave is present and supplies thickness.
  if (m_staveSupport) {
    m_thicknessP = std::max(m_thicknessP, m_staveSupport->thicknessP()+safety); 
    m_thicknessN = std::max(m_thicknessN, m_staveSupport->thicknessN()+safety); 
  }
  
  const GeoShape * ladderShape = 0;

  // If upper and lower thicknesses area within 100 um. Make them the same.
  if (std::abs(m_thicknessP - m_thicknessN) < 0.1*CLHEP::mm) {
    m_thicknessP = std::max(m_thicknessP,m_thicknessN); 
    m_thicknessN = m_thicknessP;
    double halfThickness = m_thicknessP;
    ladderShape = new GeoBox(halfThickness, m_width/2., length/2.);
  } 
  else if (gmt_mgr->PixelBentStaveNModule() != 0)
    {
      // Calculate thickness from bent stave part
      double angle              = gmt_mgr->PixelLadderBentStaveAngle() * CLHEP::pi / 180.0;
      double BentStaveThickness = double(gmt_mgr->PixelBentStaveNModule()) * gmt_mgr->PixelLadderModuleDeltaZ() * sin(angle);
      
      // Extend +ve or -ve ladder thickness according to stave angle
      if (angle < 0) m_thicknessP += BentStaveThickness;
      if (angle > 0) m_thicknessN += BentStaveThickness;
      
      //    std::cout << "NPR:: m_thicknessP = " << m_thicknessP << std::endl;
      //    std::cout << "NPR:: m_thicknessN = " << m_thicknessN << std::endl;
      //    std::cout << "NPR:: shift        = " << -0.5*BentStaveThickness << std::endl;
      //    std::cout << "NPR:: an gle       = " << angle << std::endl;
      
      // Create stave and apply shift to the ladder.
      GeoBox * box = new GeoBox((m_thicknessP+m_thicknessN)/2., m_width/2., length/2.);
      
      // Shift ladder outwards if stave bends away from beam pipe
      double shift = 0.5*BentStaveThickness;
      if (angle > 0) shift *= -1.0;
      const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
      ladderShape = &shiftedBox; 
    }
  else if (!(gmt_mgr->PixelStaveLayout()>3&& gmt_mgr->PixelStaveLayout()<7)){
    double halfThickness = 0.5*(m_thicknessP+m_thicknessN);
    double shift = 0.5*(m_thicknessP-m_thicknessN);
    GeoBox * box = new GeoBox(halfThickness, m_width/2., length/2.);
    const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
    ladderShape = &shiftedBox;  
  }
  else if(m_staveSupport) 
    {
      GeoSimplePolygonBrep* staveSupportShape=m_staveSupport->computeStaveEnvelopShape(safety);
      const GeoShape & staveShape = (*staveSupportShape);
      ladderShape = &staveShape;  
    }
  else
    {
      gmt_mgr->msg(MSG::ERROR)<<"No ladder shape could be defined "<<endmsg;      
    }

  if(ladderShape==0)
  {
     gmt_mgr->msg(MSG::ERROR)<<"No ladder shape could be defined in "<<__FILE__<<endmsg;
     exit(EXIT_FAILURE);
  } else {
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  m_theLadder = new GeoLogVol("Ladder",ladderShape,air);
  }
  m_thickness = 2*std::max(m_thicknessN,m_thicknessP);
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


GeoVPhysVol* GeoPixelLadder::Build( ) {

  // Something went wrong while building the ladder logical volume 
  if(!m_theLadder) return 0;

  // Create the ladder physVolume
  GeoPhysVol* ladderPhys = new GeoPhysVol(m_theLadder);
  //
  // Place the Modules
  //
  GeoPixelModule pm(m_theSensor);
  
  bool isBLayer=(gmt_mgr->GetLD() == 0);
  bool isModule3D=true;
  if (gmt_mgr->PixelStaveLayout()<5) isModule3D=false;
  GeoPixelSiCrystal theSensor3D(isBLayer,isModule3D);
  GeoPixelModule pm3D(theSensor3D);
  //  double pm3DLength=pm3D.Length();

  // Pixel module parameters
  int staveLayout = gmt_mgr->PixelStaveLayout();
  int nbPixelModule=gmt_mgr->PixelNModule();
  int nbPixelPlanarModule=0;
  int nbPixel3DModule=0;
  bool bDetailedStaveLayout=false;
  double pixelModuleGap=gmt_mgr->IBLStaveModuleGap();
  double zNegStavePos= 0.0;
  double planarLength=0.0;
  double v3DHalfLength=0.0;
  double v3DLength=0.0;

  bool bVerbose= (gmt_mgr->msgLvl(MSG::DEBUG));

  // Radial shift of Si3D module in case its thickness is greater that SiPl module's one
  double Si3DRadialShift=0.;
  double Si3DLateralShift=0.;
  if(isModule3D)
    {
      double SiPlThick = pm.ThicknessP();
      double Si3DThick = pm3D.ThicknessP();
      if(Si3DThick>SiPlThick) Si3DRadialShift=-(Si3DThick-SiPlThick);
      //      gmt_mgr->msg(MSG::INFO)<<"Si3D/SiPl radial shift : "<<Si3DRadialShift<<"  Pl/3D : "<<SiPlThick<<" "<<Si3DThick<<endmsg;

      double MechanicalStaveOffset = gmt_mgr->IBLStaveMechanicalStaveOffset();
      double MechanicalStaveOffset3D = gmt_mgr->IBLStaveMechanicalStaveOffset(true);
      Si3DLateralShift= -(MechanicalStaveOffset3D-MechanicalStaveOffset);
    }

  // Module 3D geometry (IBL planar and 3D modules)
  if(gmt_mgr->ibl() && gmt_mgr->GetLD()==0)
    {
      if(staveLayout==4)
	{
	  nbPixelModule=gmt_mgr->IBLStaveModuleNumber_AllPlanar();
	  bDetailedStaveLayout=true;
	  nbPixelPlanarModule=nbPixelModule;

	}
      else if(staveLayout==5||staveLayout==6)     // 75/25 or 50/50
	{
	  bDetailedStaveLayout=true;
	  nbPixelModule=m_staveSupport->PixelNModule();
	  nbPixelPlanarModule=m_staveSupport->PixelNPlanarModule();
	  nbPixel3DModule=m_staveSupport->PixelN3DModule();

	  if(staveLayout==5)
	    {
	      planarLength=nbPixelPlanarModule*pm.Length()+(nbPixelPlanarModule-1)*pixelModuleGap;
	      v3DHalfLength=(nbPixel3DModule/2)*pm3D.Length()+(nbPixel3DModule/2)*pixelModuleGap;
	      zNegStavePos=-planarLength*0.5-v3DHalfLength;
	    }
	  else
	    {
	      planarLength=nbPixelPlanarModule*pm.Length()+(nbPixelPlanarModule-1)*pixelModuleGap;
	      v3DLength=(nbPixel3DModule)*pm3D.Length()+(nbPixel3DModule-1)*pixelModuleGap+pixelModuleGap*0.5;
	      zNegStavePos=-v3DLength;
	    }

	  //	  gmt_mgr->msg(MSG::INFO)<<"GeoPixelLadder : "<<nbPixelModule<<" "<<nbPixelPlanarModule<<" + "<<nbPixel3DModule<<"   planar and 3D Lengths : "<<planarLength<<" "<<v3DLength<<endmsg;
	}
    }

  for(int ii = 0; ii < nbPixelModule; ii++) {

    int etaModule = gmt_mgr->PixelModuleEtaFromIndex(ii);
    gmt_mgr->SetEta(etaModule);

//
// For standard ATLAS stave shift down the central module, as its flag is -1.
// PixelModuleStaggerDistance is zero for standard ATLAS geometry.
// Not sure why PixelModuleDrDistance is defined as twice the shift.
// For SLHC (later versions) PixelModuleDrDistance is 0 and PixelModuleStaggerDistance is non zero for
// some layers. 
//
    
    double xpos = gmt_mgr->PixelModuleStaggerDistance() *  gmt_mgr->PixelModuleStaggerSign(etaModule)
      + gmt_mgr->PixelModuleDrDistance()*gmt_mgr->PixelModuleShiftFlag(etaModule)/2.; 
//
// Get the z position from the db
//
    double zpos = gmt_mgr->PixelModuleZPosition(etaModule);
    bool b3DModule=false;
    double xposShift=0.;
    double yposShift=0.;

    // detailed stave model : parameters are taken from PixelIBLStave table instead of PixelStave
    if(gmt_mgr->ibl() && bDetailedStaveLayout && gmt_mgr->GetLD()==0)
      {
	int moduleIndex =  gmt_mgr->PixelModuleIndexFromEta(etaModule);  

	if(gmt_mgr->PixelStaveLayout()==4)   /// 100 % planar modules
	  zpos=(pm.Length()+gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - 0.5*(gmt_mgr->IBLStaveModuleNumber_AllPlanar()-1));
	else if(gmt_mgr->PixelStaveLayout()==5)   /// 75/25  planar/3D
	  {

	    if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"Eta : cmpt/index "<<ii<<" "<<moduleIndex<<endmsg;

	    if(ii<nbPixel3DModule/2) // zneg 3D pixel area
	      {
		b3DModule=true;
		zpos=(pm3D.Length()+gmt_mgr->IBLStaveModuleGap()) * moduleIndex;
		zpos+=pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
		if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"left 3D module "<<pm3D.Length()<<" "<<zpos<<endmsg;
	      }
	    else if(ii>nbPixelPlanarModule+nbPixel3DModule/2-1)  // zpos 3D pixel area
	      {
		if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"right 3D module "<<pm3D.Length()<<" "<<zpos<<endmsg;
		b3DModule=true;
		zpos=v3DHalfLength+planarLength+gmt_mgr->IBLStaveModuleGap();
		zpos+=(pm3D.Length()+gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule/2 - nbPixelPlanarModule); 
		zpos+=pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
	      }
	    else  // planar pixel area
	      {
		zpos=v3DHalfLength+(pm.Length()+gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule/2); 
		zpos+=pm.Length()*0.5;
		if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"planar module "<<pm.Length()<<" "<<zpos<<endmsg;
		xposShift=0.;
	      }

	    // Shift vs half stave length
	    zpos=zNegStavePos+zpos;

	  }
	else if(gmt_mgr->PixelStaveLayout()==6)   /// 50/50  planar/3D
	  {

	    if(ii<nbPixel3DModule) // zneg 3D pixel area
	      {
		if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"left 3D module "<<pm3D.Length()<<" "<<moduleIndex<<endmsg;
		b3DModule=true;
		zpos=(pm3D.Length()+pixelModuleGap) * moduleIndex;
		zpos=zNegStavePos+zpos+pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
	      }
	    else  // zpos 3D pixel area
	      {
		if(bVerbose)gmt_mgr->msg(MSG::DEBUG)<<"right plannar module "<<pm.Length()<<" "<<moduleIndex<<endmsg;
		zpos=(pm.Length()+gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule);
		zpos+=pm.Length()*0.5+pixelModuleGap*0.5;
	      }
	  }
      }

    
    CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);
//
//  Rotate if module is inclined.
//
    CLHEP::HepRotation rm;
    rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
//
// Place the Module
//
    GeoVPhysVol* modulephys; // = pm.Build() ;

    if(!b3DModule)
      modulephys = pm.Build();
    else
      modulephys = pm3D.Build();

    std::ostringstream nameTag; 
    nameTag << "ModuleBrl" << gmt_mgr->Eta();
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoAlignableTransform* xform;

    xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,modulepos));
    ladderPhys->add(tag);
    ladderPhys->add(new GeoIdentifierTag(gmt_mgr->Eta() ) );
    ladderPhys->add(xform);
    ladderPhys->add(modulephys );

    // Now store the xform by identifier:
    Identifier id;
    if(!b3DModule)  id = m_theSensor.getID();
    else id = theSensor3D.getID();
    DDmgr->addAlignableTransform(0,id,xform,modulephys);
    
  }
  //Add the TMT or other stave support
  if (m_staveSupport) {
    ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
    ladderPhys->add(m_staveSupport->getPhysVol()); 
  }

  
  if (gmt_mgr->PixelBentStaveNModule() != 0)
    {
      std::cout << "SB: Adding bent stave support" << std::endl;
      double thickness = gmt_mgr->PixelLadderSupportThickness();
      double width     = gmt_mgr->PixelLadderSupportWidth();
      double length    = gmt_mgr->PixelLadderSupportLength();
      double xOffset   = gmt_mgr->PixelLadderServicesX();
      //      double yOffset   = gmt_mgr->PixelLadderServicesY();
      //      int staveIndex   = gmt_mgr->PixelStaveIndex(gmt_mgr->GetLD());
      
      // Bent stave half length  = 0.5 * nModules * ModuleSize
      double bentStaveHalfLength = 0.5 * double(gmt_mgr->PixelBentStaveNModule()) * gmt_mgr->PixelLadderModuleDeltaZ();
      
      // Create bent stave
      GeoBox * shapeSupBent = new GeoBox(0.5*thickness, 0.5*width, bentStaveHalfLength);
      
      //      std::string matName = gmt_mgr->getMaterialName("StaveSupportOuter", gmt_mgr->GetLD(), staveIndex);
      //      std::string matName = gmt_mgr->getMaterialName("StaveSupport", gmt_mgr->GetLD(), staveIndex);
      //      const GeoMaterial* materialSup = mat_mgr->getMaterialForVolume(matName,shapeSupBent->volume());
      const GeoMaterial* materialSup = mat_mgr->getMaterial("pix::StaveSupportBase");
      
      double ang = gmt_mgr->PixelLadderBentStaveAngle() * CLHEP::pi / 180.0;
      double xst = xOffset - (bentStaveHalfLength * sin(ang)); 
      
      // Construct bent stave at negative z
      GeoLogVol* logVolBentNeg = new GeoLogVol("StaveSupportBentNeg2",shapeSupBent,materialSup);
      GeoPhysVol* physVolBentNeg = new GeoPhysVol(logVolBentNeg);
      CLHEP::HepRotation rmNeg;
      rmNeg.rotateY(ang);
      double zstneg = -length/2.0 - (bentStaveHalfLength * cos(ang));
      HepGeom::Point3D<double> stavePosNeg(xst,0.,zstneg);
      ladderPhys->add(new GeoTransform(HepGeom::Transform3D(rmNeg,stavePosNeg)));
      ladderPhys->add(physVolBentNeg); 
      
      // COnstruct bent stave at positive z
      GeoLogVol* logVolBentPos = new GeoLogVol("StaveSupportBentPos2",shapeSupBent,materialSup);
      GeoPhysVol* physVolBentPos = new GeoPhysVol(logVolBentPos);
      CLHEP::HepRotation rmPos;
      rmPos.rotateY(-ang);
      double zstpos = length/2.0 + (bentStaveHalfLength * cos(ang));
      HepGeom::Point3D<double> stavePosPos(xst,0.,zstpos);
      ladderPhys->add(new GeoTransform(HepGeom::Transform3D(rmPos,stavePosPos)));
      ladderPhys->add(physVolBentPos);       
    }


  return ladderPhys;
}


double GeoPixelLadder::calcThickness() {
  //
  // The module thickness is copied from the module. Should improve this
  // to avoid duplication of code
  //

  const double safety = 0.01*CLHEP::mm; 
  double clearance = gmt_mgr->PixelLadderThicknessClearance();
  clearance = std::max(clearance, safety);

  double thickn = 0.5 * gmt_mgr->PixelBoardThickness() + gmt_mgr->PixelHybridThickness() + clearance;
  double thickp = 0.5 * gmt_mgr->PixelBoardThickness() + gmt_mgr->PixelChipThickness() + gmt_mgr->PixelChipGap() + clearance;
  double thick = std::max(thickn, thickp); 

  double length =  std::max(gmt_mgr->PixelBoardLength(), std::max(gmt_mgr->PixelHybridLength(), gmt_mgr->PixelChipLength())); 
  double tiltThick = 0.5*length * sin(std::abs(gmt_mgr->PixelModuleAngle())) + thick * cos(gmt_mgr->PixelModuleAngle());
  
  // take into account stagger and tilt
  //PixelModuleDrDistance can be -ve
  double thick1 = std::max(thick + 0.5*gmt_mgr->PixelModuleDrDistance(), thick - 0.5*gmt_mgr->PixelModuleDrDistance());
  double thick2 = tiltThick + gmt_mgr->PixelModuleStaggerDistance();
		    
  thick = std::max(thick1,thick2);
		    
  return 2*thick;
}


double GeoPixelLadder::calcWidth() {
  //
  // The width is the maximum among the component widths
  //
  const double clearanceRphi = gmt_mgr->PixelLadderWidthClearance(); // From Grant : no clash with big pigtail part
  double width = max( max(
                     gmt_mgr->PixelBoardWidth(),
                     gmt_mgr->PixelHybridWidth()),
                 gmt_mgr->PixelChipWidth());
  //width += 5.8; // New DC3 ! to allow module movement
  width += 2*clearanceRphi; // New DC3 ! to allow module movement
  return width;
}


