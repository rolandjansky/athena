/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/PixelServMatFactoryDC3.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <iostream>

#define SKIPCYLINDER 3
//#define NUMBEROFCYLINDER 7

PixelServMatFactoryDC3::PixelServMatFactoryDC3(const InDetDD::AthenaComps * athenaComps, 
					       InDetMaterialManager * matManager)
  : InDetDD::SubDetectorFactoryBase(athenaComps, matManager)
{  
}


PixelServMatFactoryDC3::~PixelServMatFactoryDC3()
{

}


//## Other Operations (implementation)
void PixelServMatFactoryDC3::create(GeoPhysVol *mother)
{
  msg(MSG::DEBUG) << "Building Pixel Service Material" << endmsg;

  // And the list of materials that we are using:
    std::string mat[6] = {
      "None",
      "std::Beryllium",
      "std::Carbon",
      "pix::MatSrv",
      "pix::MatPP18", //MatPP18 is the same as MatPP17...
      "pix::MatPP19"
    };
    double epsilon = 0.002;
  
    DecodeVersionKey indetVersionKey(geoDbTagSvc(),"InnerDetector");
 
  IRDBRecordset_ptr pbfi = rdbAccessSvc()->getRecordsetPtr("PBFISERV", indetVersionKey.tag(), indetVersionKey.node());

//Test   
/*  const GeoMaterial* testMat = materialManager->getMaterial("std::Aluminium");
  std::cout << "Test Material std::Aluminium density="<<testMat->getDensity()
      <<" Rad.length="<<testMat->getRadLength()<<" Int.length="<<testMat->getIntLength()<<'\n';
  testMat = materialManager->getMaterial("std::G10");
  std::cout << "Test Material std::G10 density="<<testMat->getDensity()
      <<" Rad.length="<<testMat->getRadLength()<<" Int.length="<<testMat->getIntLength()<<'\n';
  testMat = materialManager->getMaterial("std::Carbon");
  std::cout << "Test Material std::Carbon density="<<testMat->getDensity()
      <<" Rad.length="<<testMat->getRadLength()<<" Int.length="<<testMat->getIntLength()<<'\n';
  testMat = materialManager->getMaterial("std::Copper");
  std::cout << "Test Material std::Copper density="<<testMat->getDensity()
      <<" Rad.length="<<testMat->getRadLength()<<" Int.length="<<testMat->getIntLength()<<'\n';

      GeoMaterial* TIN = new GeoMaterial("Sn", 7.31*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
//        GeoElement *testMat   = new GeoElement("Tin",  "Sn", 50.0, 118.69*Gaudi::Units::amu_c2);
      const GeoElement *tin   = materialManager->getElement("Tin");
      TIN->add(const_cast<GeoElement *>(tin),1.);
      TIN->lock(); testMat=TIN;
  std::cout << "Test Material Tin density="<<testMat->getDensity()
      <<" Rad.length="<<testMat->getRadLength()<<" Int.length="<<testMat->getIntLength()<<'\n';
  std::cout << "Atomic mass unit="<<Gaudi::Units::amu_c2<<'\n';
  std::cout << "gram/cm3 ="<<gram/cm3<<'\n';
*/
 
  int NUMBEROFCYLINDER = pbfi->size(); 
//for (int ii=0; ii<NUMBEROFCYLINDER; ii++) {
  for (int jj=0; jj<NUMBEROFCYLINDER; jj++) {                //VK  user cylinder number to allow an arbitrary order
    int ii =  (int) (*pbfi)[jj]->getFloat("CYLINDER") -1 ;   //VK  of records in ORACLE table 

    if(ii == SKIPCYLINDER) continue;       // skip the pixel/SCT attachment
 
    std::string matName =  mat[(int) (*pbfi)[jj]->getFloat("MAT")];
    const GeoMaterial* cylMat = materialManager()->getMaterial(matName);

//  log << MSG::DEBUG << "Pixel Service Material=" <<matName<<" Density="<<cylMat->getDensity()
//      <<" Rad.length="<<cylMat->getRadLength()<<endmsg;
//  std::cout << "Pixel Service Material=" <<matName<<" Density="<<cylMat->getDensity()
//      <<" Rad.length="<<cylMat->getRadLength()<<'\n';


    double rmin = (*pbfi)[jj]->getFloat("RIN")*Gaudi::Units::cm;
    double rmax = (*pbfi)[jj]->getFloat("ROUT")*Gaudi::Units::cm;
    double zmin = (*pbfi)[jj]->getFloat("ZIN");
    double zmax = (*pbfi)[jj]->getFloat("ZOUT");

    if( rmin < 0 )   continue; // REMOVE non symmetrical service


    // elaborate it 'a la G3...
    double halflength = 0.;
    if(zmin*zmax < -0.00001) { // length in % of Rad Length
      double rl = cylMat->getRadLength();
      halflength = fabs(zmax) * rl /200. ;
    } else {
      halflength = fabs(zmax-zmin)*Gaudi::Units::cm;
    }

//VK Temporary!!!  To bring thickness to nominal values
//std::cout << "Old="<<halflength<<", "<<zmin<<", "<<ii<<'\n';
//    if( ii == 0 ) halflength *= 105.*1.5;
//    if( ii == 2 ) halflength *= 12.*0.7;
//    if( ii == 0 ) zmin += 0.7;   // in cm!
//std::cout << "New="<<halflength<<", "<<zmin<<", "<<ii<<'\n';

    double zpos = fabs(zmin*Gaudi::Units::cm)+halflength+epsilon;
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "PixServ"+o.str();    

    const GeoShape* serviceTube   = 0;
    if (ii==0 || ii==2 || ii==6 || ii==1){
       double angGapWid    = M_PI/40.;
       double angSectorWid = M_PI/6.;
       double rint = (fabs(rmax)+fabs(rmin))/2.;
       GeoPcon* serviceSector = new GeoPcon(angGapWid/2., angSectorWid-angGapWid);
//  To simulate a density change with radius a trapezoidal shape is used
//  GWG: I think the third one should be rmin/rint rather than rint/rmax but we leave it as this file is no longer used in newer geometries.
       serviceSector->addPlane(-halflength                               ,fabs(rmin), fabs(rmax));
       serviceSector->addPlane(-halflength+2.*halflength*fabs(rmin/rmax) ,fabs(rmin), fabs(rmax));
       serviceSector->addPlane(-halflength+2.*halflength*fabs(rint/rmax) ,fabs(rmin),      rint );
       serviceSector->addPlane(-halflength+2.*halflength                 ,fabs(rmin), fabs(rmin));
       serviceTube = serviceSector;
    }else{
       serviceTube = new GeoTube(fabs(rmin),fabs(rmax),halflength);
    }
    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoPhysVol* ServPhys = new GeoPhysVol(ServLog);
    
    if(zmin < 0) zpos = -zpos; 
    GeoTrf::Vector3D servpos1(0.,0.,zpos);
    GeoTrf::Vector3D servpos2(0.,0.,-zpos);
//
    if (ii==0 || ii==2 || ii==6 || ii==1){
	for (int isec=0; isec<12 ; isec++){
	    std::ostringstream o;
            o<<"_";  o << isec;
            std::string logNameTmp = logName+o.str();    
//std::cout<<isec<<", "<<logNameTmp<<'\n';
	    GeoTransform *xform1 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,0.,isec*M_PI/6.),servpos1));
	    GeoTransform *xform2 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0.,0.,isec*M_PI/6.),servpos2));
	    xform2->ref();//artificial refcount increment
	    mother->add(new GeoNameTag(logNameTmp));
	    mother->add(xform1);//xform1 is always used
	    mother->add(ServPhys);
	    if( rmin > 0.){
	       mother->add(new GeoNameTag(logNameTmp));
	       mother->add(xform2);
	       mother->add(ServPhys);
	    }
	    xform2->unref(); //will delete it, if it was never used
        }
    }else{
        GeoTransform *xform1 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0,0,0),servpos1));
	GeoTransform *xform2 = new GeoTransform(GeoTrf::GeoTransformRT(GeoTrf::GeoRotation(0,0,0),servpos2));
	xform2->ref();//artificial refcount increment
	mother->add(xform1);
	mother->add(ServPhys);
	if( rmin > 0.){
	  mother->add(xform2);
	  mother->add(ServPhys);
	}
	xform2->unref(); //will delete it, if it was never used

    }
      
  }

  
}


