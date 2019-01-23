/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SCT_ServMatFactory.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <iostream>

SCT_ServMatFactory::SCT_ServMatFactory(const InDetDD::AthenaComps * athenaComps)
  : InDetDD::SubDetectorFactoryBase(athenaComps)
{
}

SCT_ServMatFactory::~SCT_ServMatFactory()
{
  // It owns the material manager
  delete m_materialManager;
}



//## Other Operations (implementation)
void SCT_ServMatFactory::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building SCT Service Material" << endmsg;

  DecodeVersionKey atlasVersionKey(geoDbTagSvc(),"ATLAS");
  DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
  DecodeVersionKey sctVersionKey(geoDbTagSvc(),"SCT");

  std::string railversion =  rdbAccessSvc()->getChildTag("IDDetailedRail",indetVersionKey.tag(),indetVersionKey.node());
  if(!railversion.empty()) {
    // ______________________________________________________________________________________________________________________
    // Geometry with ID rail

  IRDBRecordset_ptr atls   = rdbAccessSvc()->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr sctGenServices = rdbAccessSvc()->getRecordsetPtr("SCTGenServices",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr sctsup = rdbAccessSvc()->getRecordsetPtr("SCTSupport",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr weightTable = rdbAccessSvc()->getRecordsetPtr("SctWeights", sctVersionKey.tag(), sctVersionKey.node());
  IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("InDetServMatScaling", indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr cage = rdbAccessSvc()->getRecordsetPtr("SquirrelCage", atlasVersionKey.tag(), atlasVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table.
  m_materialManager = new InDetMaterialManager("SCT_MaterialManager", getAthenaComps());
  m_materialManager->addWeightTable(weightTable, "sct");
  m_materialManager->addScalingTable(scalingTable);

  //------------------------------------------
  //VK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;
  //  double minRofGap  =       1050.0*Gaudi::Units::mm;
  //  double minRofGap  =       1110.0*Gaudi::Units::mm;
  double minRofGap  =       1089.0*Gaudi::Units::mm;
  double phiWid=(70.*Gaudi::Units::mm)/outROfIDet;   
  //  std::cout << "Gap phiWid = " << phiWid << std::endl;
  double safetyGap=1.*Gaudi::Units::mm;

  //created by Adam Agocs 

  double rminInt = (*cage)[0]->getDouble("RINGRMIN");

  IRDBRecordset_ptr commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon",indetVersionKey.tag(), indetVersionKey.node());
  if(commonParameters->size()==0)
    commonParameters = rdbAccessSvc()->getRecordsetPtr("IDDetRailCommon","IDDetRailCommon-00");

  double yWidthUSP1    = (*commonParameters)[0]->getDouble("YWIDTHUSP1");
  double yRailSup      = (*commonParameters)[0]->getDouble("YRAILSUP");
  double yRailSupPart3 = (*commonParameters)[0]->getDouble("YRAILSUPPART3");
  double gapOfRSF      = (*commonParameters)[0]->getDouble("GAPOFRSF");
  double yWidthTRSB    = (*commonParameters)[0]->getDouble("YWIDTHTRSB");

  double coordY = yWidthUSP1/2. - yRailSup + (yRailSupPart3 - gapOfRSF/2.);

  double phiTop = asin((yWidthTRSB + coordY) / (rminInt));
  double phiBot = asin((yWidthTRSB/2. - coordY) / (rminInt));

  const GeoShape* railGap1=new GeoTubs( rminInt, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
					-phiBot, phiBot + phiTop);
  const GeoShape* railGap2=new GeoTubs( rminInt, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
					-phiTop + M_PI, phiBot + phiTop);
  const GeoShape* railGap12=new GeoTubs( rminInt, outROfIDet+safetyGap ,endZOfIDet+safetyGap ,
					-phiTop, phiBot + phiTop); //because of asymmetry
  // In case they don't get used. 
  railGap1->ref(); 
  railGap2->ref(); 
  railGap12->ref();   
 
  // Build general services:
  //
  std::string NameOfService;
  for (unsigned int ii =0; ii < sctGenServices->size(); ii++) {

    NameOfService = (*sctGenServices)[ii]->getString("NAME");
    InDetDD::GenericTubeMaker tubeHelper((*sctGenServices)[ii]);
    
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    } 
    logName = "Sct" + logName;

    double volumeCut = 0;
    const GeoShape* serviceTube = serviceTubeTmp;
    const GeoShape* serviceTube2 = serviceTubeTmp; //because of asymmetry
    if( tubeHelper.volData().maxRadius() > minRofGap && tubeHelper.volData().phiStart()*Gaudi::Units::radian < phiTop)  {
      // Subtract RailGap out of services
      if (NameOfService == "PPB1EFEG" || NameOfService == "CableTrayEFEG")
      {
       serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap1);
       serviceTube2 = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap12);
      }
      else
       serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap2).subtract(*railGap1);

      // GeoModel doesn't calculate volume correctly so we calculate the cutout volume.
      // This is only correct for tube volumes. The only non tube volume currently is the HSP
      // which is a radial volume. The cut out for this though is not very significant so we ignore it.
      if ( tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBE || tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBS) {
	double rminCut = std::max(minRofGap, tubeHelper.volData().rmin());
	double rmaxCut = tubeHelper.volData().maxRadius();
	volumeCut = 0.5*phiWid*(rmaxCut+rminCut)*(rmaxCut-rminCut) * tubeHelper.volData().length();
	if( tubeHelper.volData().shape() == InDetDD::TubeVolData::TUBE) {volumeCut *= 2.;}
      }
    }    

    std::string materialName = tubeHelper.materialName();
    //    std::cout<< "logName " << logName << std::endl;
    //    std::cout << " serviceTube->volume()    " << serviceTube->volume() << std::endl;
    //    std::cout << " serviceTubeTmp->volume() " << serviceTubeTmp->volume() << std::endl;
    //    std::cout << " serviceTubeTmp->volume() - volumeCut = " << serviceTubeTmp->volume() - volumeCut << std::endl;
    //const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTubeTmp->volume()-volumeCut);
   
    const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
    GeoVPhysVol* servPhys = new GeoPhysVol(servLog);

    if (NameOfService == "PPB1EFEG" || 	NameOfService == "CableTrayEFEG")
    {
     const GeoLogVol* serv2Log = new GeoLogVol(logName,serviceTube2,material);
     GeoVPhysVol* serv2Phys = new GeoPhysVol(serv2Log);

     GeoTrf::TranslateZ3D trans(tubeHelper.volData().zMid());
     GeoTrf::TranslateZ3D trans2(-tubeHelper.volData().zMid());

     GeoTransform * xform1    = new GeoTransform(trans);
     GeoTransform * xform1Neg = new GeoTransform(trans2);
     GeoTransform * xform2    = new GeoTransform(GeoTrf::RotateZ3D(180*Gaudi::Units::deg)*trans);
     GeoTransform * xform2Neg = new GeoTransform(GeoTrf::RotateZ3D(180*Gaudi::Units::deg)*trans2);
    
//     std::cerr << xform1 << std::endl << xform1Neg << std::endl << xform2 << std::endl << xform2Neg << std::endl;
     
     mother->add(xform1); mother->add(servPhys);  mother->add(xform1Neg); mother->add(servPhys);
     mother->add(xform2); mother->add(serv2Phys); mother->add(xform2Neg); mother->add(serv2Phys);
    }
    else
     tubeHelper.placeVolume(mother, servPhys);
    
  }
  
  railGap1->unref(); 
  railGap2->unref(); 
  railGap12->unref(); 

  //-------------------------------------------------------------------------------------------
  // SCT supports (wings,mechanisms)

  // Variable: NameOfMaterial is created by Adam Agocs
  // if (NameOfMaterial != "sct::FwdMechanism") statment is creadted by Adam Agocs to
  // avoid overlapping with "U" shape support - SupportRailFactory.cxx
  {
    double RMinW, RMaxW, ZHalfLengthW, ZStartW, WidI, WidO, DPhi;  
    std::string matName,logName;

    std::string NameOfMaterial;

    for (unsigned int ii =0; ii < sctsup->size(); ii++) {
      
      RMinW          = (*sctsup)[ii]->getFloat("RMIN")*Gaudi::Units::mm;
      RMaxW          = (*sctsup)[ii]->getFloat("RMAX")*Gaudi::Units::mm;
      ZHalfLengthW   = (*sctsup)[ii]->getFloat("THICK")/2.*Gaudi::Units::mm;
      WidI           = (*sctsup)[ii]->getFloat("WIDTHINNER")*Gaudi::Units::mm;
      WidO           = (*sctsup)[ii]->getFloat("WIDTHOUTER")*Gaudi::Units::mm;
      ZStartW        = (*sctsup)[ii]->getFloat("ZSTART")*Gaudi::Units::mm;
      NameOfMaterial = (*sctsup)[ii]->getString("MATERIAL");
      DPhi = asin(WidI/2./RMinW);
      
      if (NameOfMaterial != "sct::FwdMechanism")
      {
       const GeoShape* pTub1 = new GeoTubs(RMinW, RMaxW, ZHalfLengthW, 0.-DPhi, 2.*DPhi);  //Basic shape
       const GeoShape* wing = pTub1;

       double wingVolume = -1;
       if (WidI > WidO) {                                                 // Subtraction for wings
	double H1=sqrt(RMinW*RMinW - WidI*WidI/4.);
	double H2=sqrt(RMaxW*RMaxW - WidO*WidO/4.);
	double Shift = H1 + (H2-H1)*WidI/(WidI-WidO);
	double RMaxDop = sqrt(WidI*WidI/4. + (Shift-H1)*(Shift-H1));
	//	double DPhiDop = asin(WidI/2./(Shift-H1));
	double DPhiDop = atan(WidI/2./(Shift-H1));
	const GeoShape* pTub2 = new GeoTubs(0., RMaxDop, ZHalfLengthW, M_PI-DPhiDop, 2.*DPhiDop);
	const GeoShape* pTub3 = (GeoShape*) & ((*pTub2) << GeoTrf::TranslateX3D(Shift));
	wing  = (GeoShape*) & (*pTub1).intersect(*pTub3);  
	
	// GeoModel calculates the volume incorrectly so we calculate it here.
	// Volume can be calculated as a trapezoid with an upper circle segment added and a lower one subtracted
	double trapArea = 0.5 * (H2-H1)*(WidI+WidO);
	double upperSeg = atan(0.5*WidO/H2)*RMaxW*RMaxW - 0.5*H2*WidO;
	double lowerSeg = atan(0.5*WidI/H1)*RMinW*RMinW - 0.5*H1*WidI;
	wingVolume = (trapArea + upperSeg - lowerSeg) * 2 * ZHalfLengthW;
	//	std::cout << "New wing volume = " << wingVolume << std::endl;
      }
      
      matName = (*sctsup)[ii]->getString("MATERIAL");
      logName = "SctSupportGeneral";
      std::string newMatName; // If newMatName is empty, will not rename.
      std::ostringstream o; o << ii;
      if(matName.find("rWing") != std::string::npos) logName="FwdRearWing";
      if(matName.find("tWing") != std::string::npos) logName="FwdFrontWing";
      if(matName.find("Mech")  != std::string::npos) {
	logName="FwdMechanism"+o.str(); 
	newMatName = matName + o.str(); // There are two FwdMechanism volumes with different volumes so we need to rename material.
      }	
      
      if (wingVolume < 0) wingVolume = wing->volume(); 
      //std::cout << " wing->volume()    " << wing->volume() << std::endl;
      //std::cout << " New wing volume = " << wingVolume << std::endl;

      const GeoMaterial* wingMat  = m_materialManager->getMaterialForVolume(matName, wingVolume, newMatName);
            
      //std::cout<<logName<<", "<<matName<<", "<<wingMat->getName()<<'\n';
      
      const GeoLogVol* wingLog = new GeoLogVol(logName,wing,wingMat);
      GeoVPhysVol* wingPhys    = new GeoPhysVol(wingLog);
      GeoTransform* wPos1 = new GeoTransform(GeoTrf::Translate3D(0.,0., ZStartW+ZHalfLengthW));
      GeoTransform* wPos2 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(M_PI,0,0),GeoTrf::Vector3D(0.,0., ZStartW+ZHalfLengthW)));
      GeoTransform* wPos3 = new GeoTransform(GeoTrf::Translate3D(0.,0.,-ZStartW-ZHalfLengthW));
      GeoTransform* wPos4 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(M_PI,0,0),GeoTrf::Vector3D(0.,0.,-ZStartW-ZHalfLengthW)));
      mother->add(wPos1);
      mother->add(wingPhys);
      mother->add(wPos2);
      mother->add(wingPhys);
      mother->add(wPos3);
      mother->add(wingPhys);
      mother->add(wPos4);
      mother->add(wingPhys);
    }
   }
  }
  }
  else {
    // ______________________________________________________________________________________________________________________
    // Geometry without ID rail
  IRDBRecordset_ptr atls   = rdbAccessSvc()->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr sctGenServices = rdbAccessSvc()->getRecordsetPtr("SCTGenServices",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr sctsup = rdbAccessSvc()->getRecordsetPtr("SCTSupport",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr weightTable = rdbAccessSvc()->getRecordsetPtr("SctWeights", sctVersionKey.tag(), sctVersionKey.node());
  IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("InDetServMatScaling", indetVersionKey.tag(), indetVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table.
  m_materialManager = new InDetMaterialManager("SCT_MaterialManager", getAthenaComps());
  m_materialManager->addWeightTable(weightTable, "sct");
  m_materialManager->addScalingTable(scalingTable);

  //------------------------------------------
  //VK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;
  //  double minRofGap  =       1050.0*Gaudi::Units::mm;
  //  double minRofGap  =       1110.0*Gaudi::Units::mm;
  double minRofGap  =       1089.0*Gaudi::Units::mm;
  double phiWid=(70.*Gaudi::Units::mm)/outROfIDet;   
  //  std::cout << "Gap phiWid = " << phiWid << std::endl;
  double safetyGap=1.*Gaudi::Units::mm;
  const GeoShape* railGap1=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
					-phiWid/2.,phiWid);
  const GeoShape* railGap2=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap ,
					-phiWid/2.+M_PI,phiWid);
  // In case they don't get used. 
  railGap1->ref(); 
  railGap2->ref(); 

 
  // Build general services:
  //
  for (unsigned int ii =0; ii < sctGenServices->size(); ii++) {

    InDetDD::GenericTubeMaker tubeHelper((*sctGenServices)[ii]);
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    } 
    logName = "Sct" + logName;

    double volumeCut = 0;
    const GeoShape* serviceTube = serviceTubeTmp;
    if( tubeHelper.volData().maxRadius() > minRofGap && tubeHelper.volData().phiStart() < phiWid/2.)  {
      // Subtract RailGap out of services
      serviceTube  = (GeoShape*) & (*serviceTubeTmp).subtract(*railGap2).subtract(*railGap1);

      // GeoModel doesn't calculate volume correctly so we calculate the cutout volume.
      // This is only correct for tube volumes. The only non tube volume currently is the HSP
      // which is a radial volume. The cut out for this though is not very significant so we ignore it.
      if ( tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBE || tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBS) {
	double rminCut = std::max(minRofGap, tubeHelper.volData().rmin());
	double rmaxCut = tubeHelper.volData().maxRadius();
	volumeCut = 0.5*phiWid*(rmaxCut+rminCut)*(rmaxCut-rminCut) * tubeHelper.volData().length();
	if( tubeHelper.volData().shape() == InDetDD::TubeVolData::TUBE) {volumeCut *= 2.;}
      }
    }    

    std::string materialName = tubeHelper.materialName();
    //    std::cout<< "logName " << logName << std::endl;
    //    std::cout << " serviceTube->volume()    " << serviceTube->volume() << std::endl;
    //    std::cout << " serviceTubeTmp->volume() " << serviceTubeTmp->volume() << std::endl;
    //    std::cout << " serviceTubeTmp->volume() - volumeCut = " << serviceTubeTmp->volume() - volumeCut << std::endl;
    //const GeoMaterial* material = materialManager()->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = materialManager()->getMaterialForVolume(materialName, serviceTubeTmp->volume()-volumeCut);
   
    const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
    GeoVPhysVol* servPhys = new GeoPhysVol(servLog);
 
    tubeHelper.placeVolume(mother, servPhys);

  }
  
  railGap1->unref(); 
  railGap2->unref(); 

  //-------------------------------------------------------------------------------------------
  // SCT supports (wings,mechanisms)
  {
    double RMinW, RMaxW, ZHalfLengthW, ZStartW, WidI, WidO, DPhi;  
    std::string matName,logName;

    for (unsigned int ii =0; ii < sctsup->size(); ii++) {
      
      RMinW        = (*sctsup)[ii]->getFloat("RMIN")*Gaudi::Units::mm;
      RMaxW        = (*sctsup)[ii]->getFloat("RMAX")*Gaudi::Units::mm;
      ZHalfLengthW = (*sctsup)[ii]->getFloat("THICK")/2.*Gaudi::Units::mm;
      WidI         = (*sctsup)[ii]->getFloat("WIDTHINNER")*Gaudi::Units::mm;
      WidO         = (*sctsup)[ii]->getFloat("WIDTHOUTER")*Gaudi::Units::mm;
      ZStartW      = (*sctsup)[ii]->getFloat("ZSTART")*Gaudi::Units::mm;
      DPhi = asin(WidI/2./RMinW);
      
      const GeoShape* pTub1 = new GeoTubs(RMinW, RMaxW, ZHalfLengthW, 0.-DPhi, 2.*DPhi);  //Basic shape
      const GeoShape* wing = pTub1;

      double wingVolume = -1;
      if (WidI > WidO) {                                                 // Subtraction for wings
	double H1=sqrt(RMinW*RMinW - WidI*WidI/4.);
	double H2=sqrt(RMaxW*RMaxW - WidO*WidO/4.);
	double Shift = H1 + (H2-H1)*WidI/(WidI-WidO);
	double RMaxDop = sqrt(WidI*WidI/4. + (Shift-H1)*(Shift-H1));
	//	double DPhiDop = asin(WidI/2./(Shift-H1));
	double DPhiDop = atan(WidI/2./(Shift-H1));
	const GeoShape* pTub2 = new GeoTubs(0., RMaxDop, ZHalfLengthW, M_PI-DPhiDop, 2.*DPhiDop);
	const GeoShape* pTub3 = (GeoShape*) & ((*pTub2) << GeoTrf::TranslateX3D(Shift));
	wing  = (GeoShape*) & (*pTub1).intersect(*pTub3);  
	
	// GeoModel calculates the volume incorrectly so we calculate it here.
	// Volume can be calculated as a trapezoid with an upper circle segment added and a lower one subtracted
	double trapArea = 0.5 * (H2-H1)*(WidI+WidO);
	double upperSeg = atan(0.5*WidO/H2)*RMaxW*RMaxW - 0.5*H2*WidO;
	double lowerSeg = atan(0.5*WidI/H1)*RMinW*RMinW - 0.5*H1*WidI;
	wingVolume = (trapArea + upperSeg - lowerSeg) * 2 * ZHalfLengthW;
	//	std::cout << "New wing volume = " << wingVolume << std::endl;
      }
      
      matName = (*sctsup)[ii]->getString("MATERIAL");
      logName = "SctSupportGeneral";
      std::string newMatName; // If newMatName is empty, will not rename.
      std::ostringstream o; o << ii;
      if(matName.find("rWing") != std::string::npos) logName="FwdRearWing";
      if(matName.find("tWing") != std::string::npos) logName="FwdFrontWing";
      if(matName.find("Mech")  != std::string::npos) {
	logName="FwdMechanism"+o.str(); 
	newMatName = matName + o.str(); // There are two FwdMechanism volumes with different volumes so we need to rename material.
      }	
      
      if (wingVolume < 0) wingVolume = wing->volume(); 
      //std::cout << " wing->volume()    " << wing->volume() << std::endl;
      //std::cout << " New wing volume = " << wingVolume << std::endl;

      const GeoMaterial* wingMat  = materialManager()->getMaterialForVolume(matName, wingVolume, newMatName);
            
      //std::cout<<logName<<", "<<matName<<", "<<wingMat->getName()<<'\n';
      
      const GeoLogVol* wingLog = new GeoLogVol(logName,wing,wingMat);
      GeoVPhysVol* wingPhys    = new GeoPhysVol(wingLog);
      GeoTransform* wPos1 = new GeoTransform(GeoTrf::Translate3D(0.,0., ZStartW+ZHalfLengthW));
      GeoTransform* wPos2 = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translation3D(0.,0., ZStartW+ZHalfLengthW)*GeoTrf::RotateX3D(M_PI)));
      GeoTransform* wPos3 = new GeoTransform(GeoTrf::Translate3D(0.,0.,-ZStartW-ZHalfLengthW));
      GeoTransform* wPos4 = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translation3D(0.,0.,-ZStartW-ZHalfLengthW)*GeoTrf::RotateX3D(M_PI)));
      mother->add(wPos1);
      mother->add(wingPhys);
      mother->add(wPos2);
      mother->add(wingPhys);
      mother->add(wPos3);
      mother->add(wingPhys);
      mother->add(wPos4);
      mother->add(wingPhys);
    }
  }
  }
}
