/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/TRT_ServMatFactoryDC2.h"

// GeoModel includes
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"  
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

#define NUMBEROFPANEL 2
#define TRTELEMENTSINEL 9

#include <sstream>

TRT_ServMatFactoryDC2::TRT_ServMatFactoryDC2(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_materialManager(0),
  m_msg("ServMatFactoryDC2")
{
  
}


TRT_ServMatFactoryDC2::~TRT_ServMatFactoryDC2()
{

}



//## Other Operations (implementation)
void TRT_ServMatFactoryDC2::create(GeoPhysVol *mother)
{
  if (not mother){
    msg(MSG::FATAL) <<"GeoPhysVol pointer 'mother' is null"<<endmsg;
    return;
  
  }
  msg(MSG::DEBUG) << "Building TRT Service Material" << endmsg;


  // Get the material manager:  
  StatusCode sc = m_detStore->retrieve(m_materialManager, std::string("MATERIALS"));
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;
  
  double epsilon = 0.002;
  
  DecodeVersionKey indetVersionKey("InnerDetector");

  IRDBRecordset_ptr ipan = m_rdbAccess->getRecordsetPtr("IPAN", indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr inel = m_rdbAccess->getRecordsetPtr("INEL", indetVersionKey.tag(), indetVersionKey.node());


  // Build the patch panels:
  
  for (int ii=0; ii<NUMBEROFPANEL; ii++) {
    const GeoMaterial* cylMat = m_materialManager->getMaterial("trt::PatchOut");
    double rmin = (*ipan)[ii]->getFloat("RMIN")*Gaudi::Units::cm;
    double rmax = (*ipan)[ii]->getFloat("RMAX")*Gaudi::Units::cm;
    double zmin = (*ipan)[ii]->getFloat("ZMIN")*Gaudi::Units::cm;
    double zmax = (*ipan)[ii]->getFloat("ZMAX")*Gaudi::Units::cm;

    double halflength = (zmax-zmin)/2.-2*epsilon;
    double zpos = zmin + halflength+2*epsilon;
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "TrtPanel"+o.str();    
    const GeoTube* serviceTube = new GeoTube(fabs(rmin),fabs(rmax),halflength);
    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    // place two
    GeoTrf::Translate3D servpos1(0.,0.,zpos);
    GeoTrf::Translate3D servpos2(0.,0.,-zpos);
    GeoTransform *xform1 = new GeoTransform(servpos1);
    GeoTransform *xform2 = new GeoTransform(servpos2);
    mother->add(xform1);
    mother->add(ServPhys);
    mother->add(xform2);
    mother->add(ServPhys);
  }

  // Build the INEL part of the services:
  //
  for (int ii =0; ii < TRTELEMENTSINEL; ii++) {
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "TrtInel"+o.str();  
    double halflength = ((*inel)[ii]->getFloat("ZMAX")-(*inel)[ii]->getFloat("ZMIN"))/2.*Gaudi::Units::cm;
    int volType = (int) (*inel)[ii]->getFloat("VOLTYP");

    const GeoShape* serviceTube = createShape(volType,
					      (*inel)[ii]->getFloat("RMIN1")*Gaudi::Units::cm,
					      (*inel)[ii]->getFloat("RMAX1")*Gaudi::Units::cm,
					      halflength,
					      (*inel)[ii]->getFloat("RMIN2")*Gaudi::Units::cm,
					      (*inel)[ii]->getFloat("RMAX2")*Gaudi::Units::cm);
    // create the material...
    // In AGE the radiation length is specified and from that the density is
    // calculated assuming the material is C. I do the same here for now but
    // will eventually define all the materials.
    double fractionRL = (*inel)[ii]->getFloat("MATER");
    const GeoMaterial* cylMat;
    if (fractionRL > 0.999) { 
      // In this case a fixed material is used called Services which is the same
      // as trt::PatchOut
      cylMat = m_materialManager->getMaterial("trt::PatchOut");
    } else {
      std::ostringstream nameStr;
      nameStr << "TrtInelServices" << ii;
      cylMat = createMaterial(nameStr.str(),
			      volType,
			      fractionRL,
			      (*inel)[ii]->getFloat("RMIN1")*Gaudi::Units::cm,
			      (*inel)[ii]->getFloat("RMAX1")*Gaudi::Units::cm,
			      halflength,
			      (*inel)[ii]->getFloat("RMIN2")*Gaudi::Units::cm,
			      (*inel)[ii]->getFloat("RMAX2")*Gaudi::Units::cm); 
    }

    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    double zpos = ((*inel)[ii]->getFloat("ZMAX")+(*inel)[ii]->getFloat("ZMIN"))/2.*Gaudi::Units::cm+epsilon;
    // place two
    GeoTrf::Translate3D servpos1(0.,0.,zpos);
    GeoTrf::Translate3D servpos2(0.,0.,-zpos);
    GeoTransform *xform1 = new GeoTransform(servpos1);
    GeoTransform *xform2 = new GeoTransform(servpos2);
    mother->add(xform1);
    mother->add(ServPhys);
    mother->add(xform2);
    mother->add(ServPhys);
  }

  
}


const GeoShape* TRT_ServMatFactoryDC2::createShape(int volType, 
						double rmin1, 
						double rmax1, 
						double halflength,
						double rmin2=0.,
						double rmax2=0.) 
  
{
  const double epsilon = 0.001*Gaudi::Units::mm;
  enum VOLTYPE{Tube=1, Cone, ICone};
  const GeoShape* IDShape = 0;
  if(volType == Tube) {
    IDShape = new GeoTube(rmin1,rmax1,halflength);
  } 
  if(volType == Cone) {
    IDShape = new GeoCons(rmin1,rmin2-epsilon,rmax1,rmax2,halflength,0.,2*M_PI);
  }
  if(volType == ICone) {
    double zMid = halflength*(2*rmin1/rmax1-1.);
    GeoPcon* pPcon = new GeoPcon(0.,2*M_PI);
    pPcon->addPlane(-halflength, rmin1, rmax1);
    pPcon->addPlane(zMid, rmin1, rmax1);
    pPcon->addPlane(halflength, rmin1, rmin1);
    IDShape = pPcon;
  }
  return IDShape;
}

const GeoMaterial* TRT_ServMatFactoryDC2::createMaterial(const std::string & name,
						   int volType, 
						   double fractionRL,
						   double rmin1, 
						   double rmax1, 
						   double halflength,
						   double rmin2=0.,
						   double rmax2=0.) 
{
  double thickness = 0;
  enum VOLTYPE{Tube=1, Cone, ICone};
  if(volType == Tube) {
    thickness = std::min(2.*halflength, rmax1 - rmin1); 
  } 
  if(volType == Cone) {
    // There is a bug in AGE at this point, but the code is never called so it doesn't 
    // mater.
    double rmin = std::min(rmin1,rmin2);
    double rmax = std::max(rmax1,rmax2);
    thickness = std::min(2.*halflength, rmax - rmin); 
  }
  if(volType == ICone) {
    thickness = halflength*(rmin1/rmax1 + 1.);
  }
  
  // Make new material as carbon with density modified to give the right radiation length.

  const GeoMaterial* carbonMat= m_materialManager->getMaterial("std::Carbon");
  double radiationLength = thickness / fractionRL;
  double density = carbonMat->getDensity() * carbonMat->getRadLength() / radiationLength;

  GeoMaterial * newMaterial = new GeoMaterial(name, density);
  const GeoElement * carbon = m_materialManager->getElement("Carbon");
  newMaterial->add(const_cast<GeoElement *>(carbon),1.);
  newMaterial->lock();

  return newMaterial;
}

  
