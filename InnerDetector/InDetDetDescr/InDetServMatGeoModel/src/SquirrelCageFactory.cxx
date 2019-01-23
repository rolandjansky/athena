/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SquirrelCageFactory.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
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
#include "GaudiKernel/PhysicalConstants.h"

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
//     double rminInt    = 1074.0*Gaudi::Units::mm;
//     double ringThick  = 4.0*Gaudi::Units::mm;
//     double ringGap    = 20.*Gaudi::Units::mm;
//     double ringWid    = 40.*Gaudi::Units::mm;
//
    double rminInt    = (*cage)[0]->getDouble("RINGRMIN")*Gaudi::Units::mm;
    double ringThick  = (*cage)[0]->getDouble("RINGTHICK")*Gaudi::Units::mm;
    double ringGap    = (*cage)[0]->getDouble("RINGGAP")*Gaudi::Units::mm;
    double ringWid    = (*cage)[0]->getDouble("RINGWIDTH")*Gaudi::Units::mm;
//
//--- Default (initial) z positions
//     double zposFirstRing  = 805.0*Gaudi::Units::mm+161.0*Gaudi::Units::mm;
//     double zposGap1  = 390.*Gaudi::Units::mm;
//     double zposGap2  = 402.*Gaudi::Units::mm;
//     double zposGap3  = 446.*Gaudi::Units::mm;
//     double zposGap4  = 331.*Gaudi::Units::mm;
//
    double zposFirstRing  = (*cage)[0]->getDouble("ZBASE")*Gaudi::Units::mm;
    double zposGap1  = (*cage)[0]->getDouble("ZGAP1")*Gaudi::Units::mm;
    double zposGap2  = (*cage)[0]->getDouble("ZGAP2")*Gaudi::Units::mm;
    double zposGap3  = (*cage)[0]->getDouble("ZGAP3")*Gaudi::Units::mm;
    double zposGap4  = (*cage)[0]->getDouble("ZGAP4")*Gaudi::Units::mm;
//
// Now support ring
//     double rminSup    = 830.0*Gaudi::Units::mm;
//     double supThick   = 90.0*Gaudi::Units::mm;
//     double supWid     = 12.0*Gaudi::Units::mm;
//
    double rminSup    = (*cage)[0]->getDouble("SUPRMIN")*Gaudi::Units::mm;
    double supThick   = (*cage)[0]->getDouble("SUPTHICK")*Gaudi::Units::mm;
    double supWid     = (*cage)[0]->getDouble("SUPWIDTH")*Gaudi::Units::mm;
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

    GeoTrf::Vector3D trans1UShapeP2(xDepthUSP2/2. - xDepthUSP1/2., yWidthUSP1/2. + yWidthUSP2/2., 0.);
    GeoTrf::Vector3D trans2UShapeP2(- xDepthUSP2/2. + xDepthUSP1/2., yWidthUSP1/2. + yWidthUSP2/2., 0.);

    GeoTrf::Vector3D trans1UShapeP3(xDepthUSP3/2. - xDepthUSP1/2., - yWidthUSP1/2. - yWidthUSP3/2., 0.);
    GeoTrf::Vector3D trans2UShapeP3(- xDepthUSP3/2. + xDepthUSP1/2., - yWidthUSP1/2. - yWidthUSP3/2., 0.);

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
    double DphiICRT = Gaudi::Units::pi - 2*phiICRT;
 
    double phiICRB = asin((yWidthUSP1/2. + yWidthUSP2 - coordY) / rminInt);
    double DphiICRB = Gaudi::Units::pi - 2*phiICRB;

    GeoTubs* ICRT = new GeoTubs(rminInt, rminInt + ringThick, ringWid/2., phiICRT, DphiICRT);
  
    GeoTubs* ICRB = new GeoTubs(rminInt, rminInt + ringThick, ringWid/2., Gaudi::Units::pi + phiICRB, DphiICRB);


    const GeoLogVol* ICRTLog = new GeoLogVol("SQringIntTop", ICRT, ringMat);
    GeoVPhysVol* ICRTPhys = new GeoPhysVol(ICRTLog);

    const GeoLogVol* ICRBLog = new GeoLogVol("SQringIntBottom", ICRB, ringMat);
    GeoVPhysVol* ICRBPhys = new GeoPhysVol(ICRBLog);

//Outer
  
    double phiECRT = asin((yWidthUSP1/2. + yWidthUSP2 + coordY) / (rminInt+ringGap+ringThick));
    double DphiECRT = Gaudi::Units::pi - 2*phiECRT;

    double phiECRB = asin((yWidthUSP1/2. + yWidthUSP2 - coordY) / (rminInt+ringGap+ringThick));
    double DphiECRB = Gaudi::Units::pi - 2*phiECRB;

//     std::cerr << "phiET: " << phiECRT << ", DphiET: " << DphiECRT << std::endl;
//     std::cerr << "phiIT: " << phiICRT << ", DphiIT: " << DphiICRT << std::endl;
//     std::cerr << "phiEB: " << phiECRB << ", DphiEB: " << DphiECRB << std::endl;
//     std::cerr << "phiIB: " << phiICRB << ", DphiIB: " << DphiICRB << std::endl;

    GeoTubs* ECRT = new GeoTubs(rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2., phiECRT, DphiECRT);
    GeoTubs* ECRB = new GeoTubs(rminInt+ringGap+ringThick, rminInt+2.*ringThick+ringGap, ringWid/2., Gaudi::Units::pi + phiECRB, DphiECRB);

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
        
    GeoTrf::Translate3D  servpos1(0.,0., zposFirstRing+ringWid/2.);
    GeoTrf::Translate3D  servpos2(0.,0.,-zposFirstRing-ringWid/2.);
    GeoTrf::Translate3D  servpos3(0.,0., zposFirstRing+ringWid*3./2. + zposGap1);
    GeoTrf::Translate3D  servpos4(0.,0.,-zposFirstRing-ringWid*3./2. - zposGap1);
    GeoTrf::Translate3D  servpos5(0.,0., zposFirstRing+ringWid*5./2. + zposGap1 + zposGap2);
    GeoTrf::Translate3D  servpos6(0.,0.,-zposFirstRing-ringWid*5./2. - zposGap1 - zposGap2);
    GeoTrf::Translate3D  servpos7(0.,0., zposFirstRing+ringWid*7./2. + zposGap1 + zposGap2 + zposGap3);
    GeoTrf::Translate3D  servpos8(0.,0.,-zposFirstRing-ringWid*7./2. - zposGap1 - zposGap2 - zposGap3);
    GeoTrf::Translate3D  servpos9(0.,0., zposFirstRing+ringWid*9./2. + zposGap1 + zposGap2 + zposGap3 + zposGap4);
    GeoTrf::Translate3D  servpos10(0.,0.,-zposFirstRing-ringWid*9./2. - zposGap1 - zposGap2 - zposGap3 - zposGap4);
    GeoTransform *xform1  = new GeoTransform(servpos1);
    GeoTransform *xform2  = new GeoTransform(servpos2);
    GeoTransform *xform3  = new GeoTransform(servpos3);
    GeoTransform *xform4  = new GeoTransform(servpos4);
    GeoTransform *xform5  = new GeoTransform(servpos5);
    GeoTransform *xform6  = new GeoTransform(servpos6);
    GeoTransform *xform7  = new GeoTransform(servpos7);
    GeoTransform *xform8  = new GeoTransform(servpos8);
    GeoTransform *xform9  = new GeoTransform(servpos9);
    GeoTransform *xform10 = new GeoTransform(servpos10);
    
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

    GeoTrf::Vector3D USSupCoordX(rminInt + xDepthUSP1/2., 0, 0);
   
    GeoTransform* transUSS{nullptr};
    GeoTrf::Vector3D tmpVec(0,0,0);
    tmpVec = USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.);
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP1);
    tmpVec = USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.) + trans1UShapeP2;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP2);
    tmpVec = USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.) + trans1UShapeP3 ;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP3);

    tmpVec = -USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.);
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP1);
    tmpVec =  -USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.) + trans2UShapeP2;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP2);
    tmpVec = -USSupCoordX + GeoTrf::Vector3D(0, coordY, bUSS + zLengthUS/2.) + trans2UShapeP3;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP3);

    tmpVec = USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.);
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP1);
    tmpVec = USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.) + trans1UShapeP2;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP2);
    tmpVec = USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.) + trans1UShapeP3;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP3);

    tmpVec = - USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.);
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP1);
    tmpVec = - USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.) + trans2UShapeP2;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP2);
    tmpVec = - USSupCoordX - GeoTrf::Vector3D(0, - coordY, bUSS + zLengthUS/2.) + trans2UShapeP3;
    transUSS = new GeoTransform(GeoTrf::Translate3D(tmpVec.x(),tmpVec.y(),tmpVec.z()));
    mother->add(transUSS);   mother->add(UShapePP3);



//Support ring positioning

    GeoTrf::Translate3D  suppos1(0.,0., zposSupRing+supWid/2.);
    GeoTrf::Vector3D  suppos2(0.,0.,-zposSupRing-supWid/2.);
    GeoTransform *sform1  = new GeoTransform(suppos1);
    GeoTransform *sform2  = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0,M_PI,0),suppos2));
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
    double rminInt    = 1074.0*Gaudi::Units::mm;
    double ringThick  = 4.0*Gaudi::Units::mm;
    double ringGap    = 20.*Gaudi::Units::mm;
    double ringWid    = 40.*Gaudi::Units::mm;
//
    rminInt    = (*cage)[0]->getDouble("RINGRMIN")*Gaudi::Units::mm;
    ringThick  = (*cage)[0]->getDouble("RINGTHICK")*Gaudi::Units::mm;
    ringGap    = (*cage)[0]->getDouble("RINGGAP")*Gaudi::Units::mm;
    ringWid    = (*cage)[0]->getDouble("RINGWIDTH")*Gaudi::Units::mm;
//
//--- Default (initial) z positions
    double zposFirstRing  = 805.0*Gaudi::Units::mm+161.0*Gaudi::Units::mm;
    double zposGap1  = 390.*Gaudi::Units::mm;
    double zposGap2  = 402.*Gaudi::Units::mm;
    double zposGap3  = 446.*Gaudi::Units::mm;
    double zposGap4  = 331.*Gaudi::Units::mm;
//
    zposFirstRing  = (*cage)[0]->getDouble("ZBASE")*Gaudi::Units::mm;
    zposGap1  = (*cage)[0]->getDouble("ZGAP1")*Gaudi::Units::mm;
    zposGap2  = (*cage)[0]->getDouble("ZGAP2")*Gaudi::Units::mm;
    zposGap3  = (*cage)[0]->getDouble("ZGAP3")*Gaudi::Units::mm;
    zposGap4  = (*cage)[0]->getDouble("ZGAP4")*Gaudi::Units::mm;
//
// Now support ring
    double rminSup    = 830.0*Gaudi::Units::mm;
    double supThick   = 90.0*Gaudi::Units::mm;
    double supWid     = 12.0*Gaudi::Units::mm;
//
    rminSup    = (*cage)[0]->getDouble("SUPRMIN")*Gaudi::Units::mm;
    supThick   = (*cage)[0]->getDouble("SUPTHICK")*Gaudi::Units::mm;
    supWid     = (*cage)[0]->getDouble("SUPWIDTH")*Gaudi::Units::mm;
//
    double zposSupRing  = zposFirstRing+ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
//
// Now support ribbon
    double ribWid     = 68.0*Gaudi::Units::mm ;
    ribWid = (*cage)[0]->getDouble("RIBWIDTH")*Gaudi::Units::mm;
    double ribLeng    = ringWid*5. + zposGap1 + zposGap2 + zposGap3 + zposGap4;
    double ribThick = 0; 
    if (sqversion >= 3) ribThick = (*cage)[0]->getDouble("RIBTHICK")*Gaudi::Units::mm;
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
    
    GeoTrf::Translate3D  servpos1(0.,0., zposFirstRing+ringWid/2.);
    GeoTrf::Translation3D  servpos2(0.,0.,-zposFirstRing-ringWid/2.);
    GeoTrf::Translate3D  servpos3(0.,0., zposFirstRing+ringWid*3./2. + zposGap1);
    GeoTrf::Translation3D  servpos4(0.,0.,-zposFirstRing-ringWid*3./2. - zposGap1);
    GeoTrf::Translate3D  servpos5(0.,0., zposFirstRing+ringWid*5./2. + zposGap1 + zposGap2);
    GeoTrf::Translation3D  servpos6(0.,0.,-zposFirstRing-ringWid*5./2. - zposGap1 - zposGap2);
    GeoTrf::Translate3D  servpos7(0.,0., zposFirstRing+ringWid*7./2. + zposGap1 + zposGap2 + zposGap3);
    GeoTrf::Translation3D  servpos8(0.,0.,-zposFirstRing-ringWid*7./2. - zposGap1 - zposGap2 - zposGap3);
    GeoTrf::Translate3D  servpos9(0.,0., zposFirstRing+ringWid*9./2. + zposGap1 + zposGap2 + zposGap3 + zposGap4);
    GeoTrf::Translation3D servpos10(0.,0.,-zposFirstRing-ringWid*9./2. - zposGap1 - zposGap2 - zposGap3 - zposGap4);
    GeoTransform *xform1  = new GeoTransform(servpos1);
    GeoTransform *xform2  = new GeoTransform(GeoTrf::Transform3D(servpos2*GeoTrf::RotateY3D(M_PI)));
    GeoTransform *xform3  = new GeoTransform(servpos3);
    GeoTransform *xform4  = new GeoTransform(GeoTrf::Transform3D(servpos4*GeoTrf::RotateY3D(M_PI)));
    GeoTransform *xform5  = new GeoTransform(servpos5);
    GeoTransform *xform6  = new GeoTransform(GeoTrf::Transform3D(servpos6*GeoTrf::RotateY3D(M_PI)));
    GeoTransform *xform7  = new GeoTransform(servpos7);
    GeoTransform *xform8  = new GeoTransform(GeoTrf::Transform3D(servpos8*GeoTrf::RotateY3D(M_PI)));
    GeoTransform *xform9  = new GeoTransform(servpos9);
    GeoTransform *xform10 = new GeoTransform(GeoTrf::Transform3D(servpos10*GeoTrf::RotateY3D(M_PI)));
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

    GeoTrf::Translate3D  suppos1(0.,0., zposSupRing+supWid/2.);
    GeoTrf::Translation3D  suppos2(0.,0.,-zposSupRing-supWid/2.);
    GeoTransform *sform1  = new GeoTransform(suppos1);
    GeoTransform *sform2  = new GeoTransform(GeoTrf::Transform3D(suppos2*GeoTrf::RotateY3D(M_PI)));
    mother->add(sform1);
    mother->add(ringPhysSup);
    mother->add(sform2);
    mother->add(ringPhysSup);

//Support ribbon positioning


    GeoTrf::Translate3D  ribpos1( 0., 0., zposFirstRing+ribLeng/2.);
    GeoTrf::Translate3D  ribpos2( 0., 0.,-zposFirstRing-ribLeng/2.);
    GeoTransform *rform1  = new GeoTransform(ribpos1);
    GeoTransform *rform2  = new GeoTransform(ribpos1*GeoTrf::RotateZ3D(M_PI));
    GeoTransform *rform3  = new GeoTransform(ribpos2);
    GeoTransform *rform4  = new GeoTransform(ribpos2*GeoTrf::RotateZ3D(M_PI));


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


