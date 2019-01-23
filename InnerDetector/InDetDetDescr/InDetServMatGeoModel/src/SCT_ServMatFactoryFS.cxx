/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SCT_ServMatFactoryFS.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"      
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <iostream>

SCT_ServMatFactoryFS::SCT_ServMatFactoryFS(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc>& pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_materialManager(0),
  m_msg("SCT_ServMatFactoryFS")
{
}


SCT_ServMatFactoryFS::~SCT_ServMatFactoryFS()
{
  delete m_materialManager;
}



//## Other Operations (implementation)
void SCT_ServMatFactoryFS::create(GeoPhysVol *motherP,GeoPhysVol *motherM)
{

  msg(MSG::DEBUG) << "Building SCT Service Material" << endmsg;


  DecodeVersionKey atlasVersionKey("ATLAS");
  DecodeVersionKey indetVersionKey("InnerDetector");
  DecodeVersionKey sctVersionKey("SCT");

  IRDBRecordset_ptr atls   = m_rdbAccess->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr sctGenServices = m_rdbAccess->getRecordsetPtr("SCTGenServices",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr sctsup = m_rdbAccess->getRecordsetPtr("SCTSupport",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr weightTable = m_rdbAccess->getRecordsetPtr("SctWeights", sctVersionKey.tag(), sctVersionKey.node());
 
  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table.
  m_materialManager = new InDetMaterialManager("SCT_MaterialManager", m_detStore, weightTable, "sct");
  

  //------------------------------------------
  //VK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*Gaudi::Units::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*Gaudi::Units::cm;
  double minRofGap  =       1089.0*Gaudi::Units::mm;
  double phiWid=(70.*Gaudi::Units::mm)/outROfIDet;   
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

    const GeoTubs* tubeUp = 0;      //
    const GeoTubs* tubeDown = 0;    // Turn some booleans into single placements.
    const GeoTube* tube = 0;        // Shapes for such placements
    const GeoCons* cons = 0;        //

    const GeoPcon* servicePcon = 0; // helper shape 

    const GeoShape* serviceTube = serviceTubeTmp;
    if( tubeHelper.volData().maxRadius() > minRofGap && tubeHelper.volData().phiStart() < phiWid/2.)  {

      // GeoModel doesn't calculate volume correctly so we calculate the cutout volume.
      // This is only correct for tube volumes. The only non tube volume currently is the HSP
      // which is a radial volume. The cut out for this though is not very significant so we ignore it.
      if ( tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBE || tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBS) {

	// Build Up/Down shapes
	if(tubeHelper.volData().shape() ==  InDetDD::TubeVolData::TUBE) {
	  tubeUp   = new GeoTubs(tubeHelper.volData().rmin(),
				 tubeHelper.volData().maxRadius(),
				 0.5*tubeHelper.volData().length(),
				 phiWid*0.5,M_PI-phiWid);
	  tubeDown = new GeoTubs(tubeHelper.volData().rmin(),
				 tubeHelper.volData().maxRadius(),
				 0.5*tubeHelper.volData().length(),
				 M_PI+phiWid*0.5,M_PI-phiWid);
	} else {
	  tubeUp   = new GeoTubs(tubeHelper.volData().rmin(),
				 tubeHelper.volData().maxRadius(),
				 0.5*tubeHelper.volData().length(),
				 phiWid*0.5,
				 tubeHelper.volData().phiStart()+tubeHelper.volData().phiDelta()-phiWid*0.5);
	  tubeDown = new GeoTubs(tubeHelper.volData().rmin(),
				 tubeHelper.volData().maxRadius(),
				 0.5*tubeHelper.volData().length(),
				 tubeHelper.volData().phiStart(),
				 -phiWid*0.5-tubeHelper.volData().phiStart());
	}
 
	double rminCut = std::max(minRofGap, tubeHelper.volData().rmin());
	double rmaxCut = tubeHelper.volData().maxRadius();
	volumeCut = 0.5*phiWid*(rmaxCut+rminCut)*(rmaxCut-rminCut) * tubeHelper.volData().length();
	if( tubeHelper.volData().shape() == InDetDD::TubeVolData::TUBE) {volumeCut *= 2.;}
      } else {
	// Here we have SctHSP pcon
	// Break it into 4 individual placements: 1 tube, 1 cone and 2 tubs
	servicePcon = dynamic_cast<const GeoPcon*>(serviceTube);
	if(servicePcon==0 || servicePcon->getNPlanes()!=3) continue;

	// Shape 1. Tube. Rmax = Rmin of the cutout
	tube = new GeoTube(servicePcon->getRMinPlane(0),
			   minRofGap,
			   (servicePcon->getZPlane(1)-servicePcon->getZPlane(0))*0.5);

	// Shape 2. Cons component of the pcon
	cons = new GeoCons(servicePcon->getRMinPlane(1),servicePcon->getRMinPlane(2),
			   servicePcon->getRMaxPlane(1),servicePcon->getRMaxPlane(2)+1E-9*Gaudi::Units::mm,
			   (servicePcon->getZPlane(2)-servicePcon->getZPlane(1))*0.5,
			   0,2*M_PI);

	// Shape 3 & 4 similar to tubeUp and tubeDown
	tubeUp   = new GeoTubs(minRofGap,servicePcon->getRMaxPlane(0),
			       (servicePcon->getZPlane(1)-servicePcon->getZPlane(0))*0.5,
			       phiWid*0.5,M_PI-phiWid);

	tubeDown = new GeoTubs(minRofGap,servicePcon->getRMaxPlane(0),
			       (servicePcon->getZPlane(1)-servicePcon->getZPlane(0))*0.5,
			       M_PI+phiWid*0.5,M_PI-phiWid);
      }
    }    
    std::string materialName = tubeHelper.materialName();
    const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTubeTmp->volume()-volumeCut);
   
    if(tubeUp==0) {
      const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
      GeoVPhysVol* servPhys = new GeoPhysVol(servLog);
 
      tubeHelper.placeVolTwoSide(motherP, motherM, servPhys);
    } else {
      const GeoLogVol* servLogUp = new GeoLogVol(logName,tubeUp,material);
      const GeoLogVol* servLogDown = new GeoLogVol(logName,tubeDown,material);
      GeoPhysVol* servPhysUp = new GeoPhysVol(servLogUp);
      GeoPhysVol* servPhysDown = new GeoPhysVol(servLogDown);

      if(servicePcon) {
	const GeoLogVol* servLogTube = new GeoLogVol(logName,tube,material);
	const GeoLogVol* servLogCons = new GeoLogVol(logName,cons,material);
	GeoPhysVol* servPhysTube = new GeoPhysVol(servLogTube);
	GeoPhysVol* servPhysCons = new GeoPhysVol(servLogCons);

	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysTube,-(servicePcon->getZPlane(1)+servicePcon->getZPlane(0))*0.5);
	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysUp,-(servicePcon->getZPlane(1)+servicePcon->getZPlane(0))*0.5);
	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysDown,-(servicePcon->getZPlane(1)+servicePcon->getZPlane(0))*0.5);
	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysCons,-(servicePcon->getZPlane(2)+servicePcon->getZPlane(1))*0.5);
      } else {
	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysUp);
	tubeHelper.placeVolTwoSide(motherP,motherM,servPhysDown);
      }

      serviceTube->unref();
    }

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

      const GeoMaterial* wingMat  = m_materialManager->getMaterialForVolume(matName, wingVolume, newMatName);
            
      //std::cout<<logName<<", "<<matName<<", "<<wingMat->getName()<<'\n';
      
      const GeoLogVol* wingLog = new GeoLogVol(logName,wing,wingMat);
      GeoVPhysVol* wingPhys    = new GeoPhysVol(wingLog);
      GeoTransform* wPos1 = new GeoTransform(GeoTrf::Translate3D(0.,0., ZStartW+ZHalfLengthW));
      GeoTransform* wPos2 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(M_PI,0.,0.),GeoTrf::Vector3D(0.,0., ZStartW+ZHalfLengthW)));
      GeoTransform* wPos3 = new GeoTransform(GeoTrf::Translate3D(0.,0.,-ZStartW-ZHalfLengthW));
      GeoTransform* wPos4 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(M_PI,0.,0.),GeoTrf::Vector3D(0.,0.,-ZStartW-ZHalfLengthW)));
      motherP->add(wPos1);
      motherP->add(wingPhys);
      motherP->add(wPos2);
      motherP->add(wingPhys);
      motherM->add(wPos3);
      motherM->add(wingPhys);
      motherM->add(wPos4);
      motherM->add(wingPhys);
    }
  }
}

