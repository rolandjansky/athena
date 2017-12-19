/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//  HEC2WheelConstruction.cxx 
// 
//  This should only be TEMPORARY!
//  It is a way of placing the individual HEC Wheels into the Endcap cryostat.
//  (Needs a modificatio in LArGeoEndcap/EndcpcrypstatConstruction, where 
//   the present HECConstruction get the old HEC; replace HECConstruction by 
//   HEC2WheelConstruction there and you should get this construciotnof 2 separate wheels.)
//  What we really want is placing the HEC wheels individually into the Endcap cryostat.    
//  
//  When placing the wheel we have to take into account that they are fastened
//  to the cryostat at their rear ends. 
//  This means that, longitudinally, they will shrink from front to back as the cryostat
//  cools down. 
//  The number given in the database at the moment (Oct 2007) are still the warm
//  dimensions!... 
//  
//  In here, use hard coded numbers for the placement (outch!....)
//  but then this is hopefully only very, very temporary!!!!
//   
//  mgf.
//
/////////////////////////////////////////////////////////////////////
#include "LArGeoHec/HEC2WheelConstruction.h"
#include "LArGeoHec/HECWheelConstruction.h"

#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoXF.h"
#include "CLHEP/Geometry/Transform3D.h" 
#include "CLHEP/Vector/Rotation.h" 
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "AthenaKernel/getMessageSvc.h"

#include "GeoModelUtilities/GeoDBUtils.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <string>
#include <cmath>
#include <iostream>


using CLHEP::cm;
using CLHEP::mm;
using CLHEP::deg;
using HepGeom::Transform3D;
using HepGeom::Translate3D;


//Constructor
LArGeo::HEC2WheelConstruction::HEC2WheelConstruction():
  m_physiHEC(0),
  m_posZSide(false)
{

}

//~Destructor
LArGeo::HEC2WheelConstruction::~HEC2WheelConstruction() 
{;}


GeoFullPhysVol* LArGeo::HEC2WheelConstruction::GetEnvelope(bool fullGeo, bool posZSide)
{
  m_posZSide = posZSide;

  if(m_physiHEC) return m_physiHEC->clone();

  // Get access to the material manager:
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();

  MsgStream log(Athena::getMessageSvc(),"HEC2WheelConstruction " );
  

  log << MSG::DEBUG << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  log << MSG::DEBUG << "+                                                   +" << endmsg;
  log << MSG::DEBUG << "+         Start of HEC2Wheel GeoModel definition    +" << endmsg;
  log << MSG::DEBUG << "+                                                   +" << endmsg;
  log << MSG::DEBUG << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HEC2WheelConstruction, cannot access DetectorStore");
  }
 
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in HEC2WheelConstruction, cannot access Material Manager");
  }

  StatusCode sc;
  IRDBAccessSvc *pAccessSvc;
  sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }

  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }

  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();
  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

  IRDBRecordset_ptr hadronicEndcap = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  if(hadronicEndcap->size()>0)
    log << MSG::ALWAYS << "Using numbers from HadronicEndcap tag:  " << hadronicEndcap->tagName() << endmsg;
  else
    throw std::runtime_error("Error in HECConstruction:  hadronicEendcap not found");

  IRDBRecordset_ptr hecLongitudinalBlock = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 
  if(hecLongitudinalBlock->size()>0) 
    log << MSG::ALWAYS << "Using numbers from HecLongitudinalBlock tag:  " << hecLongitudinalBlock->tagName() << endmsg;
  else 
    throw std::runtime_error("Error in HECConstruction:  hecLongitudinalBlock not found");

  IRDBRecordset_ptr larPosition  =  pAccessSvc->getRecordsetPtr("LArPosition",detectorKey, detectorNode);
  if (larPosition->size()==0 ) {
    larPosition = pAccessSvc->getRecordsetPtr("LArPosition", "LArPosition-00");
    if (larPosition->size()==0 ) {
      throw std::runtime_error("Error, no lar position table in database!");
    }
  }

  // This is a start for letting everyhting go cold (shrinkCold = 1.0 means warm; = 0.997 means cold)
  double shrinkCold            = 1.0 ; 

  double rInner1               = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*cm;
  double rInner2               = shrinkCold * (*hecLongitudinalBlock)[1]->getDouble("BLRMN")*cm;
  double rOuter                = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*cm;
  double wheelGap              = shrinkCold * (*hadronicEndcap)[0]->getDouble("GAPWHL")*cm;

  
  GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in HEC2WheelConstruction, std::LiquidArgon is not found.");

  GeoPcon*           solidHEC;     //pointer to the solid HEC
  const GeoLogVol*   logicHEC;     //pointer to the logical HEC
  GeoFullPhysVol*    physiHEC;     //pointer to the physical HEC

  

  // YIKES! Hard-coded numbers!!!

  if (rOuter<2100.*mm) rOuter  = shrinkCold *2130*mm;  // Needs fixing in database to make room for HEC Clamping bars! 
                                           // Caution: We'll need 2 Routers - One for Wheel, one for Module!

  double     HEC1length = shrinkCold * 816.5*mm;
  double     HEC2length = shrinkCold * 961.0*mm;
  double     TotalWarmHEC = HEC1length + wheelGap + HEC2length ;  // For the moment, this defines the HEC rear face  
  double     TotalHECLength = TotalWarmHEC;                      // (where is that -cold- in the cryostat?

  double     zCoordinate[4];
  zCoordinate[0]=shrinkCold * 0.0*cm; 
  zCoordinate[1]=shrinkCold * 28.05*cm; 
  zCoordinate[2]=shrinkCold * 28.051*cm; 
  zCoordinate[3]=shrinkCold * (181.9*cm+4.0*mm); // JFB, MGF:  more room in the mother fiducal volume for misalignment.
 // 37.2, 47.5, 203 cm for R

  solidHEC = new GeoPcon(0., 2.*M_PI);
  for (int i=0; i< 4; i++) 
    {
      if (i<2) solidHEC->addPlane(zCoordinate[i],rInner1 - shrinkCold*1.*mm,rOuter);
      else     solidHEC->addPlane(zCoordinate[i],rInner2 - shrinkCold*1.*mm,rOuter);
   }
  
  std::string hecName = "LAr::HEC::Mother";
  logicHEC = new GeoLogVol(hecName, solidHEC , LAr);
  physiHEC = new GeoFullPhysVol(logicHEC);
  
  //--- Make the Front Wheel alignable:

  const IRDBRecord *posHec1 = GeoDBUtils::getTransformRecord(larPosition, m_posZSide ? "HEC1_POS":"HEC1_NEG");
  Transform3D xfPosHec1 = posHec1 ? GeoDBUtils::getTransform(posHec1) : Translate3D(0.,0.,0.);
  GeoAlignableTransform *xfHec1 = new GeoAlignableTransform(xfPosHec1);

  std::string tag1 = m_posZSide? std::string("HEC1_POS") : std::string("HEC1_NEG");

  HECWheelConstruction theFrontHEC(fullGeo,"front",false,m_posZSide) ;
  GeoFullPhysVol* EnvelopeF = theFrontHEC.GetEnvelope();

  StoredPhysVol *sPhysVolHec1 = new StoredPhysVol(EnvelopeF);
  sc=detStore->record(sPhysVolHec1,tag1);
  if(!sc.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag1).c_str());

  StoredAlignX *sAlignX1 = new StoredAlignX(xfHec1);
  sc=detStore->record(sAlignX1,tag1);
  if(!sc.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag1).c_str());

  physiHEC->add(xfHec1);
  physiHEC->add(new GeoIdentifierTag(0));
  physiHEC->add(EnvelopeF);

  //--- Now the Rear Wheel:

  double Zpos = TotalHECLength - HEC2length  ;  // It is fixed at its end; position it from there.
  const IRDBRecord *posHec2 = GeoDBUtils::getTransformRecord(larPosition, m_posZSide ? "HEC2_POS":"HEC2_NEG");
  Transform3D xfPosHec2 = posHec2 ? GeoDBUtils::getTransform(posHec2):Translate3D(0.,0.,Zpos);

 
  GeoAlignableTransform *xfHec2 = new GeoAlignableTransform(xfPosHec2);

  std::string tag2 = m_posZSide? std::string("HEC2_POS") : std::string("HEC2_NEG");

  HECWheelConstruction theRearHEC(fullGeo,"rear",false,m_posZSide);
  GeoFullPhysVol* EnvelopeR = theRearHEC.GetEnvelope();

  StoredPhysVol *sPhysVolHec2 = new StoredPhysVol(EnvelopeR);
  sc=detStore->record(sPhysVolHec2,tag2);
  if(!sc.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag2).c_str());

  StoredAlignX *sAlignX2 = new StoredAlignX(xfHec2);
  sc=detStore->record(sAlignX2,tag2);
  if(!sc.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag2).c_str());

  physiHEC->add(xfHec2);
  physiHEC->add(new GeoIdentifierTag(1));
  physiHEC->add(EnvelopeR);





  return physiHEC;
}
