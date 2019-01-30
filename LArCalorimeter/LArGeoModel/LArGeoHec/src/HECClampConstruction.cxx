/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//  HECClampConstruction.cxx 1.0.0
//  
//  GetClampingBar:
//  Construct Outer Connecting Bars and Support Rails for the
//  HEC Wheels (Front or Rear)
//  AddClamps:
//  Places the Bars and Rails on the HEC Wheel 
//  
//  25. Oct 2007  M. Fincke-Keeler 
//  
//====================================================================
#include "LArGeoHec/HECClampConstruction.h"
#include "LArGeoHec/HECModuleConstruction.h"

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
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoGenericFunctions/Variable.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthenaKernel/getMessageSvc.h"


#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <string>
#include <cmath>
#include <iostream>

using Gaudi::Units::cm;
using Gaudi::Units::mm;
using Gaudi::Units::deg;
using GeoTrf::RotateZ3D;
using GeoTrf::Translate3D;
using GeoTrf::TranslateZ3D;


//Constructor
LArGeo::HECClampConstruction::HECClampConstruction(bool front, bool posZSide)
  : //m_rail(false),
    //m_left(false),
    m_moduleNumber(0),
    m_moduleRouter(0),
    m_modulePhistart(0),
    m_rOuter(0),
    m_moduleDeltaPhi(0)
{
  m_front    = front;
  m_posZSide = posZSide;
}

//~Destructor
LArGeo::HECClampConstruction::~HECClampConstruction() 
{
}


GeoPhysVol* LArGeo::HECClampConstruction::GetClampingBar(bool rail,bool left)
{
  
  ISvcLocator *svcLocator = Gaudi::svcLocator();

  MsgStream log(Athena::getMessageSvc(),"HECClampConstruction " );
  log << MSG::DEBUG << " In HECClampConstruction " << endmsg;
  

  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECModuleConstruction(ClampBar), cannot access DetectorStore");
  }
  

  // Get access to the material manager:
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in HECModuleConstruction(ClampBar), cannot access Material Manager");
  }  
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in HECModuleConstruction(ClampBar), std::LiquidArgon is not found.");  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in HECModuleConstruction(ClampBar), std::Iron is not found.");
 

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

  IRDBRecordset_ptr hecLongitudinalBlock = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 
  IRDBRecordset_ptr hadronicEndcap       = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  

  //----------------------------------------------------------------
  // Collect all the numbers we need from the database: 
  //----------------------------------------------------------------
  //
  // NOT READY YET
  //
  
  //----------------------------------------------------------------
  //   Add outer clamping bars for Atlas wheels
  //----------------------------------------------------------------
  // NB.: The outer clamping bars have notches for the HEC cables.
  //      that sit at different levels for Front/Rear Wheel
  //  
  double shrinkCold = 1.0;   // thermal expansion factor: 1.0 = warm

  int    moduleNumber   = (*hadronicEndcap)[0]->getInt("NSCT");
  double moduleRouter   = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*cm;
  double modulePhistart = 264.375*deg;          // (270.-11.25/2.)*deg
  double rOuter         = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*cm;
  double moduleDeltaPhi = 2*M_PI/moduleNumber; //11.25*deg;  
  m_moduleNumber  = moduleNumber;
  m_moduleRouter  = moduleRouter;
  m_modulePhistart= modulePhistart;
  m_rOuter        = rOuter;
  m_moduleDeltaPhi= moduleDeltaPhi ;

  std::string clampName = "LAr::HEC::Clamp";
  std::string larName = "LAr::HEC::Clamp::LiquidArgon";
  double    g4allow     =   0.01 *mm;
  double    extThick    = shrinkCold *   37.*mm;
  double    extLength   = shrinkCold *   84.*mm;
  double    clampWidth  = shrinkCold *  147.0 *mm;
  double    clampThick  = shrinkCold *   50.*mm;
  double    notchLevel  = shrinkCold *   25.*mm;
  double    notchHeight = shrinkCold *   15.*mm;
  double    notchWidth  = shrinkCold *   20.*mm;
  double    slotWidth   = shrinkCold *   20.*mm;
  double    clampLength = shrinkCold *  815.*mm;  
  double    clampAngle  =  2.* asin(clampWidth/(2.*(moduleRouter+clampThick/2.)));
  double    slotAngle   =  2.* asin(notchWidth/(2.*(moduleRouter+clampThick/2.)));

  std::vector<double>  notchLocation;
  if (m_front){
    if (!rail){
//       notchLocation.push_back(shrinkCold * 201.*mm);
//       notchLocation.push_back(shrinkCold * 469.*mm);
//       notchLocation.push_back(shrinkCold * 737.*mm);
      notchLocation.push_back(shrinkCold * 200.*mm); // adjust for shortened clamp
      notchLocation.push_back(shrinkCold * 468.*mm);
      notchLocation.push_back(shrinkCold * 736.*mm);
    } 
    else {
//       notchLocation.push_back(shrinkCold * 206.*mm);
//       notchLocation.push_back(shrinkCold * 474.*mm);
//       notchLocation.push_back(shrinkCold * 742.*mm);
      notchLocation.push_back(shrinkCold * 201.*mm); // adjust for shortened rail
      notchLocation.push_back(shrinkCold * 469.*mm);
      notchLocation.push_back(shrinkCold * 737.*mm);
      clampWidth  =  shrinkCold * 148.5 *mm;
      clampThick  =  shrinkCold *  98.*mm;   //  98.*mm   (this is the total overall rail thickness)
      clampLength =  shrinkCold * 815.*mm;
    } 
  } 
  else {
    if (!rail){
//       notchLocation.push_back(shrinkCold * 364.0*mm); 
//       notchLocation.push_back(shrinkCold * 832.0*mm); 
      notchLocation.push_back(shrinkCold * 363.0*mm); // adjust for shortened clamp
      notchLocation.push_back(shrinkCold * 831.0*mm); 
      clampLength =  shrinkCold * 960.*mm;
    } 
    else {
//       notchLocation.push_back(shrinkCold * 372.5*mm); 
//       notchLocation.push_back(shrinkCold * 840.5*mm); 
      notchLocation.push_back(shrinkCold * 364.*mm);  // adjust for shortened rail
      notchLocation.push_back(shrinkCold * 832.*mm); 
      clampWidth  =  shrinkCold * 148.5 *mm;
      clampThick  =  shrinkCold *  98.*mm;   
      clampLength =  shrinkCold * 960.*mm;
      extLength   =  shrinkCold * 132.*mm;
    } 
    notchWidth  = shrinkCold * 16.*mm;
    notchHeight = shrinkCold * 16.*mm;
    notchLevel  = shrinkCold * 0.*mm;
  }
  



  // Construct the the slots and notches here. For the support rails, they have to be inserted
  // into the physExt[0] volume. For the regular clamps they go into physClamp.
  GeoTubs*         clampExt[3];
  const GeoLogVol* logExt[3];
  GeoPhysVol*      physExt[3];

  GeoTubs*         Notch      = new GeoTubs(moduleRouter+notchLevel+g4allow, moduleRouter+notchLevel+notchHeight-g4allow, 
					    notchWidth/2.,
					    modulePhistart-(clampAngle/2.) , clampAngle);
  const GeoLogVol* logNotch   = new GeoLogVol(larName, Notch, LAr);  
  GeoPhysVol*      physiNotch = new GeoPhysVol(logNotch);
  
  GeoTubs*         Slot      = new GeoTubs(moduleRouter+g4allow, moduleRouter+notchLevel-g4allow, slotWidth/2. ,
					   modulePhistart-(slotAngle/2.) , slotAngle);
  const GeoLogVol* logSlot   = new GeoLogVol(larName, Slot, LAr);  
  GeoPhysVol*      physiSlot = new GeoPhysVol(logSlot);



  GeoTubs*         clampBar;
  const GeoLogVol* logClamp;
  GeoPhysVol*      physClamp;

  if(!rail){
    clampBar = new GeoTubs(moduleRouter, moduleRouter+clampThick, clampLength/2. ,
			   modulePhistart-(clampAngle/2.) , clampAngle);
    logClamp = new GeoLogVol(clampName, clampBar, Iron);  
    physClamp= new GeoPhysVol(logClamp);
  }
  else{

    //For the support rails, make the mother LAr and the place the steel inside: 

    clampBar = new GeoTubs(moduleRouter, moduleRouter+clampThick, clampLength/2. ,
			   modulePhistart-(clampAngle/2.) , clampAngle);
    logClamp = new GeoLogVol(larName, clampBar, LAr);  
    physClamp= new GeoPhysVol(logClamp);


    clampExt[0] = new GeoTubs(moduleRouter+g4allow,                     moduleRouter+clampThick-extThick, 
			      clampLength/2. , modulePhistart-(clampAngle/2.), clampAngle);
    clampExt[1] = new GeoTubs(moduleRouter+clampThick-extThick+g4allow, moduleRouter+clampThick-g4allow, 
			      clampLength/2. , modulePhistart,                 clampAngle/2.);
    clampExt[2] = new GeoTubs(moduleRouter+clampThick-extThick+g4allow, moduleRouter+clampThick-g4allow, 
			      extLength/2. , modulePhistart,                   clampAngle/2.);




    for (int iext=0; iext<3; iext++){

      logExt[iext] = new GeoLogVol(clampName, clampExt[iext], Iron);  
      physExt[iext]= new GeoPhysVol(logExt[iext]);

      
      if   (iext==0) {
	for ( unsigned int i = 0; i < notchLocation.size(); i++ ) 
	  {
	    physExt[0]->add( new GeoIdentifierTag(i) );
	    physExt[0]->add( new GeoTransform(Translate3D(0,0,-clampLength/2.+notchLocation[i])) );  
	    physExt[0]->add( physiNotch );
	    if (m_front){
	      physExt[0]->add( new GeoIdentifierTag(i) );
	      physExt[0]->add( new GeoTransform(Translate3D(0,0,-clampLength/2.+notchLocation[i])) );  
	      physExt[0]->add( physiSlot );
	    }
	  }
      }


      if     (iext==1 && !left) physClamp->add(new GeoTransform(RotateZ3D(-clampAngle/2.)));
      else if(iext==2 && left)  physClamp->add(new GeoTransform(TranslateZ3D((clampLength-extLength)/2.)
								*RotateZ3D(-clampAngle/2.)));
      else if(iext==2)          physClamp->add(new GeoTransform(TranslateZ3D((clampLength-extLength)/2.)
								*RotateZ3D( 0.)));

      if (left) physClamp->add( new GeoIdentifierTag(16) );
      else      physClamp->add( new GeoIdentifierTag(32) );


      physClamp->add(physExt[iext]);

    }


    for (int i=0; i<3; i++){
      clampExt[i]->ref();     clampExt[i]->unref();
      logExt[i]->ref();       logExt[i]->unref();
      physExt[i]->ref();      physExt[i]->unref();
    }
 
  }
    

  for ( unsigned int i = 0; i < notchLocation.size(); i++ ) 
    {
      if(!rail){
	physClamp->add( new GeoIdentifierTag(i) );
	physClamp->add( new GeoTransform(Translate3D(0,0,-clampLength/2.+notchLocation[i])) );  
	physClamp->add( physiNotch );
      }
    }
  
  if(m_front)  
    {      
      for ( unsigned int i = 0; i < notchLocation.size(); i++ ) 
	{
	  if(!rail){
	    physClamp->add( new GeoIdentifierTag(i) );
	    physClamp->add( new GeoTransform(Translate3D(0,0,-clampLength/2.+notchLocation[i])) );  
	    physClamp->add( physiSlot );
	  }
	}
      
    }
  
  

  Slot->ref();          Slot->unref();     
  logSlot->ref();       logSlot->unref();     
  physiSlot->ref();     physiSlot->unref();     
  clampBar->ref();       clampBar->unref();     
  logClamp->ref();       logClamp->unref();     
  Notch->ref();          Notch->unref();     
  logNotch->ref();       logNotch->unref();     
  physiNotch->ref();     physiNotch->unref();     


  return physClamp;
}



void LArGeo::HECClampConstruction::AddClamps(GeoFullPhysVol* physiHECWheel)
{
  //----------------------------------------------------------------
  //   Add Outer Connecting Bars to HEC Wheel
  //----------------------------------------------------------------

  double shrinkCold = 1.0 ; // thermal expansion factor: 1.0 = warm

  std::string clampName = "LAr::HEC::Clamp";
  std::string larName   = "LAr::HEC::Clamp::LiquidArgon";
  double    clampLength =  shrinkCold * 815.*mm;  
  double    railLength  =  shrinkCold * 815.*mm;  
  double    railOffset  =  shrinkCold * 0.*mm;  
  if (!m_front) { 
    clampLength =  shrinkCold * 960.*mm;  
    railLength  =  shrinkCold * 960.*mm;
    railOffset  =  shrinkCold * 0.*mm;  
  }

  GeoVPhysVol* clampingBar   = GetClampingBar(false,false);
  GeoVPhysVol* clampingRailR = GetClampingBar(true,false);
  GeoVPhysVol* clampingRailL = GetClampingBar(true,true);



  // In the below positioning sequence, the ORDER MATTERS!
  // 

  GeoTransform *xt = new GeoTransform(TranslateZ3D(clampLength/2.));
  physiHECWheel->add(xt);

  GeoSerialIdentifier  *sIC = new GeoSerialIdentifier(0);
  GeoGenfun::Variable    IndexC;

  if (m_posZSide)
    {
      GeoGenfun::GENFUNCTION ModuleRotationAngleC = -m_modulePhistart+m_moduleDeltaPhi + m_moduleDeltaPhi*IndexC;
      GeoXF::TRANSFUNCTION tC   = GeoXF::Pow(RotateZ3D(1.0),ModuleRotationAngleC);
      GeoSerialTransformer *sTC = new GeoSerialTransformer (clampingBar,&tC,((m_moduleNumber/2)-1));
      physiHECWheel->add(sIC);
      physiHECWheel->add(sTC);
      sTC->ref();  sTC->unref();
    }
  else
    {
      GeoGenfun::GENFUNCTION ModuleRotationAngleC = -m_modulePhistart-m_moduleDeltaPhi+180*deg -  m_moduleDeltaPhi*IndexC;
      GeoXF::TRANSFUNCTION tC   = GeoXF::Pow(RotateZ3D(1.0),ModuleRotationAngleC);
      GeoSerialTransformer *sTC = new GeoSerialTransformer (clampingBar,&tC,((m_moduleNumber/2)-1));
      physiHECWheel->add(sIC);
      physiHECWheel->add(sTC);
      sTC->ref();  sTC->unref();
    }


  physiHECWheel->add(new GeoTransform(TranslateZ3D(railLength/2.-railOffset)*RotateZ3D(-m_modulePhistart-(180.*deg))));
  //GeoSerialIdentifier  *sIR = new GeoSerialIdentifier(15); // For the pos z-side this is the position of module no.15
  //physiHECWheel->add(sIR);                                 // No need to aplly it if we position clamps in this order
  //// But for the neg z-side we do have to re-number, because the rails have sided-ness!
  if (!m_posZSide)
    {
      GeoSerialIdentifier  *sIR = new GeoSerialIdentifier(31); 
      physiHECWheel->add(sIR);                                 
    }
  physiHECWheel->add(clampingRailL);


  physiHECWheel->add(xt);
  GeoGenfun::Variable    IndexC2;

  if (m_posZSide)
    {
      GeoGenfun::GENFUNCTION ModuleRotationAngleC2 = -m_modulePhistart+m_moduleDeltaPhi-(180.*deg) + m_moduleDeltaPhi*IndexC2;
      GeoXF::TRANSFUNCTION tC2   = GeoXF::Pow(RotateZ3D(1.0),ModuleRotationAngleC2);
      GeoSerialTransformer *sTC2 = new GeoSerialTransformer (clampingBar,&tC2,((m_moduleNumber/2)-1));
      physiHECWheel->add(sTC2);
      sTC2->ref(); sTC2->unref();
    }
  else
    {
      GeoGenfun::GENFUNCTION ModuleRotationAngleC2 = -m_modulePhistart-m_moduleDeltaPhi -  m_moduleDeltaPhi*IndexC2;
      GeoXF::TRANSFUNCTION tC2   = GeoXF::Pow(RotateZ3D(1.0),ModuleRotationAngleC2);
      GeoSerialTransformer *sTC2 = new GeoSerialTransformer (clampingBar,&tC2,((m_moduleNumber/2)-1));
      physiHECWheel->add(sTC2);
      sTC2->ref(); sTC2->unref();
    }

  physiHECWheel->add(new GeoTransform(TranslateZ3D(railLength/2.)*RotateZ3D(-m_modulePhistart))); 
  if (!m_posZSide)
    {
      GeoSerialIdentifier  *sIR = new GeoSerialIdentifier(15); 
      physiHECWheel->add(sIR);                                 
    }
  physiHECWheel->add(clampingRailR);


   clampingBar->ref();   clampingBar->unref();  
   clampingRailR->ref(); clampingRailR->unref(); 
   clampingRailL->ref(); clampingRailL->unref(); 
   xt->ref();   xt->unref();   
   sIC->ref();  sIC->unref();

}



