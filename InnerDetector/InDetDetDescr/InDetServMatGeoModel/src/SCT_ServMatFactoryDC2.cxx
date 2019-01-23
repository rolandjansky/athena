/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SCT_ServMatFactoryDC2.h"

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
#include "GeoModelKernel/GeoElement.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelKernel/Units.h"
#include "GaudiKernel/PhysicalConstants.h"
#include <stdexcept>
#define TRTELEMENTSINEL 9
#define SCTELEMENTSINEL 8

#include <sstream>

SCT_ServMatFactoryDC2::SCT_ServMatFactoryDC2(StoreGateSvc *detStore,ServiceHandle<IRDBAccessSvc>& pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_materialManager(0),
  m_msg("SCT_ServMatFactoryDC2")
{
  
}


SCT_ServMatFactoryDC2::~SCT_ServMatFactoryDC2()
{

}



//## Other Operations (implementation)
void SCT_ServMatFactoryDC2::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building SCT Service Material" << endmsg;

  // Get the material manager:  
  StatusCode sc = m_detStore->retrieve(m_materialManager, std::string("MATERIALS"));
  if (sc.isFailure() or not m_materialManager){
    msg(MSG::FATAL) << "Could not locate Material Manager" << endmsg;
    throw std::runtime_error("SCT_ServMatFactoryDC2::create() cannot retrieve the material manager and must abort");
  }
  
  double epsilon = 0.002;


  DecodeVersionKey sctVersionKey("SCT");
  DecodeVersionKey trtVersionKey("TRT");
  DecodeVersionKey indetVersionKey("InnerDetector");

  IRDBRecordset_ptr inel = m_rdbAccess->getRecordsetPtr("INEL",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr tsci = m_rdbAccess->getRecordsetPtr("TSCI", trtVersionKey.tag(), trtVersionKey.node());
  IRDBRecordset_ptr zscg = m_rdbAccess->getRecordsetPtr("ZSCG", sctVersionKey.tag(), sctVersionKey.node());

  
  // Build SCT services in Endcap.
  // (Code taken from TRT_GeoModel)

  double innerRadiusOfSCTSupport = (*tsci)[0]->getDouble("RMIN")*Gaudi::Units::cm + 2*epsilon;
  double outerRadiusOfSCTSupport = (*tsci)[0]->getDouble("RMAX")*Gaudi::Units::cm;
  double lengthOfSCTSupport = ((*tsci)[0]->getDouble("ZMAX")-(*tsci)[0]->getDouble("ZMIN"))*Gaudi::Units::cm - epsilon;
  double positionOfSCTSupport= 0.5 * ((*tsci)[0]->getDouble("ZMAX")+(*tsci)[0]->getDouble("ZMIN"))*Gaudi::Units::cm;

  double innerRadiusOfSCTCables = (*tsci)[1]->getDouble("RMIN")*Gaudi::Units::cm + 2*epsilon;
  double outerRadiusOfSCTCables = (*tsci)[1]->getDouble("RMAX")*Gaudi::Units::cm;
  double lengthOfSCTCables = ((*tsci)[1]->getDouble("ZMAX")-(*tsci)[1]->getDouble("ZMIN"))*Gaudi::Units::cm - epsilon;
  double positionOfSCTCables = 0.5 * ((*tsci)[1]->getDouble("ZMAX")+(*tsci)[1]->getDouble("ZMIN"))*Gaudi::Units::cm;
 
  double innerRadiusOfSCTCooling = (*tsci)[2]->getDouble("RMIN")*Gaudi::Units::cm + 2*epsilon;
  double outerRadiusOfSCTCooling = (*tsci)[2]->getDouble("RMAX")*Gaudi::Units::cm;
  double lengthOfSCTCooling = ((*tsci)[2]->getDouble("ZMAX")-(*tsci)[2]->getDouble("ZMIN"))*Gaudi::Units::cm - epsilon;
  double positionOfSCTCooling = 0.5 * ((*tsci)[2]->getDouble("ZMAX")+(*tsci)[2]->getDouble("ZMIN"))*Gaudi::Units::cm;


  // For new LMT we get name from SCT table ZSCG.
  // Older versions don't have this defined.
  std::string sctCablesMaterialName = "trt::SCTCables";
  if (!(*zscg)[0]->isFieldNull("TAPEMATERIAL")) {
    sctCablesMaterialName = (*zscg)[0]->getString("TAPEMATERIAL");
  }

  const GeoMaterial * sctCablesMaterial;
  if (sctCablesMaterialName == "sct::CuKapton"){
    // We define it here for now as a quick fix.
    
    // Thickness of CuK 896 tapes smeared in phi = 0.08575cm
    double tapeCrossSection = (*zscg)[0]->getDouble("ALAREA")*Gaudi::Units::cm2;
    double rave = 2*innerRadiusOfSCTCables*outerRadiusOfSCTCables/(innerRadiusOfSCTCables+outerRadiusOfSCTCables);
    double thickness = 988*tapeCrossSection/(2*Gaudi::Units::pi*rave);
    // We need to scale the density to fit in with space given.
    //std::cout << "LMT thickness (Gaudi::Units::mm) = " << thickness/Gaudi::Units::mm << std::endl;
    double densityfactor = thickness/lengthOfSCTCables;
    const GeoElement  *copper  = m_materialManager->getElement("Copper");
    const GeoMaterial *kapton  = m_materialManager->getMaterial("std::Kapton");
    GeoMaterial * matCuKapton   = new GeoMaterial("CuKaptoninTRT",densityfactor * 2.94*GeoModelKernelUnits::gram/Gaudi::Units::cm3);
    matCuKapton->add(const_cast<GeoElement*>(copper),  0.6142);
    matCuKapton->add(const_cast<GeoMaterial*>(kapton), 0.3858);
    matCuKapton->lock();
    sctCablesMaterial = matCuKapton;
  } else {
    sctCablesMaterial = m_materialManager->getMaterial(sctCablesMaterialName);
  }
   
  // SCT Support: 
  GeoTube    *sSCTSupport = new GeoTube( innerRadiusOfSCTSupport, outerRadiusOfSCTSupport, 0.5*lengthOfSCTSupport); 
  GeoLogVol  *lSCTSupport = new GeoLogVol("SCTSupport", sSCTSupport, sctCablesMaterial);
  GeoPhysVol *pSCTSupport = new GeoPhysVol(lSCTSupport);
  GeoTransform *xSCTSupportPlus  = new GeoTransform(GeoTrf::TranslateZ3D(+positionOfSCTSupport));
  GeoTransform *xSCTSupportMinus = new GeoTransform(GeoTrf::TranslateZ3D(-positionOfSCTSupport));
  mother->add(xSCTSupportPlus);
  mother->add(pSCTSupport);
  mother->add(xSCTSupportMinus);
  mother->add(pSCTSupport);
   
  // SCT Cables:
  GeoTube    *sSCTCables = new GeoTube(innerRadiusOfSCTCables, outerRadiusOfSCTCables, 0.5*lengthOfSCTCables); 
  GeoLogVol  *lSCTCables = new GeoLogVol("SCTCables", sSCTCables, m_materialManager->getMaterial("trt::SCTCables"));
  GeoPhysVol *pSCTCables = new GeoPhysVol(lSCTCables);
  GeoTransform *xSCTCablesPlus  = new GeoTransform(GeoTrf::TranslateZ3D(+positionOfSCTCables ));
  GeoTransform *xSCTCablesMinus = new GeoTransform(GeoTrf::TranslateZ3D(-positionOfSCTCables ));

  mother->add(xSCTCablesPlus);
  mother->add(pSCTCables);
  mother->add(xSCTCablesMinus);
  mother->add(pSCTCables);

   
  // SCT Cooling:
  GeoTube    *sSCTCooling = new GeoTube( innerRadiusOfSCTCooling, outerRadiusOfSCTCooling, 0.5*lengthOfSCTCooling); 
  GeoLogVol  *lSCTCooling = new GeoLogVol("SCTCooling", sSCTCooling, m_materialManager->getMaterial("trt::SCTCooling"));
  GeoPhysVol *pSCTCooling = new GeoPhysVol(lSCTCooling);
  GeoTransform *xSCTCoolingPlus  = new GeoTransform(GeoTrf::TranslateZ3D(+positionOfSCTCooling ));
  GeoTransform *xSCTCoolingMinus = new GeoTransform(GeoTrf::TranslateZ3D(-positionOfSCTCooling ));
  mother->add(xSCTCoolingPlus);
  mother->add(pSCTCooling);
  mother->add(xSCTCoolingMinus);
  mother->add(pSCTCooling);


  // Build the INEL part of the services:
  //
  for (int jj =0; jj < SCTELEMENTSINEL; jj++) {
    int ii = jj + TRTELEMENTSINEL;
    // Build the Phys Vol
    std::ostringstream o;
    o << jj;
    std::string logName = "SctInel"+o.str();  
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
      nameStr << "SctInelServices" << ii;
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


const GeoShape* SCT_ServMatFactoryDC2::createShape(int volType, 
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

const GeoMaterial* SCT_ServMatFactoryDC2::createMaterial(const std::string & name,
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

  
