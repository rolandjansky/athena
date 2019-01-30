/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTBFactory.h"
#include "TileGeoSectionBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"
#include "TileConditions/TileCablingService.h"
#include "CaloIdentifier/TileID.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "GeoGenericFunctions/AbsFunction.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoTransform.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace GeoGenfun;
using namespace GeoXF;

 
// Constructor:
TileTBFactory::TileTBFactory(StoreGateSvc *pDetStore,
                             TileDetDescrManager *manager,
                             bool addPlates,
                             int ushape,
                             int glue,
                             int cstube,
                             MsgStream *log)
  : m_detectorStore(pDetStore)
  , m_detectorManager(manager)
  , m_log(log)
  , m_addPlatesToCellVolume(addPlates)
  , m_uShape(ushape)
  , m_glue(glue)
  , m_csTube(cstube)
  , m_testbeamGeometry(true)
  , m_verbose(log->level()<=MSG::VERBOSE)
{
}
  
// Destructor:
TileTBFactory::~TileTBFactory()
{
}
  
// Creation of geometry:
void TileTBFactory::create(GeoPhysVol *world) 
{
  (*m_log) << MSG::INFO <<" Entering TileTBFactory::create()" << endmsg;

  // -------- -------- MATERIAL MANAGER -------- ----------
  const StoredMaterialManager* theMaterialManager = nullptr;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(theMaterialManager, "MATERIALS")) 
  { 
    (*m_log) << MSG::ERROR << "Could not find Material Manager MATERIALS" << endmsg; 
    return; 
  }
  const GeoMaterial* matAir = theMaterialManager->getMaterial("std::Air");

  // -------- -------- SECTION BUILDER  -------- ----------
  TileDddbManager* dbManager = m_detectorManager->getDbManager();
  TileGeoSectionBuilder* sectionBuilder = new TileGeoSectionBuilder(theMaterialManager,dbManager,m_uShape,m_glue,m_csTube,m_log);

  //Tile envelope thickness, Extended & ITC offset
  //and Central module center Z coordinate
  //double tileTBEnvThickness = 0;
  //double extOffset = 0;
  //double itcOffset = 0;
  double rMinITC = 0;
  double dzITC2 = 0;

  double thicknessWedgeMother, heightWedgeMother, dy1WedgeMother, dy2WedgeMother;
  double dzGlue;     //Thickness of glue layer in the absorber
  //double zEndSection;

  // double deltaPhi = 360./dbManager->TILEnmodul();
  double deltaPhi = 360./ 64; // we know apriory that 64 modules makes full cylinder

  double PhiMax = -360.0;
  double PhiMin = +360.0;
  double RInMin = 999999.9;
  double ROutMax = 0;
  bool IfB = false;
  bool IfEBNeg = false;
  bool IfEBPos = false;
  double tileTBEnvThickness = 0;
  double ZLengthEBarrelNeg =0, ZLengthEBarrelPos =0;
  int ModuleNcp =0;
  //I take Finger Length here; it's due to Fingers need in their own Envelope
  //and despite they are not in GlobalEnv table they shift other Envelopes (EBarrels, for example)
  dbManager->SetCurrentTifg(2);  //ebarrel finger
  double EBFingerLength = dbManager->TIFGdz();
  dbManager->SetCurrentTifg(2);  //barrel finger - make it of the same size as ext.barrel finger!!!
  double BFingerLength = dbManager->TIFGdz();

  // finger starts at the same radius as girder - after sensitive part
  dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
  double FingerRmin = dbManager->TILBrmax();

  int n_env = dbManager->GetNumberOfEnv();
  for(int i = 0; i < n_env ; ++i)
  {
    dbManager->SetCurrentEnvByIndex(i);
    tileTBEnvThickness += dbManager->GetEnvZLength();
    if(dbManager->GetEnvType() == 1){
      IfB = true;
      //ZLengthBarrel = dbManager->GetEnvZLength();
    }
    if(dbManager->GetEnvType() == 2){
      IfEBNeg = true;
      ZLengthEBarrelNeg = dbManager->GetEnvZLength();
    }
    if(dbManager->GetEnvType() == 3){
      IfEBPos = true;
      ZLengthEBarrelPos = dbManager->GetEnvZLength();
    }
    PhiMin = std::min(PhiMin, dbManager->GetEnvDPhi());
    PhiMax = std::max(PhiMax, dbManager->GetEnvNModules()*deltaPhi + dbManager->GetEnvDPhi());
    RInMin = std::min(RInMin,  dbManager->GetEnvRin());
    ROutMax= std::max(ROutMax, dbManager->GetEnvRout());
  }

 (*m_log) << MSG::DEBUG << "tileTBEnvThickness 1 " << tileTBEnvThickness << endmsg;

 // enlarge mother volume by extra distance between barrel and positive ext.barrel
  if(IfEBPos){
    dbManager->SetCurrentEnvByType(3);
    tileTBEnvThickness += dbManager->GetEnvDZ()*dbManager->GetEnvSide();
  }

  (*m_log) << MSG::DEBUG << "tileTBEnvThickness 2 " << tileTBEnvThickness << endmsg;

  // enlarge mother volume by extra distance between barrel and negative ext.barrel
  if(IfEBNeg){
    dbManager->SetCurrentEnvByType(2);
    tileTBEnvThickness += dbManager->GetEnvDZ()*dbManager->GetEnvSide();
  }

 (*m_log) << MSG::DEBUG << "tileTBEnvThickness 3 " << tileTBEnvThickness << endmsg;

  // take into account extra DZ for barrel only if there are no Ext.Barrel modules
  // but keep the mother volume symmetric (i.e. add DZ on both sides)
  if((!IfEBNeg)&&(!IfEBPos)){
    dbManager->SetCurrentEnvByType(1);
    tileTBEnvThickness += 2 * fabs(dbManager->GetEnvDZ());
  }

  (*m_log) << MSG::DEBUG << "Presence of barrel " << ((IfB) ? "true" : "false") << endmsg;
  (*m_log) << MSG::DEBUG << "Presence of positive ext.barrel " << ((IfEBPos) ? "true" : "false") << endmsg;
  (*m_log) << MSG::DEBUG << "Presence of negative ext.barrel " << ((IfEBNeg) ? "true" : "false") << endmsg;

  (*m_log) << MSG::DEBUG << "tileTBEnvThickness " << tileTBEnvThickness << endmsg;


// -------------------- TILE  TB  ENVELOPE ---------------------

  //int EnvNumLayer = 64;
  //dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
  //double deltaPhi = 360./dbManager->TILEnmodul();
  GeoTubs* tileTBEnv = new GeoTubs(RInMin * Gaudi::Units::cm,
				   ROutMax * Gaudi::Units::cm,
				   tileTBEnvThickness/2.0 * Gaudi::Units::cm,
				   PhiMin*Gaudi::Units::deg,
				   (PhiMax - PhiMin)*Gaudi::Units::deg);
  
  (*m_log) << MSG::DEBUG << "TileTB envelope parameters: " 
      << " length=" << tileTBEnvThickness << " cm"
      << " Rmin=" << RInMin << " cm"
      << " Rmax=" << ROutMax << " cm"
      << " PhiMin=" << PhiMin << " deg"
      << " PhiMax=" << PhiMax << " deg"
      << endmsg;

  GeoLogVol* lvTileTBEnv = new GeoLogVol("TileTBEnv",tileTBEnv,matAir);
  GeoFullPhysVol* pvTileTBEnv = new GeoFullPhysVol(lvTileTBEnv);
  
  GeoFullPhysVol *pvBarrelMother     =0;
  GeoFullPhysVol *pvEBarrelMotherNeg =0, *pvEBarrelMotherPos =0;
  GeoFullPhysVol *pvFingerMotherNeg  =0, *pvFingerMotherPos  =0;
  GeoFullPhysVol *pvEFingerMotherNeg =0, *pvEFingerMotherPos =0;
  GeoFullPhysVol *pvITCMotherPos     =0, *pvITCMotherNeg     =0;

  int NumberOfEnv = dbManager->GetNumberOfEnv();
  for(int EnvCounter = 0; EnvCounter < NumberOfEnv; ++EnvCounter){ //Loop over Envelopes
    dbManager->SetCurrentEnvByIndex(EnvCounter);
    int EnvType = dbManager->GetEnvType();
    int NumberOfMod = dbManager->GetEnvNModules();

    //----------------------- BUILDING ENVELOPES------------------------
    
    // It may be usful on the way of universalization
    //    GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
    //					 (dbManager->GetEnvRout()) * Gaudi::Units::cm,
    //				 (dbManager->GetEnvZLength())/2.0 * Gaudi::Units::cm,
    //				 0.0 * deltaPhi * Gaudi::Units::deg,
    //				 (NumberOfMod)*deltaPhi*Gaudi::Units::deg);
  
    if(EnvType == 1) {

      GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
					   (dbManager->GetEnvRout()) * Gaudi::Units::cm,
					   (dbManager->GetEnvZLength() - 2 * BFingerLength)/2.0 * Gaudi::Units::cm,
					   0.0 * Gaudi::Units::deg,
					   NumberOfMod*deltaPhi*Gaudi::Units::deg);

      GeoTubs* barrelMother = GeneralMother;      
      GeoLogVol* lvBarrelMother = new GeoLogVol("Barrel",barrelMother,matAir);
      pvBarrelMother = new GeoFullPhysVol(lvBarrelMother);

      (*m_log) << MSG::DEBUG << "Barrel envelope parameters: " 
          << " length=" << (dbManager->GetEnvZLength() - 2 * BFingerLength) << " cm"
          << " Rmin=" << (dbManager->GetEnvRin()) << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
      
      //Envelopes for two barrel fingers
      dbManager->SetCurrentTifg(2);  //use small size for barrel finger !

      GeoTubs* fingerMother = new GeoTubs(FingerRmin*Gaudi::Units::cm,
      					  dbManager->GetEnvRout()*Gaudi::Units::cm,
      					  BFingerLength/2.*Gaudi::Units::cm,
      					  0.0 * Gaudi::Units::deg,
      					  NumberOfMod*deltaPhi*Gaudi::Units::deg);

      GeoLogVol* lvFingerMother = new GeoLogVol("Finger",fingerMother,matAir);
      pvFingerMotherPos = new GeoFullPhysVol(lvFingerMother);
      pvFingerMotherNeg = new GeoFullPhysVol(lvFingerMother);

      (*m_log) << MSG::DEBUG << "Barrel finger envelope parameters: " 
          << " length=" << BFingerLength << " cm"
          << " Rmin=" << FingerRmin << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
    }
    
    if(EnvType == 3) {   
      GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
					   (dbManager->GetEnvRout()) * Gaudi::Units::cm,
					   (dbManager->GetEnvZLength() - EBFingerLength)/2.0 * Gaudi::Units::cm,
					   0.0 * Gaudi::Units::deg,
					   NumberOfMod*deltaPhi*Gaudi::Units::deg);

      GeoTubs* ebarrelMotherPos = GeneralMother;
      GeoLogVol* lvEBarrelMotherPos = new GeoLogVol("EBarrel",ebarrelMotherPos,matAir);
      pvEBarrelMotherPos = new GeoFullPhysVol(lvEBarrelMotherPos);

      (*m_log) << MSG::DEBUG << "Positive ext.barrel envelope parameters: " 
          << " length=" << (dbManager->GetEnvZLength() - EBFingerLength) << " cm"
          << " Rmin=" << (dbManager->GetEnvRin()) << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;

      //Envelope for finger separately
      GeoTubs* fingerMother = new GeoTubs(FingerRmin*Gaudi::Units::cm,
					  dbManager->GetEnvRout()*Gaudi::Units::cm,
					  EBFingerLength/2.*Gaudi::Units::cm,
					  0.0 * Gaudi::Units::deg,
					  NumberOfMod*deltaPhi*Gaudi::Units::deg);

      GeoLogVol* lvEFingerMother = new GeoLogVol("EFinger",fingerMother,matAir);
      pvEFingerMotherPos = new GeoFullPhysVol(lvEFingerMother);
    
      (*m_log) << MSG::DEBUG << "Positive ext.barrel finger envelope parameters: " 
          << " length=" << EBFingerLength << " cm"
          << " Rmin=" << FingerRmin << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
    }
    
    if(EnvType == 2) {    
      GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
					   (dbManager->GetEnvRout()) * Gaudi::Units::cm,
					   (dbManager->GetEnvZLength() - EBFingerLength)/2.0 * Gaudi::Units::cm,
					   0.0 * Gaudi::Units::deg,
					   NumberOfMod*deltaPhi*Gaudi::Units::deg);
      GeoTubs* ebarrelMotherNeg = GeneralMother;
      GeoLogVol* lvEBarrelMotherNeg = new GeoLogVol("EBarrel",ebarrelMotherNeg,matAir);  
      pvEBarrelMotherNeg = new GeoFullPhysVol(lvEBarrelMotherNeg);
      
      (*m_log) << MSG::DEBUG << "Negative ext.barrel envelope parameters: " 
          << " length=" << (dbManager->GetEnvZLength() - EBFingerLength) << " cm"
          << " Rmin=" << (dbManager->GetEnvRin()) << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;

      //Envelope for finger separately
      GeoTubs* fingerMother = new GeoTubs(FingerRmin*Gaudi::Units::cm,
					  dbManager->GetEnvRout()*Gaudi::Units::cm,
					  EBFingerLength/2.*Gaudi::Units::cm,
					  0.0 * Gaudi::Units::deg,
					  NumberOfMod*deltaPhi*Gaudi::Units::deg);
      
      GeoLogVol* lvEFingerMother = new GeoLogVol("EFinger",fingerMother,matAir);
      pvEFingerMotherNeg = new GeoFullPhysVol(lvEFingerMother);

      (*m_log) << MSG::DEBUG << "Negative ext.barrel finger envelope parameters: " 
          << " length=" << EBFingerLength << " cm"
          << " Rmin=" << FingerRmin << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
    }
    
    if(EnvType == 5) {
      GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
					   (dbManager->GetEnvRout()) * Gaudi::Units::cm,
					   (dbManager->GetEnvZLength())/2.0 * Gaudi::Units::cm,
					   0.0 * Gaudi::Units::deg,
					   NumberOfMod*deltaPhi*Gaudi::Units::deg);
      
      GeoTubs* itcMother = GeneralMother;
      GeoLogVol* lvITCMother = new GeoLogVol("ITC",itcMother,matAir);
      pvITCMotherPos = new GeoFullPhysVol(lvITCMother);

      (*m_log) << MSG::DEBUG << "Positive ITC envelope parameters: " 
          << " length=" << (dbManager->GetEnvZLength()) << " cm"
          << " Rmin=" << (dbManager->GetEnvRin()) << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
    }
    
    if(EnvType == 4) {
      GeoTubs* GeneralMother = new GeoTubs((dbManager->GetEnvRin()) * Gaudi::Units::cm,
					   (dbManager->GetEnvRout()) * Gaudi::Units::cm,
					   (dbManager->GetEnvZLength())/2.0 * Gaudi::Units::cm,
					   0.0 * Gaudi::Units::deg,
					   NumberOfMod*deltaPhi*Gaudi::Units::deg);

      GeoTubs* itcMotherNeg = GeneralMother;
      GeoLogVol* lvITCMotherNeg = new GeoLogVol("ITC",itcMotherNeg,matAir);
      pvITCMotherNeg = new GeoFullPhysVol(lvITCMotherNeg);

      (*m_log) << MSG::DEBUG << "Negative ITC envelope parameters: " 
          << " length=" << (dbManager->GetEnvZLength()) << " cm"
          << " Rmin=" << (dbManager->GetEnvRin()) << " cm"
          << " Rmax=" << (dbManager->GetEnvRout()) << " cm"
          << " nMod=" << NumberOfMod
          << endmsg;
    }

   
   //Envelope 2-level has been made (it is tube); begin loop over modules
  
    // for(int ModCounter = 0; ModCounter < NumberOfMod; ModCounter++){
    // simple hack - we know that all modules are identical, 
    // let's put all of them with GeoSerialTransfomer, 
    // i.e. execute this loop only once
    for(int ModCounter = 0; ModCounter < NumberOfMod; ModCounter+=NumberOfMod){
      dbManager->SetCurrentModuleByIndex(ModCounter);
      int ModType = dbManager->GetModType();
      int ModPositionNumber = dbManager->GetModNumber();
      (*m_log) << MSG::DEBUG 
          << "ModCounter is " << ModCounter
          << " ModType is " << ModType
          << " ModPositionNumber is " << ModPositionNumber
          << endmsg;
          
      Variable varInd;
      GENFUNCTION phiInd = deltaPhi*(varInd + ModCounter + 0.5) * Gaudi::Units::deg; 


      //------------------- BARREL BLOCKS -------------------------------------
     
      if( EnvType == 1 || EnvType == 0 ) { // normal barrel module or module zero
        dbManager->SetCurrentSectionByNumber(ModType);	  
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
       
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - (dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) - dbManager->TILBdzmast()))/(2.*(2.*dbManager->TILBnperiod() - 1));
       
        GeoTrd* barrelModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                                thicknessWedgeMother/2.,
                                                dy1WedgeMother,
                                                dy2WedgeMother,
                                                heightWedgeMother/2.);
       
        GeoLogVol* lvBarrelModuleMother = new GeoLogVol("BarrelModule",barrelModuleMother,matAir);
        GeoPhysVol* pvBarrelModuleMother = new GeoPhysVol(lvBarrelModuleMother);
       
        //Fill the section
        sectionBuilder->fillSection(pvBarrelModuleMother,
                                    1,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi); 
       
        TRANSFUNCTION xfBarrelModuleMother = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

        GeoSerialTransformer* stBarrelModuleMother = new GeoSerialTransformer(pvBarrelModuleMother,
                                                                              &xfBarrelModuleMother,
                                                                              NumberOfMod); // all modules at once
        pvBarrelMother->add(new GeoSerialIdentifier(ModPositionNumber));
        pvBarrelMother->add(stBarrelModuleMother);

       
        //--------------------------BARREL FINGERS MAKING------------------------------
       
        dbManager->SetCurrentTifg(2);  //at the testbeam put "small" barrel finger - the same as ext.barrel one 
        //zEndSection = dbManager->TILBzoffset() + dbManager->TILBdzmodul()/2.;

        // Trd - one finger mother

        thicknessWedgeMother = dbManager->TIFGdz() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
       
        GeoTrd* fingerModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                                thicknessWedgeMother/2.,
                                                dy1WedgeMother,
                                                dy2WedgeMother,
                                                heightWedgeMother/2.);
       
        GeoLogVol* lvFingerModuleMother = new GeoLogVol("FingerModule",fingerModuleMother,matAir);
        GeoPhysVol* pvFingerModuleMother = new GeoPhysVol(lvFingerModuleMother);
       
        // Fill the section
        sectionBuilder->fillFinger(pvFingerModuleMother, 1,
                                   dbManager->TILErmax(),
                                   dbManager->TILBrmax(),
                                   deltaPhi,
                                   m_testbeamGeometry,
                                   ModuleNcp,
                                   thicknessWedgeMother*(1./Gaudi::Units::cm));
       
        // --- Position N modules inside mother (positive/negative) -----
        TRANSFUNCTION xfFingerModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
        
        GeoSerialTransformer* stFingerModuleMotherPos = new GeoSerialTransformer(pvFingerModuleMother,
                                                                                 &xfFingerModuleMotherPos,
                                                                                 NumberOfMod); // all modules at once
        pvFingerMotherPos->add(new GeoSerialIdentifier(ModPositionNumber));
        pvFingerMotherPos->add(stFingerModuleMotherPos);
       
        TRANSFUNCTION xfFingerModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

        GeoSerialTransformer* stFingerModuleMotherNeg = new GeoSerialTransformer(pvFingerModuleMother,
                                                                                 &xfFingerModuleMotherNeg,
                                                                                 NumberOfMod); // all modules at once
        pvFingerMotherNeg->add(new GeoSerialIdentifier(ModPositionNumber));
        pvFingerMotherNeg->add(stFingerModuleMotherNeg);
      }

     
      //------------------- EBARREL BLOCKS POS --------------------------------

      if((ModType == 2)&&(EnvType == 3)){
        dbManager->SetCurrentSectionByNumber(ModType);
        // Trd - module mother 
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
        
        GeoTrd* ebarrelModuleMotherPos = new GeoTrd(thicknessWedgeMother/2.,
                                                    thicknessWedgeMother/2.,
                                                    dy1WedgeMother,
                                                    dy2WedgeMother,
                                                    heightWedgeMother/2.);
        
        GeoLogVol* lvEBarrelModuleMotherPos = new GeoLogVol("EBarrelModule",ebarrelModuleMotherPos,matAir);
        GeoPhysVol* pvEBarrelModuleMotherPos = new GeoPhysVol(lvEBarrelModuleMotherPos);
        
        // Fill the section
        sectionBuilder->fillSection(pvEBarrelModuleMotherPos,
                                    2,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi);
             
        TRANSFUNCTION xfEBarrelModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
        
        GeoSerialTransformer* stEBarrelModuleMotherPos = new GeoSerialTransformer(pvEBarrelModuleMotherPos,
                                                                                  &xfEBarrelModuleMotherPos,
                                                                                  NumberOfMod); // all modules at once
        pvEBarrelMotherPos->add(new GeoSerialIdentifier(ModPositionNumber));
        pvEBarrelMotherPos->add(stEBarrelModuleMotherPos);
  
  
        //--------------------------EBARREL FINGERS MAKING------------------------------
  
        dbManager->SetCurrentTifg(2);  //barrel efinger (small)
        
        // Trd - one finger mother
        thicknessWedgeMother = dbManager->TIFGdz() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        GeoTrd* efingerModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                                 thicknessWedgeMother/2.,
                                                 dy1WedgeMother,
                                                 dy2WedgeMother,
                                                 heightWedgeMother/2.);
        
        GeoLogVol* lvEFingerModuleMother = new GeoLogVol("EFingerModule",efingerModuleMother,matAir);
        GeoPhysVol* pvEFingerModuleMother = new GeoPhysVol(lvEFingerModuleMother);
        
        // Fill the section
        sectionBuilder->fillFinger(pvEFingerModuleMother,
                                   2,
                                   dbManager->TILErmax(),
                                   dbManager->TILBrmax(),
                                   deltaPhi,
                                   m_testbeamGeometry);
        
        // --- Position N modules inside mother (positive/negative) -----
  
        TRANSFUNCTION xfEFingerModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
        
        GeoSerialTransformer* stEFingerModuleMotherPos = new GeoSerialTransformer(pvEFingerModuleMother,
                                                                                  &xfEFingerModuleMotherPos,
                                                                                  NumberOfMod); // all modules at once
        pvEFingerMotherPos->add(new GeoSerialIdentifier(ModPositionNumber));
        pvEFingerMotherPos->add(stEFingerModuleMotherPos);
      }
      
      
      //------------------- EBARREL BLOCKS NEG --------------------------------
  
      if((ModType == 2)&&(EnvType == 2)){
        dbManager->SetCurrentSectionByNumber(ModType);
        // Trd - module mother 
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
  
        GeoTrd* ebarrelModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2.,
                                                    thicknessWedgeMother/2.,
                                                    dy1WedgeMother,
                                                    dy2WedgeMother,
                                                    heightWedgeMother/2.);
        
        GeoLogVol* lvEBarrelModuleMotherNeg = new GeoLogVol("EBarrelModule",ebarrelModuleMotherNeg,matAir);
        GeoPhysVol* pvEBarrelModuleMotherNeg = new GeoPhysVol(lvEBarrelModuleMotherNeg);
        
        // Fill the section
        sectionBuilder->fillSection(pvEBarrelModuleMotherNeg,
                                    2,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi);
      
        TRANSFUNCTION xfEBarrelModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+dbManager->TILBrminimal())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

        GeoSerialTransformer* stEBarrelModuleMotherNeg = new GeoSerialTransformer(pvEBarrelModuleMotherNeg,
                                                                                  &xfEBarrelModuleMotherNeg,
                                                                                  NumberOfMod); // all modules at once
        pvEBarrelMotherNeg->add(new GeoSerialIdentifier(ModPositionNumber));
        pvEBarrelMotherNeg->add(stEBarrelModuleMotherNeg);
        
        
        //--------------------------EBARREL FINGERS MAKING------------------------------
        
        dbManager->SetCurrentTifg(2);  //barrel efinger (small)
        
        //zEndSection = extOffset + dbManager->TILBdzmodul()/2. + dbManager->TILEzshift();
        // Trd - one finger mother
        thicknessWedgeMother = dbManager->TIFGdz() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        GeoTrd* efingerModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                                 thicknessWedgeMother/2.,
                                                 dy1WedgeMother,
                                                 dy2WedgeMother,
                                                 heightWedgeMother/2.);
        
        GeoLogVol* lvEFingerModuleMother = new GeoLogVol("EFingerModule",efingerModuleMother,matAir);
        GeoPhysVol* pvEFingerModuleMother = new GeoPhysVol(lvEFingerModuleMother);
        
        // Fill the section
        sectionBuilder->fillFinger(pvEFingerModuleMother,
                                   2,
                                   dbManager->TILErmax(),
                                   dbManager->TILBrmax(),
                                   deltaPhi,
                                   m_testbeamGeometry);
          
        TRANSFUNCTION xfEFingerModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILErmax()+dbManager->TILBrmax())/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);

        GeoSerialTransformer* stEFingerModuleMotherNeg = new GeoSerialTransformer(pvEFingerModuleMother,
                                                                                  &xfEFingerModuleMotherNeg,
                                                                                  NumberOfMod); // all modules at once
        pvEFingerMotherNeg->add(new GeoSerialIdentifier(ModPositionNumber));
        pvEFingerMotherNeg->add(stEFingerModuleMotherNeg);
      }
        
    
      //------------------- ITC BLOCKS POS --------------------------------
      
      if(((ModType == 6050403)||(ModType == 403))&&(EnvType == 5)){
          
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
        rMinITC = dbManager->TILBrminimal();
        dzITC2 = dbManager->TILBdzmodul();
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
        
        // Common mother for ITC1/2 modules
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - rMinITC) * Gaudi::Units::cm;
        dy1WedgeMother = rMinITC * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        GeoTrd* itcModuleMotherPos = new GeoTrd(thicknessWedgeMother/2.,
                                                thicknessWedgeMother/2.,
                                                dy1WedgeMother,
                                                dy2WedgeMother,
                                                heightWedgeMother/2.);  
        
        GeoLogVol* lvITCModuleMotherPos = new GeoLogVol("ITCModule",itcModuleMotherPos,matAir);
        GeoPhysVol* pvITCModuleMotherPos = new GeoPhysVol(lvITCModuleMotherPos);
        
        //Mother volume for ITC1
        //In plug1 it's necessary to produce GeoShapeUnion for mother volume that is composed by two parts:
        // 1. Mother for absorber and girder
        // 2. Mother for frontplate (since it's short)
        
        //First submother
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrmin()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
        
        GeoTrd* plug1SubMother = new GeoTrd(thicknessWedgeMother/2.,
                                            thicknessWedgeMother/2.,
                                            dy1WedgeMother,
                                            dy2WedgeMother,
                                            heightWedgeMother/2.);
        
        //Second submother
        thicknessWedgeMother = (dbManager->TILBdzmodul() - dzITC2) * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmin() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        GeoTrd* plug2SubMother = new GeoTrd(thicknessWedgeMother/2.,
                                            thicknessWedgeMother/2.,
                                            dy1WedgeMother,
                                            dy2WedgeMother,
                                            heightWedgeMother/2.);
        
        GeoTrf::Translate3D plug1SubOffset(-dzITC2*Gaudi::Units::cm/2.,
                                      0.,
                                      (dbManager->TILBrminimal()-dbManager->TILBrmaximal())*Gaudi::Units::cm/2.);
        
        const GeoShapeUnion& plug1ModuleMother = plug1SubMother->add(*plug2SubMother<<plug1SubOffset);
        GeoLogVol* lvPlug1ModuleMother = new GeoLogVol("Plug1Module",&plug1ModuleMother,matAir);
        GeoPhysVol* pvPlug1ModuleMother = new GeoPhysVol(lvPlug1ModuleMother);
        
        // Fill the section
        sectionBuilder->fillSection(pvPlug1ModuleMother, 3,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi,
                                    ModuleNcp,
                                    dzITC2);
        
        GeoTransform* tfPlug1ModuleMother = new GeoTransform(GeoTrf::Translate3D(0.,
                                                                            0.,
                                                                            (dbManager->TILBrmin()-rMinITC)*Gaudi::Units::cm/2.));
        
        
        pvITCModuleMotherPos->add(tfPlug1ModuleMother);
        pvITCModuleMotherPos->add(pvPlug1ModuleMother);
        	 
        //Mother volume for ITC2
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - ((dbManager->TILBnperiod()-1)*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) + dbManager->TILBdzspac()))/(4.*(dbManager->TILBnperiod() - 1));
        
        GeoTrd* plug2ModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                               thicknessWedgeMother/2.,
                                               dy1WedgeMother,
                                               dy2WedgeMother,
                                               heightWedgeMother/2.);
        
        GeoLogVol* lvPlug2ModuleMother = new GeoLogVol("Plug2Module",plug2ModuleMother,matAir);
        GeoPhysVol* pvPlug2ModuleMother = new GeoPhysVol(lvPlug2ModuleMother);
        
        // Fill the section
        sectionBuilder->fillSection(pvPlug2ModuleMother,
                                    4,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi);
        
        
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
        GeoTransform* tfPlug2ModuleMother = new GeoTransform(GeoTrf::Translate3D((dbManager->TILBdzmodul() - dzITC2)*Gaudi::Units::cm/2.,
                                                                            0.,
                                                                            (dbManager->TILBrmin() - dbManager->TILBrmaximal())*Gaudi::Units::cm/2.));
        pvITCModuleMotherPos->add(tfPlug2ModuleMother);
        pvITCModuleMotherPos->add(pvPlug2ModuleMother);
        
        TRANSFUNCTION xfITCModuleMotherPos = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+rMinITC)/2.*Gaudi::Units::cm)*GeoTrf::RotateX3D(180*Gaudi::Units::deg)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
        
        GeoSerialTransformer* stITCModuleMotherPos = new GeoSerialTransformer(pvITCModuleMotherPos,
                                                                              &xfITCModuleMotherPos,
                                                                              NumberOfMod); // all modules at once
        pvITCMotherPos->add(new GeoSerialIdentifier(ModPositionNumber));
        pvITCMotherPos->add(stITCModuleMotherPos);
      }


      //------------------- ITC BLOCKS NEG --------------------------------
       
      if(((ModType == 6050403)||(ModType == 403))&&(EnvType == 4)){
          
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
        rMinITC = dbManager->TILBrminimal();
        dzITC2 = dbManager->TILBdzmodul();
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
          
        // Common mother for ITC1/2 modules
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - rMinITC) * Gaudi::Units::cm;
        dy1WedgeMother = rMinITC * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
          
        GeoTrd* itcModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2.,
                                                thicknessWedgeMother/2.,
                                                dy1WedgeMother,
                                                dy2WedgeMother,
                                                heightWedgeMother/2.);  
        
        GeoLogVol* lvITCModuleMotherNeg = new GeoLogVol("ITCModule",itcModuleMotherNeg,matAir);
        GeoPhysVol* pvITCModuleMotherNeg = new GeoPhysVol(lvITCModuleMotherNeg);
          
        //Mother volume for ITC1
        //In plug1 it's necessary to produce GeoShapeUnion for mother volume that is composed by two parts:
        // 1. Mother for absorber and girder
        // 2. Mother for frontplate (since it's short)
          
        //First submother
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrmin()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
          
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
          
        GeoTrd* plug1SubMother = new GeoTrd(thicknessWedgeMother/2.,
                                            thicknessWedgeMother/2.,
                                            dy1WedgeMother,
                                            dy2WedgeMother,
                                            heightWedgeMother/2.);
          
        //Second submother
        thicknessWedgeMother = (dbManager->TILBdzmodul() - dzITC2) * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmin() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        GeoTrd* plug2SubMother = new GeoTrd(thicknessWedgeMother/2.,
                                            thicknessWedgeMother/2.,
                                            dy1WedgeMother,
                                            dy2WedgeMother,
                                            heightWedgeMother/2.);
          
        GeoTrf::Translate3D plug1SubOffset(-dzITC2*Gaudi::Units::cm/2.,
                                      0.,
                                      (dbManager->TILBrminimal()-dbManager->TILBrmaximal())*Gaudi::Units::cm/2.);
          
        const GeoShapeUnion& plug1ModuleMother = plug1SubMother->add(*plug2SubMother<<plug1SubOffset);
        GeoLogVol* lvPlug1ModuleMother = new GeoLogVol("Plug1Module",&plug1ModuleMother,matAir);
        GeoPhysVol* pvPlug1ModuleMother = new GeoPhysVol(lvPlug1ModuleMother);
          
        // Fill the section
        sectionBuilder->fillSection(pvPlug1ModuleMother,
                                    3,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi,
                                    ModuleNcp,
                                    dzITC2);
        
        GeoTransform* tfPlug1ModuleMother = new GeoTransform(GeoTrf::Translate3D(0.,
                                                                            0.,
                                                                            (dbManager->TILBrmin()-rMinITC)*Gaudi::Units::cm/2.));
          
          
        pvITCModuleMotherNeg->add(tfPlug1ModuleMother);
        pvITCModuleMotherNeg->add(pvPlug1ModuleMother);
          	 
        //Mother volume for ITC2
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
        thicknessWedgeMother = dbManager->TILBdzmodul() * Gaudi::Units::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * Gaudi::Units::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2.*Gaudi::Units::deg) * Gaudi::Units::cm;
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() - ((dbManager->TILBnperiod()-1)*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) + dbManager->TILBdzspac()))/(4.*(dbManager->TILBnperiod() - 1));
          
        GeoTrd* plug2ModuleMother = new GeoTrd(thicknessWedgeMother/2.,
                                               thicknessWedgeMother/2.,
                                               dy1WedgeMother,
                                               dy2WedgeMother,
                                               heightWedgeMother/2.);
          
        GeoLogVol* lvPlug2ModuleMother = new GeoLogVol("Plug2Module",plug2ModuleMother,matAir);
        GeoPhysVol* pvPlug2ModuleMother = new GeoPhysVol(lvPlug2ModuleMother);
          
        // Fill the section
        sectionBuilder->fillSection(pvPlug2ModuleMother,
                                    4,
                                    dbManager->TILBrmaximal(),
                                    dbManager->TILBrminimal(),
                                    dzGlue,
                                    deltaPhi);
        
          
        dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
        GeoTransform* tfPlug2ModuleMother = new GeoTransform(GeoTrf::Translate3D((dbManager->TILBdzmodul() - dzITC2)*Gaudi::Units::cm/2.,
                                                                            0.,
                                                                            (dbManager->TILBrmin() - dbManager->TILBrmaximal())*Gaudi::Units::cm/2.));
        pvITCModuleMotherNeg->add(tfPlug2ModuleMother);
        pvITCModuleMotherNeg->add(pvPlug2ModuleMother);
          
        TRANSFUNCTION xfITCModuleMotherNeg = Pow(GeoTrf::RotateZ3D(1.0),phiInd)*GeoTrf::TranslateX3D((dbManager->TILBrmaximal()+rMinITC)/2.*Gaudi::Units::cm)*GeoTrf::RotateY3D(90*Gaudi::Units::deg);
          
        GeoSerialTransformer* stITCModuleMotherNeg = new GeoSerialTransformer(pvITCModuleMotherNeg,
                                                                              &xfITCModuleMotherNeg,
                                                                              NumberOfMod); // all modules at once
        pvITCMotherNeg->add(new GeoSerialIdentifier(ModPositionNumber));
        pvITCMotherNeg->add(stITCModuleMotherNeg);
      }  
    }
 
    double ztrans=0.0;

    if(EnvType == 1 || EnvType == 0) { 
      GeoTransform* tfBarrelMother;
      // consider 3 options - with/without ext.barrels and take into account DZ correction
      if((IfEBPos&&IfEBNeg)||((!IfEBPos)&&(!IfEBNeg))) {
        ztrans = dbManager->GetEnvDZ();
      } 
      else if(IfEBPos&&(!IfEBNeg)) {
        ztrans = dbManager->GetEnvDZ() + (-tileTBEnvThickness/2. + (dbManager->GetEnvZLength())/2.);
      } 
      else if((!IfEBPos)&&IfEBNeg) {
        ztrans = dbManager->GetEnvDZ() + ( tileTBEnvThickness/2. - (dbManager->GetEnvZLength())/2.);
      }
      else {
        ztrans = 0;
      }
      tfBarrelMother = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D((dbManager->GetEnvDPhi())*Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning barrel with translation " << ztrans << " cm" << endmsg;
      GeoNameTag* ntBarrelModuleMother = new GeoNameTag("Barrel"); 

      pvTileTBEnv->add(tfBarrelMother);
      pvTileTBEnv->add(ntBarrelModuleMother);
      pvTileTBEnv->add(pvBarrelMother);
      
      GeoTransform* tfFingerMotherPos;

      if((IfEBPos&&IfEBNeg)||((!IfEBPos)&&(!IfEBNeg))) {
        ztrans = dbManager->GetEnvDZ() + (dbManager->GetEnvZLength()/2. - BFingerLength/2.);
      }
      else if((!IfEBPos)&&IfEBNeg) {
        ztrans = dbManager->GetEnvDZ() + (tileTBEnvThickness/2. - BFingerLength/2.);
      }
      else if(IfEBPos&&(!IfEBNeg)) {
        ztrans = dbManager->GetEnvDZ() + (-tileTBEnvThickness/2. + dbManager->GetEnvZLength() - BFingerLength/2.);
      }
      else {
        ztrans = 0;
      }
      tfFingerMotherPos = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D((dbManager->GetEnvDPhi())*Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning positive barrel finger with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;
      GeoNameTag* ntFingerMotherPos = new GeoNameTag("TileFingerPos");
      pvTileTBEnv->add(tfFingerMotherPos);
      pvTileTBEnv->add(ntFingerMotherPos);
      pvTileTBEnv->add(pvFingerMotherPos);

      GeoTransform* tfFingerMotherNeg;

      if((IfEBPos&&IfEBNeg)||((!IfEBPos)&&(!IfEBNeg))) {
        ztrans = dbManager->GetEnvDZ() + (-dbManager->GetEnvZLength()/2. + BFingerLength/2.);
      }
      else if(IfEBPos&&(!IfEBNeg)) {
        ztrans = dbManager->GetEnvDZ() + (-tileTBEnvThickness/2. + BFingerLength/2.);
      }
      else if((!IfEBPos)&&IfEBNeg) {
        ztrans = dbManager->GetEnvDZ() + ( tileTBEnvThickness/2. - dbManager->GetEnvZLength() + BFingerLength/2.);
      }
      else {
        ztrans = 0;
      }
      tfFingerMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D((dbManager->GetEnvDPhi())*Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning negative barrel finger with translation " << ztrans 
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;
      GeoNameTag* ntFingerMotherNeg = new GeoNameTag("TileFingerNeg");
      pvTileTBEnv->add(tfFingerMotherNeg);
      pvTileTBEnv->add(ntFingerMotherNeg);
      pvTileTBEnv->add(pvFingerMotherNeg);

    }


    if(EnvType == 3) { // positive ext.barrel is always at positive boundary, after finger
      ztrans = (tileTBEnvThickness/2. - dbManager->GetEnvZLength()/2. - EBFingerLength/2.);
      GeoTransform* tfEBarrelMotherPos = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning positive ext.barrel with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;
      
      GeoNameTag* ntEBarrelMotherPos = new GeoNameTag("EBarrelPos");
      pvTileTBEnv->add(tfEBarrelMotherPos);
      pvTileTBEnv->add(ntEBarrelMotherPos);
      pvTileTBEnv->add(pvEBarrelMotherPos);
      
      ztrans = (tileTBEnvThickness/2. - EBFingerLength/2.);
      GeoTransform* tfEFingerMotherPos = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning positive ext.barrel finger with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;

      GeoNameTag* ntEFingerMotherPos = new GeoNameTag("TileEFingerPos");
      pvTileTBEnv->add(tfEFingerMotherPos);
      pvTileTBEnv->add(ntEFingerMotherPos);
      pvTileTBEnv->add(pvEFingerMotherPos);

    }
   
    if(EnvType == 2) { // negative ext.barrel is always at negative boundary, after finger
      ztrans = (-tileTBEnvThickness/2. + dbManager->GetEnvZLength()/2. + EBFingerLength/2.);
      GeoTransform* tfEBarrelMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning negative ext.barrel with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;
      
      GeoNameTag* ntEBarrelMotherNeg = new GeoNameTag("EBarrelNeg");
      pvTileTBEnv->add(tfEBarrelMotherNeg);
      pvTileTBEnv->add(ntEBarrelMotherNeg);
      pvTileTBEnv->add(pvEBarrelMotherNeg);
      
      ztrans = (-tileTBEnvThickness/2. + EBFingerLength/2.);
      GeoTransform* tfEFingerMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning negative ext.barrel finger with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;

      GeoNameTag* ntEFingerMotherNeg = new GeoNameTag("TileEFingerNeg");
      pvTileTBEnv->add(tfEFingerMotherNeg);
      pvTileTBEnv->add(ntEFingerMotherNeg);
      pvTileTBEnv->add(pvEFingerMotherNeg);

    }

    if(EnvType == 5) { // positive ITC attached to positive ext.barrel
      ztrans = (tileTBEnvThickness/2. - ZLengthEBarrelPos - dbManager->GetEnvZLength()/2.);
      GeoTransform* tfITCMotherPos = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning positive ITC with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;

      GeoNameTag* ntITCMotherPos = new GeoNameTag("ITCPos");
      pvTileTBEnv->add(tfITCMotherPos);
      pvTileTBEnv->add(ntITCMotherPos);
      pvTileTBEnv->add(pvITCMotherPos);
    }

    if(EnvType == 4) { // negative ITC attached to negative ext.barrel
      ztrans = (-tileTBEnvThickness/2. + ZLengthEBarrelNeg + dbManager->GetEnvZLength()/2.);
      GeoTransform* tfITCMotherNeg = new GeoTransform(GeoTrf::TranslateZ3D(ztrans*Gaudi::Units::cm) * GeoTrf::RotateZ3D(dbManager->GetEnvDPhi() * Gaudi::Units::deg));
      (*m_log) << MSG::DEBUG << "Positioning negative ITC with translation " << ztrans
          << " cm and rotation " << dbManager->GetEnvDPhi() << " deg " << endmsg;

      GeoNameTag* ntITCMotherNeg = new GeoNameTag("ITCNeg");
      pvTileTBEnv->add(tfITCMotherNeg);
      pvTileTBEnv->add(ntITCMotherNeg);
      pvTileTBEnv->add(pvITCMotherNeg);
    }
  }

  // creating Descriptiors and CaloDetDescrElements 

  int nModulesInSection[6] = {0,0,0,0,0,0};
  float zShiftInSection[6] = {0.0,0.0,0.0,0.0,0.0,0.0,};
  
  NumberOfEnv = dbManager->GetNumberOfEnv();
  for(int EnvCounter = 0; EnvCounter < NumberOfEnv; ++EnvCounter){ //Loop over Envelopes
    dbManager->SetCurrentEnvByIndex(EnvCounter);
    int EnvType = dbManager->GetEnvType();
    int NumberOfMod = dbManager->GetEnvNModules();
    float Zshift = dbManager->GetEnvZShift() * Gaudi::Units::cm;
    (*m_log) << MSG::DEBUG 
        << "EnvCounter is " << EnvCounter
        << " EnvType is " << EnvType
        << " Nmodules is " << NumberOfMod
        << " Zshift is " << Zshift*(1./Gaudi::Units::cm) << " cm"
        << endmsg;

    if(EnvType == 1 || EnvType == 0) { // central barrel
      nModulesInSection[0] = nModulesInSection[1] = NumberOfMod;
      zShiftInSection[0] = zShiftInSection[1] = Zshift;
    } else if(EnvType < 6) {
      nModulesInSection[EnvType] = NumberOfMod;
      zShiftInSection[EnvType] = Zshift;
    }
  }
  
  TileCablingService * cabling = TileCablingService::getInstance(); 
  const TileID* tileID = m_detectorManager->get_id();

  int dete[6] = {TILE_REGION_CENTRAL,TILE_REGION_CENTRAL,
                 TILE_REGION_EXTENDED,TILE_REGION_EXTENDED,
                 TILE_REGION_GAP,TILE_REGION_GAP};
  int side[6] = {0,1,0,1,0,1};

  for (int ii=0; ii<6; ++ii) {

    if (ii%2 == 0) {
      sectionBuilder->computeCellDim(m_detectorManager,
                                     dete[ii],
                                     m_addPlatesToCellVolume,
                                     zShiftInSection[ii+1], // zShiftPos
                                     zShiftInSection[ii]);  // zShiftNeg
    }
    
    TileDetDescriptor* descriptor = new TileDetDescriptor();
    sectionBuilder->fillDescriptor(descriptor,
                                   dete[ii],
                                   side[ii],
                                   m_testbeamGeometry,      // TB
                                   m_addPlatesToCellVolume, // add front/end plates to cell volume
                                   nModulesInSection[ii],   // 0-3 modules
                                   zShiftInSection[ii]);    // Z-shift
  
    Identifier idRegion = tileID->region_id(ii);
    descriptor->set(idRegion);
    m_detectorManager->add(descriptor); 
    m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));

    cabling->setConnected(dete[ii],side[ii],0,nModulesInSection[ii]);
  }

  
  // Set TileTB as tree top -----------
  GeoNameTag* nTag = new GeoNameTag("Tile"); 
  world->add(nTag);
  world->add(pvTileTBEnv);
  m_detectorManager->addTreeTop(pvTileTBEnv);

  delete sectionBuilder;
}

