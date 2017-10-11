/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HECConstructionH62004.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoBox.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoVFullPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoShapeUnion.h"  
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include <string>
#include <cmath>
#include <iostream>



using namespace Genfun;
using namespace GeoXF;



LArGeo::HECConstructionH62004::HECConstructionH62004()
   :m_h6Phys(0)
{
 // access source of detector parameters
  // m_parameters = LArGeo::VDetectorParameters::GetInstance();
}


LArGeo::HECConstructionH62004::~HECConstructionH62004()
{
}


GeoVFullPhysVol* LArGeo::HECConstructionH62004::GetEnvelope()
{

  if (m_h6Phys) return (m_h6Phys);


  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECConstruction, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "HECConstruction");  
  log << MSG::INFO;  
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of HEC TB GeoModel definition      +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;


  StoreGateSvc *detectorStore;
  if (svcLocator->service("DetectorStore", detectorStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECConstruction, cannot access DetectorStore");
  }


  StatusCode sc;
  IRDBAccessSvc *pAccessSvc;
  sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }


  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
    return NULL; 
  } 

  
   
  //-----------------------------------------------------------------------------------//  
  // Get the materials that we shall use.                                              //  
  // ----------------------------------------------------------------------------------//  
  
  //const GeoMaterial *air        = materialManager->getMaterial("std::Air");

  GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in HECConstruction, std::LiquidArgon is not found.");
  
  GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in HECConstruction, std::Iron is not found.");
  
  GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in HECConstruction, std::Copper is not found.");
  
  GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in HECConstruction, std::Kapton is not found.");
  

  DecodeVersionKey larVersion("LAr");
  std::string detectorKey = larVersion.tag();
  std::string detectorNode = larVersion.node();

  IRDBRecordset_ptr hadronicEndcap       = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  IRDBRecordset_ptr hecLongitudinalBlock = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 
  if(!hadronicEndcap) throw std::runtime_error("Error in HECConstruction:  hadronicEendcap not found");
  if(!hecLongitudinalBlock) throw std::runtime_error("Error in HECConstruction:  hecLongitudinalBlock not found");
  
  int numberZplane = 6;
  int depthNumber = 7;
  double depthSize[7];
  double kaptonPosition[3];
  double kaptonWidth[3];
  double tieRodPositionX[4];
  double tieRodPositionY[4];
  double tieRodDiameter[2];
  double spacerDiameter[2];
  double firstAbsorber[7];
  int gapNumber[7];

  double zCoordinate[6],innerRadius[6],outerRadius[6];

  double absorberPosY = 1.02*CLHEP::cm;            
  double PosYcorr = 0.005*CLHEP::cm;            

  const double moduleNumber   = (*hadronicEndcap)[0]->getInt("NSCT"); // 32 Modules
  unsigned int TBmoduleNumber = 8;
  double moduleRinner1  = (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*CLHEP::cm; // 37.2*CLHEP::cm Inner Radius 
  double moduleRinner2  = (*hecLongitudinalBlock)[1]->getDouble("BLRMN")*CLHEP::cm; // 47.5*CLHEP::cm
  //double moduleRouter   = (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*CLHEP::cm; //203.*CLHEP::cm Outer Radius
  double copperPad      = (*hadronicEndcap)[0]->getDouble("COPPER")*CLHEP::cm; // 0.003.*CLHEP::cm 
  double gapSize        = (*hadronicEndcap)[0]->getDouble("LARG")*CLHEP::cm; // 8.5*CLHEP::mm
  double betweenWheel   = (*hadronicEndcap)[0]->getDouble("GAPWHL")*CLHEP::cm-0.001*CLHEP::cm; //40.5*CLHEP::mm
  int indexloop,index;
  for (indexloop=0; indexloop < depthNumber; ++indexloop){
    depthSize[indexloop]    = (*hecLongitudinalBlock)[indexloop]->getDouble("BLDPTH")*CLHEP::cm; 
    firstAbsorber[indexloop]= (*hecLongitudinalBlock)[indexloop]->getDouble("PLATE0")*CLHEP::cm; 
    gapNumber[indexloop]    = (int) (*hecLongitudinalBlock)[indexloop]->getDouble("BLMOD"); // 4 or 8 
  }

  std::string sidx[4]={"_0","_1","_2","_3"};

  for (indexloop=0; indexloop < 3; ++indexloop){
    kaptonPosition[indexloop] = (*hadronicEndcap)[0]->getDouble("KPTPOS" + sidx[indexloop])*CLHEP::cm; 
    kaptonWidth[indexloop]    = (*hadronicEndcap)[0]->getDouble("KPTWID" + sidx[indexloop])*CLHEP::cm; 
  }

  for (indexloop=0; indexloop < 4; ++indexloop){
    tieRodPositionX[indexloop] = (*hadronicEndcap)[0]->getDouble("RODPOSX" + sidx[indexloop])*CLHEP::cm; 
    tieRodPositionY[indexloop] = (*hadronicEndcap)[0]->getDouble("RODPOSR" + sidx[indexloop])*CLHEP::cm; 
  }

  for (indexloop=0; indexloop < 2; ++indexloop){
    tieRodDiameter[indexloop] = (*hadronicEndcap)[0]->getDouble("RODDIM" + sidx[indexloop])*CLHEP::cm; 
    spacerDiameter[indexloop] = (*hadronicEndcap)[0]->getDouble("SPCDIM" + sidx[indexloop])*CLHEP::cm; 
  }

  double absorberZ1 = (*hadronicEndcap)[0]->getDouble("PLATE_0")*CLHEP::cm; // 2.5*CLHEP::cm;
  double absorberZ2 = (*hadronicEndcap)[0]->getDouble("PLATE_1")*CLHEP::cm; //5.0*CLHEP::cm;

  const double moduleDeltaPhi   = 2*M_PI/moduleNumber; //11.25*CLHEP::deg;  
  double modulePhistart = -moduleDeltaPhi/2.; 
  //double modulePhistart   = 0.0*CLHEP::deg;
  zCoordinate[0]=0.0*CLHEP::cm; 
  zCoordinate[1]=depthSize[0]; //28.05*CLHEP::cm; 
  zCoordinate[2]=depthSize[0]+0.001*CLHEP::cm; //28.051*CLHEP::cm; 
  zCoordinate[3]=zCoordinate[2]+depthSize[1]+depthSize[2]+betweenWheel/2;
  zCoordinate[4]=zCoordinate[3]+depthSize[3]+depthSize[4]+betweenWheel/2;
  zCoordinate[5]=181.8*CLHEP::cm;
  innerRadius[0]=moduleRinner1;
  innerRadius[1]=moduleRinner1;
  for (index=2; index<numberZplane;++index) {innerRadius[index]=moduleRinner2;}
  for (index=0; index<numberZplane;++index) {outerRadius[index]=innerRadius[0] + 78.7*CLHEP::cm; } 


//----------------------------------------------------------------
//   HEC 
//----------------------------------------------------------------


  std::string hecName = "LAr::HEC::LiquidArgon";
  GeoPcon* solidHEC = new GeoPcon(M_PI / 2., TBmoduleNumber*moduleDeltaPhi);
  for (int i = 0; i < numberZplane - 1; ++i) { 
         solidHEC->addPlane(zCoordinate[i],innerRadius[i]-PosYcorr,outerRadius[i]);
  }	 
  const GeoLogVol   *logicHEC = new GeoLogVol(hecName, solidHEC , LAr);

  m_h6Phys   = new GeoFullPhysVol(logicHEC);
  
  std::string tag = std::string("HEC_POS");
  StatusCode status;
  
  StoredPhysVol *sPhysVol = new StoredPhysVol(m_h6Phys);
  status=detectorStore->record(sPhysVol,tag);
  if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());


// Depth
  GeoTubs* solidDepth[7];
  GeoLogVol* logiDepth[7];
  GeoPhysVol* physiDepth[7];

// Slice
  GeoTubs* solidSlice[3];
  GeoLogVol* logiSlice[3];
  GeoPhysVol* physiSlice[3];

// EstBoard
  GeoTubs* solidEstBoard;
  GeoLogVol* logiEstBoard;
  GeoPhysVol* physiEstBoard;

// PadBoard
  GeoTubs* solidPadBoard;
  GeoLogVol* logiPadBoard;
  GeoPhysVol* physiPadBoard;

// TieRod in the gap
  GeoTubs* solidTieRod[2];
  GeoLogVol* logiTieRod[2];
  GeoPhysVol* physiTieRod[2];
// TieRod in absorbers
  GeoTubs* solidAbsorberTieRod[2];
  GeoLogVol* logiAbsorberTieRod[2];
  GeoPhysVol* physiAbsorberTieRod[2];
// Absorber
  GeoTubs* solidAbsorber[3] ;
  GeoLogVol* logiAbsorber[3] ;
  GeoPhysVol* physiAbsorber[3] ;
// First Absorber
  GeoTubs* solidFirstAbsorber ;
  GeoLogVol* logiFirstAbsorber ;
  GeoPhysVol* physiFirstAbsorber ;

//----------------------------------------------------------------
//   HEC Module
//----------------------------------------------------------------

// Module         
  std::string moduleName = "LAr::HEC::Module";
  GeoPcon* solidModule = new GeoPcon(modulePhistart, moduleDeltaPhi);
  for (int i=0; i< numberZplane - 1; ++i){
     solidModule->addPlane(zCoordinate[i],innerRadius[i]-PosYcorr,outerRadius[i]);
  }
  GeoLogVol*  logicModule = new GeoLogVol(moduleName, solidModule , LAr);
  GeoPhysVol* physiModule = new GeoPhysVol(logicModule);
  {
     //----------------------------------------------------------------
    //   Place Module in HEC mother
    //----------------------------------------------------------------
    GeoSerialIdentifier  *sI = new GeoSerialIdentifier(9);
    Genfun::Variable    Index;
    Genfun::GENFUNCTION ModuleRotationAngle = M_PI/2. + moduleDeltaPhi/2. + moduleDeltaPhi*Index; 
    GeoXF::TRANSFUNCTION t = GeoXF::Pow(HepGeom::RotateZ3D(1.0),ModuleRotationAngle);
    GeoSerialTransformer *sT = new GeoSerialTransformer (physiModule,&t,TBmoduleNumber);
    m_h6Phys->add(sI);
    m_h6Phys->add(sT);
  }

//----------------------------------------------------------------
//    Sensitive slicegap 
//----------------------------------------------------------------
  int sliceCopyNo=0;
  std::string depthName = moduleName + "::Depth";
  std::string sliceName = depthName + "::Slice";
  double moduleRinner= moduleRinner1 ;
  int sliceNo;
  for( sliceNo=0; sliceNo<3; sliceNo++){
    if (sliceNo>0) moduleRinner = moduleRinner2;
    solidSlice[sliceNo] = new GeoTubs(moduleRinner-PosYcorr,outerRadius[0],gapSize/2.,modulePhistart,moduleDeltaPhi);                   
    logiSlice[sliceNo]  = new GeoLogVol(sliceName, solidSlice[sliceNo], LAr);
    physiSlice[sliceNo] = new GeoPhysVol(logiSlice[sliceNo]);
  }  

//----------------------------------------------------------------
//    Absorbers , the inner and outer Radius are smaller on 1.02*CLHEP::cm
//                but positionned in the center of depth. this alows
//                to have 2 CLHEP::mm gap between the copper plates of neighbor Modules 
//----------------------------------------------------------------
  std::string absorberName = depthName + "::Absorber";
  double absorberRinner1 = moduleRinner1-absorberPosY;
  double absorberRinner2 = moduleRinner2-absorberPosY;
  double absorberRouter  = outerRadius[0] - absorberPosY;
  solidAbsorber[0] = new GeoTubs(absorberRinner1,absorberRouter,absorberZ1/2.,
                                   modulePhistart,moduleDeltaPhi);                   
  solidAbsorber[1] = new GeoTubs(absorberRinner2,absorberRouter,absorberZ1/2.,
                                   modulePhistart,moduleDeltaPhi);                   
  solidAbsorber[2] = new GeoTubs(absorberRinner2,absorberRouter,absorberZ2/2.,
                                   modulePhistart,moduleDeltaPhi);                   
  int absorberNo;
  for (absorberNo=0;absorberNo<3;absorberNo++){
      logiAbsorber[absorberNo] = new GeoLogVol(absorberName, solidAbsorber[absorberNo], Copper);
      physiAbsorber[absorberNo] = new GeoPhysVol(logiAbsorber[absorberNo]);
  }
//----------------------------------------------------------------
//    First Absorbers in front of first and third samplings
//----------------------------------------------------------------
  std::string firstAbsorberName = depthName + "::FirstAbsorber";
  solidFirstAbsorber = new GeoTubs(moduleRinner1-absorberPosY,outerRadius[0]-absorberPosY,firstAbsorber[0]/2.,
                                     modulePhistart,moduleDeltaPhi);                   
  logiFirstAbsorber = new GeoLogVol(firstAbsorberName, solidFirstAbsorber, Copper);
  physiFirstAbsorber = new GeoPhysVol(logiFirstAbsorber);

//----------------------------------------------------------------
//   Place 7 depths in 1 Module of HEC
//   Only 5 depths in TB
//----------------------------------------------------------------
  double depthPositionZ = 0.;
  for(int indexDepth=0; indexDepth<5; indexDepth++){
      depthPositionZ +=depthSize[indexDepth]/2.;
      if (indexDepth==1) depthPositionZ +=0.001*CLHEP::cm;
      moduleRinner = moduleRinner2;
      if (indexDepth==0) moduleRinner = moduleRinner1; //for first depth
      //Absorber size and position
      double absorberSize = absorberZ2;
      if (indexDepth < 3) {
      absorberSize = absorberZ1; 
       }
      double absorberPositionZ=firstAbsorber[indexDepth]+
                             gapSize+absorberSize/2.0-depthSize[indexDepth]/2.0;
      // Now depth
      solidDepth[indexDepth]  = new GeoTubs(moduleRinner-PosYcorr,outerRadius[0], depthSize[indexDepth]/2.,
	                                    modulePhistart,moduleDeltaPhi);                   
      logiDepth[indexDepth]   = new GeoLogVol(depthName, solidDepth[indexDepth], LAr);
      physiDepth[indexDepth]  = new GeoPhysVol(logiDepth[indexDepth]);

      physiModule->add(new GeoIdentifierTag(100+indexDepth));
      physiModule->add(new GeoTransform(HepGeom::Translate3D(0,0,depthPositionZ)));
      physiModule->add(physiDepth[indexDepth]);
      depthPositionZ +=depthSize[indexDepth]/2.;
      if (indexDepth==2) depthPositionZ +=betweenWheel;
// Position of 8 (or 4)sensitive gaps and absorbers in 7 depths
      double slicePositionZ=firstAbsorber[indexDepth] + gapSize/2.0 - depthSize[indexDepth]/2.0;
      if (indexDepth>0) sliceCopyNo += gapNumber[indexDepth-1]; 
      if(indexDepth==0) {
	  sliceNo=0;
	  absorberNo=0;
      } else {
	  sliceNo=1;
	  absorberNo=1;
      }   
      if(indexDepth>2) {
	  sliceNo=2;
	  absorberNo=2;
      }
      
      Genfun::Variable Index;
      GeoXF::TRANSFUNCTION TS = GeoXF::Pow(HepGeom::TranslateZ3D(1.0),slicePositionZ + (absorberSize+gapSize)*Index);
      GeoXF::TRANSFUNCTION TA = HepGeom::TranslateX3D(absorberPosY) * GeoXF::Pow(HepGeom::TranslateZ3D(1.0),absorberPositionZ + (absorberSize+gapSize)*Index);

      GeoSerialIdentifier *sI = new GeoSerialIdentifier(sliceCopyNo);
      GeoSerialTransformer *sTS = new GeoSerialTransformer(physiSlice[sliceNo],   &TS, gapNumber[indexDepth]);
      GeoSerialTransformer *sTA = new GeoSerialTransformer(physiAbsorber[absorberNo],&TA, gapNumber[indexDepth]);
      physiDepth[indexDepth]->add(sI);
      physiDepth[indexDepth]->add(sTS);
      physiDepth[indexDepth]->add(sI);
      physiDepth[indexDepth]->add(sTA);
  } //for indexDepth     

  double firstAbsorberPositionZ = firstAbsorber[0]/2.- depthSize[0]/2.0;
  physiDepth[0]->add(new GeoIdentifierTag(50));
  physiDepth[0]->add(new GeoTransform(HepGeom::Translate3D(absorberPosY,0,firstAbsorberPositionZ))); 
  physiDepth[0]->add(physiFirstAbsorber); 

  
  firstAbsorberPositionZ = firstAbsorber[3]/2.- depthSize[3]/2.0;
  physiAbsorber[1] = new GeoPhysVol(logiAbsorber[1]);
  physiDepth[3]->add(new GeoIdentifierTag(51));
  physiDepth[3]->add(new GeoTransform(HepGeom::Translate3D(absorberPosY,0,firstAbsorberPositionZ))); 
  physiDepth[3]->add(physiAbsorber[1]);
  


//----------------------------------------------------------------
//    Electronic boards of Slice
//----------------------------------------------------------------
  std::string electrodeName = sliceName + "::Electrode";
  std::string copperName = electrodeName + "::Copper";
  for(int indexBoard=0; indexBoard<3; indexBoard++){
      if (indexBoard==0) moduleRinner = moduleRinner1;
         else  moduleRinner = moduleRinner2;           
      int indexKapton=1;  
      solidPadBoard = new GeoTubs(moduleRinner,outerRadius[0],copperPad/2.,modulePhistart,moduleDeltaPhi);                   
      logiPadBoard = new GeoLogVol(copperName, solidPadBoard, Copper);
      solidEstBoard = new GeoTubs(moduleRinner,outerRadius[0],(kaptonWidth[indexKapton]/2.+kaptonWidth[0]),
	                          modulePhistart,moduleDeltaPhi);
      logiEstBoard  = new GeoLogVol(electrodeName, solidEstBoard, Kapton);
// 
      double kaptonPositionZ = kaptonPosition[indexKapton]-gapSize/2.;

      physiEstBoard = new GeoPhysVol(logiEstBoard);
      physiSlice[indexBoard]->add(new GeoIdentifierTag(indexKapton));
      physiSlice[indexBoard]->add(new GeoTransform(HepGeom::Translate3D(0,0,kaptonPositionZ))); 
      physiSlice[indexBoard]->add(physiEstBoard);

      if(indexKapton==1) { 
	  physiPadBoard = new GeoPhysVol(logiPadBoard);
          physiEstBoard->add(new GeoIdentifierTag(indexKapton));
          physiEstBoard->add(new GeoTransform(HepGeom::Translate3D(0,0,0))); 
          physiEstBoard->add(physiPadBoard);
      }	   
  }//for indexBoard
//----------------------------------------------------------------
//    Tie rods in Slice
//----------------------------------------------------------------
  
  std::string tieRodName = sliceName + "::TieRod";
  double ztie[2];
  ztie[0]=-0.227825*CLHEP::cm;
  ztie[1]= 0.227825*CLHEP::cm;
  double rodSize = 0.39435*CLHEP::cm;
  for (int indexWheel=0; indexWheel<2; indexWheel++) { 
     solidTieRod[indexWheel] = new GeoTubs(0.*CLHEP::cm,spacerDiameter[indexWheel]/2.,rodSize/2.,
		                           0.*CLHEP::deg,360.*CLHEP::deg);         //size                 
     logiTieRod[indexWheel] = new GeoLogVol(tieRodName,solidTieRod[indexWheel], Iron);
  }
  for(int numberSlice=0; numberSlice<3; numberSlice++){
        int numberTie=0;
	if(numberSlice==2) numberTie=1;
	int indexRod;
        for(indexRod=1; indexRod<2; indexRod++){
            for(int iz=0;iz<2;iz++){
                physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
                physiSlice[numberSlice]->add(new GeoIdentifierTag(indexRod));
                physiSlice[numberSlice]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[indexRod],
                                                      tieRodPositionX[indexRod],ztie[iz]))); 
                physiSlice[numberSlice]->add(physiTieRod[numberTie]); 

                physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
                physiSlice[numberSlice]->add(new GeoIdentifierTag(indexRod));
                physiSlice[numberSlice]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[indexRod],
                                                      -tieRodPositionX[indexRod],ztie[iz]))); 
                physiSlice[numberSlice]->add(physiTieRod[numberTie]); 
            }//for iz
        }
        for( int iz1=0;iz1<2;iz1++){   
	        physiTieRod[numberTie] = new GeoPhysVol(logiTieRod[numberTie]);
                physiSlice[numberSlice]->add(new GeoIdentifierTag(indexRod));
                physiSlice[numberSlice]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[0],tieRodPositionX[0],ztie[iz1]))); 
                physiSlice[numberSlice]->add(physiTieRod[numberTie]);
        }//for iz1 
  }     
  
//----------------------------------------------------------------
//    Tie rods in Absorbers
//----------------------------------------------------------------
      
  solidAbsorberTieRod[0] = new GeoTubs(0.*CLHEP::cm,tieRodDiameter[0]/2.,absorberZ1/2.,0.*CLHEP::deg,360.*CLHEP::deg);                 
  solidAbsorberTieRod[1] = new GeoTubs(0.*CLHEP::cm,tieRodDiameter[1]/2.,absorberZ2/2.,0.*CLHEP::deg,360.*CLHEP::deg);                 
  logiAbsorberTieRod[0] = new GeoLogVol(tieRodName, solidAbsorberTieRod[0], Iron);
  logiAbsorberTieRod[1] = new GeoLogVol(tieRodName, solidAbsorberTieRod[1], Iron);
  for(int indexA=0; indexA<3; indexA++){  
    int indexR=0;
    if(indexA>1) indexR=1;
    int indexRod;
    for(indexRod=1; indexRod<2; indexRod++){
       physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
       physiAbsorber[indexA]->add(new GeoIdentifierTag(indexRod));
       physiAbsorber[indexA]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[indexRod]+absorberPosY,
		                                                  tieRodPositionX[indexRod], 0))); 
       physiAbsorber[indexA]->add(physiAbsorberTieRod[indexR]); 

       physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
       physiAbsorber[indexA]->add(new GeoIdentifierTag(indexRod));
       physiAbsorber[indexA]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[indexRod]+absorberPosY,
		                                                 -tieRodPositionX[indexRod], 0))); 
       physiAbsorber[indexA]->add(physiAbsorberTieRod[indexR]);
    }     
    physiAbsorberTieRod[indexR] = new GeoPhysVol(logiAbsorberTieRod[indexR]);
    physiAbsorber[indexA]->add(new GeoIdentifierTag(indexRod));
    physiAbsorber[indexA]->add(new GeoTransform(HepGeom::Translate3D(tieRodPositionY[0]+absorberPosY,
		                                               tieRodPositionX[0],0))); 
    physiAbsorber[indexA]->add(physiAbsorberTieRod[indexR]);
  }      
  
  // Some slices may not be used.
  // Make sure that if they're not used, that they get deleted.
  for (sliceNo = 0; sliceNo < 3; sliceNo++) {
    solidSlice[sliceNo]->ref();    solidSlice[sliceNo]->unref();
    logiSlice[sliceNo]->ref();     logiSlice[sliceNo]->unref();
    physiSlice[sliceNo]->ref();    physiSlice[sliceNo]->unref();
  }

  std::cout << " In the H6 2004 HEC Constr. GetEnvelope - about to return m_h6Phys " << std::endl;

  return m_h6Phys;

}


