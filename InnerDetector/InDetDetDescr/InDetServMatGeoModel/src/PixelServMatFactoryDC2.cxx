/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/PixelServMatFactoryDC2.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
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
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"

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

  msg(MSG::DEBUG) << "Building Pixel Service Material" << endmsg;

  // Get the material manager:  
  const StoredMaterialManager* materialManager;
  StatusCode sc = m_detStore->retrieve(materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;

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
  
  DecodeVersionKey indetVersionKey("InnerDetector");
 
  IRDBRecordset_ptr pbfi = m_rdbAccess->getRecordsetPtr("PBFISERV", indetVersionKey.tag(), indetVersionKey.node());
  

  for (int ii=0; ii<NUMBEROFCYLINDER; ii++) {
    if(ii == SKIPCYLINDER) ii++; // skip the pixel/SCT attachment

    std::string matName =  mat[(int) (*pbfi)[ii]->getFloat("MAT")];

    const GeoMaterial* cylMat = materialManager->getMaterial(matName);
    double rmin = (*pbfi)[ii]->getFloat("RIN")*Gaudi::Units::cm;
    double rmax = (*pbfi)[ii]->getFloat("ROUT")*Gaudi::Units::cm;
    double zmin = (*pbfi)[ii]->getFloat("ZIN");
    double zmax = (*pbfi)[ii]->getFloat("ZOUT");
    // elaborate it 'a la G3...
    double halflength = 0.;
    if(zmin*zmax < -0.00001) { // length in % of Rad Length
      double rl = cylMat->getRadLength();
      halflength = fabs(zmax) * rl /200. ;
    } else {
      halflength = fabs(zmax-zmin)*Gaudi::Units::cm;
    }
    double zpos = fabs(zmin*Gaudi::Units::cm)+halflength+epsilon;
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "PixServ"+o.str();    
    const GeoTube* serviceTube = new GeoTube(fabs(rmin),fabs(rmax),halflength);
    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    
    if(rmin > 0.)  // place two
      {
	GeoTrf::Translate3D servpos1(0.,0.,zpos);
	GeoTrf::Translate3D servpos2(0.,0.,-zpos);
	GeoTransform *xform1 = new GeoTransform(servpos1);
	GeoTransform *xform2 = new GeoTransform(servpos2);
	mother->add(xform1);
	mother->add(ServPhys);
	mother->add(xform2);
	mother->add(ServPhys);
      } else { // only one on the right side
	if(zmin < 0) zpos = -zpos; 
	GeoTrf::Translate3D servpos1(0.,0.,zpos);
	GeoTransform *xform1 = new GeoTransform(servpos1);
	mother->add(xform1);
	mother->add(ServPhys);
      }
  }

  
}


