/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/PixelServMatFactoryDC2.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoTransform.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"

#define SKIPCYLINDER 3
#define NUMBEROFCYLINDER 7

PixelServMatFactoryDC2::PixelServMatFactoryDC2(StoreGateSvc *detStore,
					       ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_msg("PixelServmatFactoryDC2")
{
  
}


PixelServMatFactoryDC2::~PixelServMatFactoryDC2()
{

}



//## Other Operations (implementation)
void PixelServMatFactoryDC2::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building Pixel Service Material" << endreq;

  // Get the material manager:  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endreq;

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
  
 // Get the SvcLocator 
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;

  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");
 
  IRDBRecordset_ptr pbfi = m_rdbAccess->getRecordsetPtr("PBFISERV", indetVersionKey.tag(), indetVersionKey.node());
  

  for (int ii=0; ii<NUMBEROFCYLINDER; ii++) {
    if(ii == SKIPCYLINDER) ii++; // skip the pixel/SCT attachment

    std::string matName =  mat[(int) (*pbfi)[ii]->getFloat("MAT")];

    const GeoMaterial* cylMat = materialManager->getMaterial(matName);
    double rmin = (*pbfi)[ii]->getFloat("RIN")*CLHEP::cm;
    double rmax = (*pbfi)[ii]->getFloat("ROUT")*CLHEP::cm;
    double zmin = (*pbfi)[ii]->getFloat("ZIN");
    double zmax = (*pbfi)[ii]->getFloat("ZOUT");
    // elaborate it 'a la G3...
    double halflength = 0.;
    if(zmin*zmax < -0.00001) { // length in % of Rad Length
      double rl = cylMat->getRadLength();
      halflength = fabs(zmax) * rl /200. ;
    } else {
      halflength = fabs(zmax-zmin)*CLHEP::cm;
    }
    double zpos = fabs(zmin*CLHEP::cm)+halflength+epsilon;
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "PixServ"+o.str();    
    const GeoTube* serviceTube = new GeoTube(fabs(rmin),fabs(rmax),halflength);
    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    
    if(rmin > 0.)  // place two
      {
	CLHEP::Hep3Vector servpos1(0.,0.,zpos);
	CLHEP::Hep3Vector servpos2(0.,0.,-zpos);
	GeoTransform *xform1 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),servpos1));
	GeoTransform *xform2 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),servpos2));
	mother->add(xform1);
	mother->add(ServPhys);
	mother->add(xform2);
	mother->add(ServPhys);
      } else { // only one on the right side
	if(zmin < 0) zpos = -zpos; 
	CLHEP::Hep3Vector servpos1(0.,0.,zpos);
	GeoTransform *xform1 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),servpos1));
	mother->add(xform1);
	mother->add(ServPhys);
      }
  }

  
}


