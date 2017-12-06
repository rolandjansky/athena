/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SquirrelCageFactory.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoBox.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include <iostream>


SquirrelCageFactory::SquirrelCageFactory(const InDetDD::AthenaComps * athenaComps, 
					 InDetMaterialManager * matManager)
  : InDetDD::SubDetectorFactoryBase(athenaComps, matManager)
{  
}


SquirrelCageFactory::~SquirrelCageFactory()
{
}



//## Other Operations (implementation)
void SquirrelCageFactory::create(GeoPhysVol *mother)
{

  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  std::string railversion =  rdbAccessSvc()->getChildTag("IDDetailedRail",indetVersionKey.tag(),indetVersionKey.node());
  if(!railversion.empty()) {
    // ______________________________________________________________________________________________________________________
    // Geometry with ID rail

  IRDBRecordset_ptr cage = rdbAccessSvc()->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());
  //It is need for "U" shape support
  IRDBRecordset_ptr sctGenServices = rdbAccessSvc()->getRecordsetPtr("SCTGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Get table version
  // This is a work around to allow running with older DB releases.
  std::string sqversionStr = rdbAccessSvc()->getChildTag("SquirrelCage", indetVersionKey.tag(), indetVersionKey.node());
  size_t pos = sqversionStr.find("-");
  int sqversion = 0;
  if (pos != std::string::npos && pos+1 != sqversionStr.size()) {
    std::istringstream tmpStr(sqversionStr.substr(pos+1));
    tmpStr >> sqversion;
  }
  msg(MSG::DEBUG) << sqversionStr << " : " << sqversion << endmsg;
  

//----------------------------------------------------------------------------------
//     Squirrel cage rings
//  Default (initial) ring parameters
//
//     double rminInt    = 1074.0*CLHEP::mm;
//     double ringThick  = 4.0*CLHEP::mm;
//     double ringGap    = 20.*CLHEP::mm;
//     double ringWid    = 40.*CLHEP::mm;
//
    double rminInt    = (*cage)[0]->getDouble("RINGRMIN")*CLHEP::mm;
    double ringThick  = (*cage)[0]->getDouble("RINGTHICK")*CLHEP::mm;
    double ringGap    = (*cage)[0]->getDouble("RINGGAP")*CLHEP::mm;
    double ringWid    = (*cage)[0]->getDouble("RINGWIDTH")*CLHEP::mm;
//
//--- Default (initial) z positions
//     double zposFirstRing  = 805.0*CLHEP::mm+161.0*CLHEP::mm;
//     double zposGap1  = 390.*CLHEP::mm;
//     double zposGap2  = 402.*CLHEP::mm;
//     double zposGap3  = 446.*CLHEP::mm;
//     double zposGap4  = 331.*CLHEP::mm;
//
    double zposFirstRing  = (*cage)[0]->getDouble("ZBASE")*CLHEP::mm;
    double zposGap1  = (*cage)[0]->getDouble("ZGAP1")*CLHEP::mm;
    double zposGap2  = (*cage)[0]->getDouble("ZGAP2")*CLHEP::mm;
    double zposGap3  = (*cage)[0]->getDouble("ZGAP3")*CLHEP::mm;
    double zposGap4  = (*cage)[0]->getDouble("ZGAP4")*CLHEP::mm;
//
// Now support ring
//     double rminSup    = 830.0*CLHEP::mm;
//     double supThick   = 90.0*CLHEP::mm;
//     double supWid     = 12.0*CLHEP::mm;
//
    double rminSup    = (*cage)[0]->getDouble("SUPRMIN")*CLHEP::mm;
    double supThick   = (*cage)[0]->getDouble("SUPTHICK")*CLHEP::mm;
    double supWid     = (*cage)[0]->getDouble("SUPWIDTH")*CLHEP::mm;
//
    double zposSupRing  = zposFirstRing+ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;



    const GeoMaterial*  ringMat = materialManager()->getMaterial((*cage)[0]->getString("MATERIAL")); 

    IRDBRecordset_ptr commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon",indetVersionKey.tag(), indetVersionKey.node());
    if(commonParameters->size()==0)
      commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon","IDDetRailCommon-00");


    double yRailSup      = (*commonParameters)[0]->getDouble("YRAILSUP");
    double yRailSupPart3 = (*commonParameters)[0]->getDouble("YRAILSUPPART3");
    double gapOfRSF      = (*commonParameters)[0]->getDouble("GAPOFRSF");


//Parameters of "U shape" support - these parameters aren`t in database
//    ___________
//   |_____2_____|
//   | |         
//   | |
//   |1|
//   | |
//   |_|_________
//   |_____2_____|
//

    //double zLengthUS = 1992;

    double bUSS = 0.; double eUSS  = 0.;
    for (unsigned int i = 0; i < sctGenServices->size();  i++)
    {
     if ((*sctGenServices)[i]->getString("NAME") == "HSP")
     {
      bUSS = (*sctGenServices)[i]->getDouble("ZMAX");
     }
     if ((*sctGenServices)[i]->getString("NAME") == "FwdRadialServ")
     {
      eUSS = (*sctGenServices)[i]->getDouble("ZMIN");
     }
    }
    
    double zLengthUS = eUSS - bUSS;

    std::unique_ptr<IRDBQuery> queryUSP = rdbAccessSvc()->getQuery("IDDetRailUSP",indetVersionKey.tag(), indetVersionKey.node());
    if(!queryUSP)
      queryUSP = rdbAccessSvc()->getQuery("IDDetRailUSP","IDDetRailUSP-00");

    queryUSP->setOrder("USP_ID");
    queryUSP->execute();
 
    queryUSP->next();
    double xDepthUSP1 = queryUSP->data<double>("IDDETRAILUSP_DATA.XDEPTH");
    double yWidthUSP1 = queryUSP->data<double>("IDDETRAILUSP_DATA.YWIDTH");
    
    queryUSP->next();
    double xDepthUSP2 = queryUSP->data<double>("IDDETRAILUSP_DATA.XDEPTH");
    double yWidthUSP2 = queryUSP->data<double>("IDDETRAILUSP_DATA.YWIDTH");
   
    queryUSP->next();
    double xDepthUSP3 = queryUSP->data<double>("IDDETRAILUSP_DATA.XDEPTH");
    double yWidthUSP3 = queryUSP->data<double>("IDDETRAILUSP_DATA.YWIDTH");

    queryUSP->finalize();
//Definition of "U shape" support    

    GeoBox* UShapePart1 = new GeoBox(xDepthUSP1/2., yWidthUSP1/2., zLengthUS/2.);
    GeoBox* UShapePart2 = new GeoBox(xDepthUSP2/2., yWidthUSP2/2., zLengthUS/2.);
    GeoBox* UShapePart3 = new GeoBox(xDepthUSP3/2., yWidthUSP3/2., zLengthUS/2.);

    CLHEP::Hep3Vector trans1UShapeP2(xDepthUSP2/2. - xDepthUSP1/2., yWidthUSP1/2. + yWidthUSP2/2., 0.);
    CLHEP::Hep3Vector trans2UShapeP2(- xDepthUSP2/2. + xDepthUSP1/2., yWidthUSP1/2. + yWidthUSP2/2., 0.);

    CLHEP::Hep3Vector trans1UShapeP3(xDepthUSP3/2. - xDepthUSP1/2., - yWidthUSP1/2. - yWidthUSP3/2., 0.);
    CLHEP::Hep3Vector trans2UShapeP3(- xDepthUSP3/2. + xDepthUSP1/2., - yWidthUSP1/2. - yWidthUSP3/2., 0.);

//Determining the exact place of "U" Shape support

    double coordY = yWidthUSP1/2. - yRailSup + (yRailSupPart3 - gapOfRSF/2.);

//Logical Volume
    const GeoLogVol* UShapeLP1 = new GeoLogVol("UShapeSupport1", UShapePart1, ringMat);
    const GeoLogVol* UShapeLP2 = new GeoLogVol("UShapeSupport2", UShapePart2, ringMat);
    const GeoLogVol* UShapeLP3 = new GeoLogVol("UShapeSupport3", UShapePart3, ringMat);

//Physical Volume
    GeoVPhysVol* UShapePP1 = new GeoPhysVol(UShapeLP1);
    GeoVPhysVol* UShapePP2 = new GeoPhysVol(UShapeLP2);
    GeoVPhysVol* UShapePP3 = new GeoPhysVol(UShapeLP3);

//Squirrel Cage Rings
//Inner 
   
    double phiICRT = asin((yWidthUSP1/2. + yWidthUSP2 + coordY) / rminInt);
    double DphiICRT = CLHEP::pi - 2*phiICRT;
 
    double phiICRB = asin((yWidthUSP1/2. + yWidthUSP2 - coordY) / rminInt);
    double DphiICRB = CLHEP::pi - 2*phiICRB;

    GeoTubs* ICRT = new GeoTubs(rminInt, rminInt + ringThick, ringWid/2., phiICRT, DphiICRT);
  
    GeoTubs* ICRB = new GeoTubs(rminInt, rminInt + ringThick, ringWid/2., CLHEP::pi + phiICRB, DphiICRB);


    const GeoLogVol* ICRTLog = new GeoLogVol("SQringIntTop", ICRT, ringMat);
    GeoVPhysVol* ICRTPhys = new GeoPhysVol(ICRTLog);

    const GeoLogVol* ICRBLog = new GeoLogVol("SQringIntBottom", ICRB, ringMat);
    GeoVPhysVol* ICRBPhys = new GeoPhysVol(ICRBLog);

//Outer
  
    double phiECRT = asin((yWidthUSP1/2. + yWidthUSP2 + coordY) / (rminInt+ringGap+ringThick));
    double DphiECRT = CLHEP::pi - 2*phiECRT;

    double phiECRB = asin((yWidthUSP1/2. + yWidthUSP2 - coordY) / (rminInt+ringGap+ringThick));
    double DphiECRB = CLHEP::pi - 2*phiECRB;

//     std::cerr << "phiET: " << phiECRT << ", DphiET: " << DphiECRT << std::endl;
//     std::cerr << "phiIT: " << phiICRT << ", DphiIT: " << DphiICRT << std::endl;
//     std::cerr << "phiEB: " << phiECRB << ", DphiEB: " << DphiECRB << std::endl;
//     std::cerr << "phiIB: " << phiICRB << ", DphiIB: " << DphiICRB << std::endl;

    GeoTubs* ECRT = new GeoTubs(rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2., phiECRT, DphiECRT);
    GeoTubs* ECRB = new GeoTubs(rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2., CLHEP::pi + phiECRB, DphiECRB);

    const GeoLogVol* ECRTLog = new GeoLogVol("SQringExtTop", ECRT, ringMat);
    GeoVPhysVol* ECRTPhys = new GeoPhysVol(ECRTLog);
  
    const GeoLogVol* ECRBLog = new GeoLogVol("SQringExtBottom", ECRB, ringMat);
    GeoVPhysVol* ECRBPhys = new GeoPhysVol(ECRBLog);

//SupportRing
    GeoTube* ringSup = new GeoTube( rminSup, rminSup+supThick, supWid/2.);

    const GeoLogVol* ringLogSup = new GeoLogVol("SQringSup",ringSup,ringMat);
    GeoVPhysVol* ringPhysSup = new GeoPhysVol(ringLogSup);

//Positioning and adding to geometry
//Rings
        
    CLHEP::Hep3Vector  servpos1(0.,0., zposFirstRing+ringWid/2.);
    CLHEP::Hep3Vector  servpos2(0.,0.,-zposFirstRing-ringWid/2.);
    CLHEP::Hep3Vector  servpos3(0.,0., zposFirstRing+ringWid*3./2. + zposGap1);
    CLHEP::Hep3Vector  servpos4(0.,0.,-zposFirstRing-ringWid*3./2. - zposGap1);
    CLHEP::Hep3Vector  servpos5(0.,0., zposFirstRing+ringWid*5./2. + zposGap1 + zposGap2);
    CLHEP::Hep3Vector  servpos6(0.,0.,-zposFirstRing-ringWid*5./2. - zposGap1 - zposGap2);
    CLHEP::Hep3Vector  servpos7(0.,0., zposFirstRing+ringWid*7./2. + zposGap1 + zposGap2 + zposGap3);
    CLHEP::Hep3Vector  servpos8(0.,0.,-zposFirstRing-ringWid*7./2. - zposGap1 - zposGap2 - zposGap3);
    CLHEP::Hep3Vector  servpos9(0.,0., zposFirstRing+ringWid*9./2. + zposGap1 + zposGap2 + zposGap3 + zposGap4);
    CLHEP::Hep3Vector servpos10(0.,0.,-zposFirstRing-ringWid*9./2. - zposGap1 - zposGap2 - zposGap3 - zposGap4);
    GeoTransform *xform1  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos1));
    GeoTransform *xform2  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,0.),servpos2));
    GeoTransform *xform3  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos3));
    GeoTransform *xform4  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,0.),servpos4));
    GeoTransform *xform5  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos5));
    GeoTransform *xform6  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,0.),servpos6));
    GeoTransform *xform7  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos7));
    GeoTransform *xform8  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,0.),servpos8));
    GeoTransform *xform9  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos9));
    GeoTransform *xform10 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,0.),servpos10));
    
    mother->add(xform1);   mother->add(ICRTPhys);   mother->add(xform1);   mother->add(ECRTPhys);
    mother->add(xform2);   mother->add(ICRTPhys);   mother->add(xform2);   mother->add(ECRTPhys);
    mother->add(xform3);   mother->add(ICRTPhys);   mother->add(xform3);   mother->add(ECRTPhys);
    mother->add(xform4);   mother->add(ICRTPhys);   mother->add(xform4);   mother->add(ECRTPhys);
    mother->add(xform5);   mother->add(ICRTPhys);   mother->add(xform5);   mother->add(ECRTPhys);
    mother->add(xform6);   mother->add(ICRTPhys);   mother->add(xform6);   mother->add(ECRTPhys);
    mother->add(xform7);   mother->add(ICRTPhys);   mother->add(xform7);   mother->add(ECRTPhys);
    mother->add(xform8);   mother->add(ICRTPhys);   mother->add(xform8);   mother->add(ECRTPhys);
    mother->add(xform9);   mother->add(ICRTPhys);   mother->add(xform9);   mother->add(ECRTPhys);
    mother->add(xform10);  mother->add(ICRTPhys);   mother->add(xform10);  mother->add(ECRTPhys);

    mother->add(xform1);   mother->add(ICRBPhys);   mother->add(xform1);   mother->add(ECRBPhys);
    mother->add(xform2);   mother->add(ICRBPhys);   mother->add(xform2);   mother->add(ECRBPhys);
    mother->add(xform3);   mother->add(ICRBPhys);   mother->add(xform3);   mother->add(ECRBPhys);
    mother->add(xform4);   mother->add(ICRBPhys);   mother->add(xform4);   mother->add(ECRBPhys);
    mother->add(xform5);   mother->add(ICRBPhys);   mother->add(xform5);   mother->add(ECRBPhys);
    mother->add(xform6);   mother->add(ICRBPhys);   mother->add(xform6);   mother->add(ECRBPhys);
    mother->add(xform7);   mother->add(ICRBPhys);   mother->add(xform7);   mother->add(ECRBPhys);
    mother->add(xform8);   mother->add(ICRBPhys);   mother->add(xform8);   mother->add(ECRBPhys);
    mother->add(xform9);   mother->add(ICRBPhys);   mother->add(xform9);   mother->add(ECRBPhys);
    mother->add(xform10);  mother->add(ICRBPhys);   mother->add(xform10);  mother->add(ECRBPhys);

// "U Shape" support

    CLHEP::Hep3Vector USSupCoordX(rminInt + xDepthUSP1/2., 0, 0);
   
    GeoTransform* transUSS;
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.)));
    mother->add(transUSS);   mother->add(UShapePP1);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.) + trans1UShapeP2 ));
    mother->add(transUSS);   mother->add(UShapePP2);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.) + trans1UShapeP3 ));
    mother->add(transUSS);   mother->add(UShapePP3);

    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), -USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.)));
    mother->add(transUSS);   mother->add(UShapePP1);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), -USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.) + trans2UShapeP2 ));
    mother->add(transUSS);   mother->add(UShapePP2);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), -USSupCoordX + CLHEP::Hep3Vector(0, coordY, bUSS + zLengthUS/2.) + trans2UShapeP3 ));
    mother->add(transUSS);   mother->add(UShapePP3);

    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.)));
    mother->add(transUSS);   mother->add(UShapePP1);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.) + trans1UShapeP2 ));
    mother->add(transUSS);   mother->add(UShapePP2);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.) + trans1UShapeP3 ));
    mother->add(transUSS);   mother->add(UShapePP3);

    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.)));
    mother->add(transUSS);   mother->add(UShapePP1);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.) + trans2UShapeP2 ));
    mother->add(transUSS);   mother->add(UShapePP2);
    transUSS = new GeoTransform(HepGeom::Transform3D(CLHEP::HepRotation(), - USSupCoordX - CLHEP::Hep3Vector(0, - coordY, bUSS + zLengthUS/2.) + trans2UShapeP3 ));
    mother->add(transUSS);   mother->add(UShapePP3);



//Support ring positioning

    CLHEP::Hep3Vector  suppos1(0.,0., zposSupRing+supWid/2.);
    CLHEP::Hep3Vector  suppos2(0.,0.,-zposSupRing-supWid/2.);
    GeoTransform *sform1  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          suppos1));
    GeoTransform *sform2  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),suppos2));
    mother->add(sform1);
    mother->add(ringPhysSup);
    mother->add(sform2);
    mother->add(ringPhysSup);
  }
  else {
    // ______________________________________________________________________________________________________________________
    // Geometry without ID rail
  IRDBRecordset_ptr cage = rdbAccessSvc()->getRecordsetPtr("SquirrelCage",  indetVersionKey.tag(), indetVersionKey.node());

  // Get table version
  // This is a work around to allow running with older DB releases.
  std::string sqversionStr = rdbAccessSvc()->getChildTag("SquirrelCage", indetVersionKey.tag(), indetVersionKey.node());
  size_t pos = sqversionStr.find("-");
  int sqversion = 0;
  if (pos != std::string::npos && pos+1 != sqversionStr.size()) {
    std::istringstream tmpStr(sqversionStr.substr(pos+1));
    tmpStr >> sqversion;
  }
  msg(MSG::DEBUG) << sqversionStr << " : " << sqversion << endmsg;
  

//----------------------------------------------------------------------------------
//     Squirrel cage rings
//  Default (initial) ring parameters
//
    double rminInt    = 1074.0*CLHEP::mm;
    double ringThick  = 4.0*CLHEP::mm;
    double ringGap    = 20.*CLHEP::mm;
    double ringWid    = 40.*CLHEP::mm;
//
    rminInt    = (*cage)[0]->getDouble("RINGRMIN")*CLHEP::mm;
    ringThick  = (*cage)[0]->getDouble("RINGTHICK")*CLHEP::mm;
    ringGap    = (*cage)[0]->getDouble("RINGGAP")*CLHEP::mm;
    ringWid    = (*cage)[0]->getDouble("RINGWIDTH")*CLHEP::mm;
//
//--- Default (initial) z positions
    double zposFirstRing  = 805.0*CLHEP::mm+161.0*CLHEP::mm;
    double zposGap1  = 390.*CLHEP::mm;
    double zposGap2  = 402.*CLHEP::mm;
    double zposGap3  = 446.*CLHEP::mm;
    double zposGap4  = 331.*CLHEP::mm;
//
    zposFirstRing  = (*cage)[0]->getDouble("ZBASE")*CLHEP::mm;
    zposGap1  = (*cage)[0]->getDouble("ZGAP1")*CLHEP::mm;
    zposGap2  = (*cage)[0]->getDouble("ZGAP2")*CLHEP::mm;
    zposGap3  = (*cage)[0]->getDouble("ZGAP3")*CLHEP::mm;
    zposGap4  = (*cage)[0]->getDouble("ZGAP4")*CLHEP::mm;
//
// Now support ring
    double rminSup    = 830.0*CLHEP::mm;
    double supThick   = 90.0*CLHEP::mm;
    double supWid     = 12.0*CLHEP::mm;
//
    rminSup    = (*cage)[0]->getDouble("SUPRMIN")*CLHEP::mm;
    supThick   = (*cage)[0]->getDouble("SUPTHICK")*CLHEP::mm;
    supWid     = (*cage)[0]->getDouble("SUPWIDTH")*CLHEP::mm;
//
    double zposSupRing  = zposFirstRing+ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
//
// Now support ribbon
    double ribWid     = 68.0*CLHEP::mm ;
    ribWid = (*cage)[0]->getDouble("RIBWIDTH")*CLHEP::mm;
    double ribLeng    = ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
    double ribThick = 0; 
    if (sqversion >= 3) ribThick = (*cage)[0]->getDouble("RIBTHICK")*CLHEP::mm;
    double safety =0.01;
    double ribThickMax = ringGap - 2*safety;
    if (ribThick == 0 || ribThick > ribThickMax) {
      ribThick = ribThickMax;
    } 
    double rmidRib =  rminInt+ringThick+0.5*ringGap; // Place rib midway between internal and external support.

    GeoTube* ringInt = new GeoTube( rminInt, rminInt+ringThick, ringWid/2.);
    GeoTube* ringExt = new GeoTube( rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2.);
    GeoTube* ringSup = new GeoTube( rminSup, rminSup+supThick, supWid/2.);
// To avoid clashes
//
//  GeoBox*  ribSup  = new GeoBox( ribThick/2, ribWid/2, ribLeng/2.);
//
    double phiWid=ribWid/(rminInt+ringThick+ringGap/2.);
    //GeoTubs*  ribSup  = new GeoTubs( rminInt+ringThick+safety, rminInt+ringGap+ringThick-safety, 
    //                                 ribLeng/2., -phiWid/2.,phiWid);
    GeoTubs*  ribSup  = new GeoTubs( rmidRib-0.5*ribThick, rmidRib+0.5*ribThick, 
                                     ribLeng/2., -phiWid/2.,phiWid);

// Build the Phys Vol
//  const GeoMaterial*  ringMat = materialManager()->getMaterial("std::Aluminium");
    const GeoMaterial*  ringMat = materialManager()->getMaterial((*cage)[0]->getString("MATERIAL"));
//  std::cout << "Squirrel cage density="<<ringMat->getDensity()<<" Rad.length="<<ringMat->getRadLength()<<'\n';

    const GeoLogVol* ringLogInt = new GeoLogVol("SQringInt",ringInt,ringMat);
    const GeoLogVol* ringLogExt = new GeoLogVol("SQringExt",ringExt,ringMat);
    const GeoLogVol* ringLogSup = new GeoLogVol("SQringSup",ringSup,ringMat);
    const GeoLogVol* ribLogSup  = new GeoLogVol("SQribSup" ,ribSup ,ringMat);
    GeoVPhysVol* ringPhysInt = new GeoPhysVol(ringLogInt);
    GeoVPhysVol* ringPhysExt = new GeoPhysVol(ringLogExt);
    GeoVPhysVol* ringPhysSup = new GeoPhysVol(ringLogSup);
    GeoVPhysVol*  ribPhysSup = new GeoPhysVol(ribLogSup);
    
    CLHEP::Hep3Vector  servpos1(0.,0., zposFirstRing+ringWid/2.);
    CLHEP::Hep3Vector  servpos2(0.,0.,-zposFirstRing-ringWid/2.);
    CLHEP::Hep3Vector  servpos3(0.,0., zposFirstRing+ringWid*3./2. + zposGap1);
    CLHEP::Hep3Vector  servpos4(0.,0.,-zposFirstRing-ringWid*3./2. - zposGap1);
    CLHEP::Hep3Vector  servpos5(0.,0., zposFirstRing+ringWid*5./2. + zposGap1 + zposGap2);
    CLHEP::Hep3Vector  servpos6(0.,0.,-zposFirstRing-ringWid*5./2. - zposGap1 - zposGap2);
    CLHEP::Hep3Vector  servpos7(0.,0., zposFirstRing+ringWid*7./2. + zposGap1 + zposGap2 + zposGap3);
    CLHEP::Hep3Vector  servpos8(0.,0.,-zposFirstRing-ringWid*7./2. - zposGap1 - zposGap2 - zposGap3);
    CLHEP::Hep3Vector  servpos9(0.,0., zposFirstRing+ringWid*9./2. + zposGap1 + zposGap2 + zposGap3 + zposGap4);
    CLHEP::Hep3Vector servpos10(0.,0.,-zposFirstRing-ringWid*9./2. - zposGap1 - zposGap2 - zposGap3 - zposGap4);
    GeoTransform *xform1  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos1));
    GeoTransform *xform2  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),servpos2));
    GeoTransform *xform3  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos3));
    GeoTransform *xform4  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),servpos4));
    GeoTransform *xform5  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos5));
    GeoTransform *xform6  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),servpos6));
    GeoTransform *xform7  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos7));
    GeoTransform *xform8  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),servpos8));
    GeoTransform *xform9  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          servpos9));
    GeoTransform *xform10 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),servpos10));
    mother->add(xform1);
    mother->add(ringPhysInt);
    mother->add(xform1);
    mother->add(ringPhysExt);
    mother->add(xform2);
    mother->add(ringPhysInt);
    mother->add(xform2);
    mother->add(ringPhysExt);
    mother->add(xform3);
    mother->add(ringPhysInt);
    mother->add(xform3);
    mother->add(ringPhysExt);
    mother->add(xform4);
    mother->add(ringPhysInt);
    mother->add(xform4);
    mother->add(ringPhysExt);
    mother->add(xform5);
    mother->add(ringPhysInt);
    mother->add(xform5);
    mother->add(ringPhysExt);
    mother->add(xform6);
    mother->add(ringPhysInt);
    mother->add(xform6);
    mother->add(ringPhysExt);
    mother->add(xform7);
    mother->add(ringPhysInt);
    mother->add(xform7);
    mother->add(ringPhysExt);
    mother->add(xform8);
    mother->add(ringPhysInt);
    mother->add(xform8);
    mother->add(ringPhysExt);
    mother->add(xform9);
    mother->add(ringPhysInt);
    mother->add(xform9);
    mother->add(ringPhysExt);
    mother->add(xform10);
    mother->add(ringPhysInt);
    mother->add(xform10);
    mother->add(ringPhysExt);

//Support ring positioning

    CLHEP::Hep3Vector  suppos1(0.,0., zposSupRing+supWid/2.);
    CLHEP::Hep3Vector  suppos2(0.,0.,-zposSupRing-supWid/2.);
    GeoTransform *sform1  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          suppos1));
    GeoTransform *sform2  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,M_PI,0.),suppos2));
    mother->add(sform1);
    mother->add(ringPhysSup);
    mother->add(sform2);
    mother->add(ringPhysSup);

//Support ribbon positioning


    CLHEP::Hep3Vector  ribpos1( 0., 0., zposFirstRing+ribLeng/2.);
    CLHEP::Hep3Vector  ribpos2( 0., 0.,-zposFirstRing-ribLeng/2.);
    GeoTransform *rform1  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          ribpos1));
    GeoTransform *rform2  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,M_PI),ribpos1));
    GeoTransform *rform3  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),          ribpos2));
    GeoTransform *rform4  = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(0.,0.,M_PI),ribpos2));


    mother->add(rform1);
    mother->add(ribPhysSup);
    mother->add(rform2);
    mother->add(ribPhysSup);
    mother->add(rform3);
    mother->add(ribPhysSup);
    mother->add(rform4);
    mother->add(ribPhysSup);

//    std::cout << "End of SquirrelCage building !!!" << zposFirstRing<< '\n';
//----------------------------------------------------------------------------------

  }
}


