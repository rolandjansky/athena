/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelLadder.h"
#include "GeoPixelStaveSupport.h"
#include "GeoPixelModule.h"
#include "GeoPixelSiCrystal.h"
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

#include "GaudiKernel/PhysicalConstants.h"

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
  double length = m_gmt_mgr->PixelLadderLength();
  double safety = 0.01*Gaudi::Units::mm; 

  m_width = calcWidth();
  m_thicknessP = 0.5 * m_gmt_mgr->PixelLadderThickness();
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

  // If upper and lower thicknesses are within 100 um. Make them the same.
  if (std::abs(m_thicknessP - m_thicknessN) < 0.1*Gaudi::Units::mm) {
    m_thicknessP = std::max(m_thicknessP,m_thicknessN); 
    m_thicknessN = m_thicknessP;
    double halfThickness = m_thicknessP;
    ladderShape = new GeoBox(halfThickness, m_width/2., length/2.);
  } 
  else if (m_gmt_mgr->PixelBentStaveNModule() != 0)
    {
      // Calculate thickness from bent stave part
      double angle              = m_gmt_mgr->PixelLadderBentStaveAngle() * Gaudi::Units::pi / 180.0;
      double BentStaveThickness = double(m_gmt_mgr->PixelBentStaveNModule()) * m_gmt_mgr->PixelLadderModuleDeltaZ() * sin(angle);
      
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
      const GeoShape & shiftedBox = (*box) << GeoTrf::TranslateX3D(shift);
      ladderShape = &shiftedBox; 
    }
  else if (!(m_gmt_mgr->PixelStaveLayout()>3&& m_gmt_mgr->PixelStaveLayout()<7)){
    double halfThickness = 0.5*(m_thicknessP+m_thicknessN);
    double shift = 0.5*(m_thicknessP-m_thicknessN);
    GeoBox * box = new GeoBox(halfThickness, m_width/2., length/2.);
    const GeoShape & shiftedBox = (*box) << GeoTrf::TranslateX3D(shift);
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
      m_gmt_mgr->msg(MSG::ERROR)<<"No ladder shape could be defined "<<endmsg;      
    }

  if(not ladderShape)
  { 
     m_gmt_mgr->msg(MSG::ERROR)<<"No ladder shape could be defined in "<<__FILE__<<endmsg;
     exit(EXIT_FAILURE);
  } else {
    const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
    m_theLadder = new GeoLogVol("Ladder",ladderShape,air);
    m_theLadder->ref();
  }
  m_thickness = 2*std::max(m_thicknessN,m_thicknessP);
}

GeoPixelLadder::~GeoPixelLadder(){
  m_theLadder->unref();
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
  
  bool isBLayer=(m_gmt_mgr->GetLD() == 0);
  bool isModule3D=true;
  if (m_gmt_mgr->PixelStaveLayout()<5) isModule3D=false;
  GeoPixelSiCrystal theSensor3D(isBLayer,isModule3D);
  GeoPixelModule pm3D(theSensor3D);
  //  double pm3DLength=pm3D.Length();

  // Pixel module parameters
  int staveLayout = m_gmt_mgr->PixelStaveLayout();
  int nbPixelModule=m_gmt_mgr->PixelNModule();
  int nbPixelPlanarModule=0;
  int nbPixel3DModule=0;
  bool bDetailedStaveLayout=false;
  double pixelModuleGap=m_gmt_mgr->IBLStaveModuleGap();
  double zNegStavePos= 0.0;
  double planarLength=0.0;
  double v3DHalfLength=0.0;
  double v3DLength=0.0;

  bool bVerbose= (m_gmt_mgr->msgLvl(MSG::DEBUG));

  // Radial shift of Si3D module in case its thickness is greater that SiPl module's one
  double Si3DRadialShift=0.;
  double Si3DLateralShift=0.;
  if(isModule3D)
    {
      double SiPlThick = pm.ThicknessP();
      double Si3DThick = pm3D.ThicknessP();
      if(Si3DThick>SiPlThick) Si3DRadialShift=-(Si3DThick-SiPlThick);
      //      m_gmt_mgr->msg(MSG::INFO)<<"Si3D/SiPl radial shift : "<<Si3DRadialShift<<"  Pl/3D : "<<SiPlThick<<" "<<Si3DThick<<endmsg;

      double MechanicalStaveOffset = m_gmt_mgr->IBLStaveMechanicalStaveOffset();
      double MechanicalStaveOffset3D = m_gmt_mgr->IBLStaveMechanicalStaveOffset(true);
      Si3DLateralShift= -(MechanicalStaveOffset3D-MechanicalStaveOffset);
    }

  // Module 3D geometry (IBL planar and 3D modules)
  if(m_gmt_mgr->ibl() && m_gmt_mgr->GetLD()==0)
    {
      if(staveLayout==4)
	{
	  nbPixelModule=m_gmt_mgr->IBLStaveModuleNumber_AllPlanar();
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

	  //	  m_gmt_mgr->msg(MSG::INFO)<<"GeoPixelLadder : "<<nbPixelModule<<" "<<nbPixelPlanarModule<<" + "<<nbPixel3DModule<<"   planar and 3D Lengths : "<<planarLength<<" "<<v3DLength<<endmsg;
	}
    }

  for(int ii = 0; ii < nbPixelModule; ii++) {

    int etaModule = m_gmt_mgr->PixelModuleEtaFromIndex(ii);
    m_gmt_mgr->SetEta(etaModule);

//
// For standard ATLAS stave shift down the central module, as its flag is -1.
// PixelModuleStaggerDistance is zero for standard ATLAS geometry.
// Not sure why PixelModuleDrDistance is defined as twice the shift.
// For SLHC (later versions) PixelModuleDrDistance is 0 and PixelModuleStaggerDistance is non zero for
// some layers. 
//
    
    double xpos = m_gmt_mgr->PixelModuleStaggerDistance() *  m_gmt_mgr->PixelModuleStaggerSign(etaModule)
      + m_gmt_mgr->PixelModuleDrDistance()*m_gmt_mgr->PixelModuleShiftFlag(etaModule)/2.; 
//
// Get the z position from the db
//
    double zpos = m_gmt_mgr->PixelModuleZPosition(etaModule);
    bool b3DModule=false;
    double xposShift=0.;
    double yposShift=0.;

    // detailed stave model : parameters are taken from PixelIBLStave table instead of PixelStave
    if(m_gmt_mgr->ibl() && bDetailedStaveLayout && m_gmt_mgr->GetLD()==0)
      {
	int moduleIndex =  m_gmt_mgr->PixelModuleIndexFromEta(etaModule);  

	if(m_gmt_mgr->PixelStaveLayout()==4)   /// 100 % planar modules
	  zpos=(pm.Length()+m_gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - 0.5*(m_gmt_mgr->IBLStaveModuleNumber_AllPlanar()-1));
	else if(m_gmt_mgr->PixelStaveLayout()==5)   /// 75/25  planar/3D
	  {

	    if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"Eta : cmpt/index "<<ii<<" "<<moduleIndex<<endmsg;

	    if(ii<nbPixel3DModule/2) // zneg 3D pixel area
	      {
		b3DModule=true;
		zpos=(pm3D.Length()+m_gmt_mgr->IBLStaveModuleGap()) * moduleIndex;
		zpos+=pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
		if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"left 3D module "<<pm3D.Length()<<" "<<zpos<<endmsg;
	      }
	    else if(ii>nbPixelPlanarModule+nbPixel3DModule/2-1)  // zpos 3D pixel area
	      {
		if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"right 3D module "<<pm3D.Length()<<" "<<zpos<<endmsg;
		b3DModule=true;
		zpos=v3DHalfLength+planarLength+m_gmt_mgr->IBLStaveModuleGap();
		zpos+=(pm3D.Length()+m_gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule/2 - nbPixelPlanarModule); 
		zpos+=pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
	      }
	    else  // planar pixel area
	      {
		zpos=v3DHalfLength+(pm.Length()+m_gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule/2); 
		zpos+=pm.Length()*0.5;
		if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"planar module "<<pm.Length()<<" "<<zpos<<endmsg;
		xposShift=0.;
	      }

	    // Shift vs half stave length
	    zpos=zNegStavePos+zpos;

	  }
	else if(m_gmt_mgr->PixelStaveLayout()==6)   /// 50/50  planar/3D
	  {

	    if(ii<nbPixel3DModule) // zneg 3D pixel area
	      {
		if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"left 3D module "<<pm3D.Length()<<" "<<moduleIndex<<endmsg;
		b3DModule=true;
		zpos=(pm3D.Length()+pixelModuleGap) * moduleIndex;
		zpos=zNegStavePos+zpos+pm3D.Length()*0.5;
		xposShift=Si3DRadialShift;
		yposShift=Si3DLateralShift;
	      }
	    else  // zpos 3D pixel area
	      {
		if(bVerbose)m_gmt_mgr->msg(MSG::DEBUG)<<"right plannar module "<<pm.Length()<<" "<<moduleIndex<<endmsg;
		zpos=(pm.Length()+m_gmt_mgr->IBLStaveModuleGap()) * (moduleIndex - nbPixel3DModule);
		zpos+=pm.Length()*0.5+pixelModuleGap*0.5;
	      }
	  }
      }

    
    GeoTrf::Translation3D modulepos(xpos+xposShift,yposShift,zpos);
//
//  Rotate if module is inclined.
//
    GeoTrf::RotateY3D rm(m_gmt_mgr->PixelModuleAngle()*m_gmt_mgr->PixelModuleAngleSign(etaModule) );
//
// Place the Module
//
    GeoVPhysVol* modulephys; // = pm.Build() ;

    if(!b3DModule)
      modulephys = pm.Build();
    else
      modulephys = pm3D.Build();

    std::ostringstream nameTag; 
    nameTag << "ModuleBrl" << m_gmt_mgr->Eta();
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoAlignableTransform* xform;

    xform = new GeoAlignableTransform(GeoTrf::Transform3D(modulepos*rm));
    ladderPhys->add(tag);
    ladderPhys->add(new GeoIdentifierTag(m_gmt_mgr->Eta() ) );
    ladderPhys->add(xform);
    ladderPhys->add(modulephys );

    // Now store the xform by identifier:
    Identifier id;
    if(!b3DModule)  id = m_theSensor.getID();
    else id = theSensor3D.getID();
    m_DDmgr->addAlignableTransform(0,id,xform,modulephys);
    
  }
  //Add the TMT or other stave support
  if (m_staveSupport) {
    ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
    ladderPhys->add(m_staveSupport->getPhysVol()); 
  }

  
  if (m_gmt_mgr->PixelBentStaveNModule() != 0)
    {
      std::cout << "SB: Adding bent stave support" << std::endl;
      double thickness = m_gmt_mgr->PixelLadderSupportThickness();
      double width     = m_gmt_mgr->PixelLadderSupportWidth();
      double length    = m_gmt_mgr->PixelLadderSupportLength();
      double xOffset   = m_gmt_mgr->PixelLadderServicesX();
      //      double yOffset   = m_gmt_mgr->PixelLadderServicesY();
      //      int staveIndex   = m_gmt_mgr->PixelStaveIndex(m_gmt_mgr->GetLD());
      
      // Bent stave half length  = 0.5 * nModules * ModuleSize
      double bentStaveHalfLength = 0.5 * double(m_gmt_mgr->PixelBentStaveNModule()) * m_gmt_mgr->PixelLadderModuleDeltaZ();
      
      // Create bent stave
      GeoBox * shapeSupBent = new GeoBox(0.5*thickness, 0.5*width, bentStaveHalfLength);
      
      //      std::string matName = m_gmt_mgr->getMaterialName("StaveSupportOuter", m_gmt_mgr->GetLD(), staveIndex);
      //      std::string matName = m_gmt_mgr->getMaterialName("StaveSupport", m_gmt_mgr->GetLD(), staveIndex);
      //      const GeoMaterial* materialSup = m_mat_mgr->getMaterialForVolume(matName,shapeSupBent->volume());
      const GeoMaterial* materialSup = m_mat_mgr->getMaterial("pix::StaveSupportBase");
      
      double ang = m_gmt_mgr->PixelLadderBentStaveAngle() * Gaudi::Units::pi / 180.0;
      double xst = xOffset - (bentStaveHalfLength * sin(ang)); 
      
      // Construct bent stave at negative z
      GeoLogVol* logVolBentNeg = new GeoLogVol("StaveSupportBentNeg2",shapeSupBent,materialSup);
      GeoPhysVol* physVolBentNeg = new GeoPhysVol(logVolBentNeg);
      GeoTrf::RotateY3D rmNeg(ang);
      double zstneg = -length/2.0 - (bentStaveHalfLength * cos(ang));
      GeoTrf::Translation3D stavePosNeg(xst,0.,zstneg);
      ladderPhys->add(new GeoTransform(GeoTrf::Transform3D(stavePosNeg*rmNeg)));
      ladderPhys->add(physVolBentNeg); 
      
      // COnstruct bent stave at positive z
      GeoLogVol* logVolBentPos = new GeoLogVol("StaveSupportBentPos2",shapeSupBent,materialSup);
      GeoPhysVol* physVolBentPos = new GeoPhysVol(logVolBentPos);
      GeoTrf::RotateY3D rmPos(-ang);
      double zstpos = length/2.0 + (bentStaveHalfLength * cos(ang));
      GeoTrf::Translation3D stavePosPos(xst,0.,zstpos);
      ladderPhys->add(new GeoTransform(GeoTrf::Transform3D(stavePosPos*rmPos)));
      ladderPhys->add(physVolBentPos);       
    }


  return ladderPhys;
}


double GeoPixelLadder::calcThickness() {
  //
  // The module thickness is copied from the module. Should improve this
  // to avoid duplication of code
  //

  const double safety = 0.01*Gaudi::Units::mm; 
  double clearance = m_gmt_mgr->PixelLadderThicknessClearance();
  clearance = std::max(clearance, safety);

  double thickn = 0.5 * m_gmt_mgr->PixelBoardThickness() + m_gmt_mgr->PixelHybridThickness() + clearance;
  double thickp = 0.5 * m_gmt_mgr->PixelBoardThickness() + m_gmt_mgr->PixelChipThickness() + m_gmt_mgr->PixelChipGap() + clearance;
  double thick = std::max(thickn, thickp); 

  double length =  std::max(m_gmt_mgr->PixelBoardLength(), std::max(m_gmt_mgr->PixelHybridLength(), m_gmt_mgr->PixelChipLength())); 
  double tiltThick = 0.5*length * sin(std::abs(m_gmt_mgr->PixelModuleAngle())) + thick * cos(m_gmt_mgr->PixelModuleAngle());
  
  // take into account stagger and tilt
  //PixelModuleDrDistance can be -ve
  double thick1 = std::max(thick + 0.5*m_gmt_mgr->PixelModuleDrDistance(), thick - 0.5*m_gmt_mgr->PixelModuleDrDistance());
  double thick2 = tiltThick + m_gmt_mgr->PixelModuleStaggerDistance();
		    
  thick = std::max(thick1,thick2);
		    
  return 2*thick;
}


double GeoPixelLadder::calcWidth() {
  //
  // The width is the maximum among the component widths
  //
  const double clearanceRphi = m_gmt_mgr->PixelLadderWidthClearance(); // From Grant : no clash with big pigtail part
  double width = max( max(
                     m_gmt_mgr->PixelBoardWidth(),
                     m_gmt_mgr->PixelHybridWidth()),
                 m_gmt_mgr->PixelChipWidth());
  //width += 5.8; // New DC3 ! to allow module movement
  width += 2*clearanceRphi; // New DC3 ! to allow module movement
  return width;
}


