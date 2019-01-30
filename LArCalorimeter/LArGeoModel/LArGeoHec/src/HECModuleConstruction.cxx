/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//  HECModuleConstruction.cxx 1.0.0
//  Derived from the HECConstruction - M. Fincke,  Sept.2007
//  
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
using GeoTrf::Translate3D;
using GeoTrf::TranslateY3D;
using GeoTrf::TranslateZ3D;


//Constructor
LArGeo::HECModuleConstruction::HECModuleConstruction(bool threeBoards, bool frontWheel, bool tb, int tbyear):
  m_physiHECModule(0)
{
  m_threeBoards = threeBoards;  // If true, build 3 boards in gap. Else just one thick board.
  m_frontWheel = frontWheel;    // If true, build a front Module. Else build a rear Module. 
  m_tb = tb;                    // If true, build a Module for testbeam. 
  m_tbyear = tbyear;            // If testbeam, specify the testbeam year. 

//   if (tb && ( tbyear!=2002 && tbyear!=2004 )) {
//     throw std::runtime_error ("TESTBEAM HEC: Asking for an unsupported HEC testbeam year!!!!! ");
//   }

}

//~Destructor
LArGeo::HECModuleConstruction::~HECModuleConstruction() 
{;}


GeoFullPhysVol* LArGeo::HECModuleConstruction::GetEnvelope()
{
  if(m_physiHECModule) return m_physiHECModule->clone();



  ISvcLocator *svcLocator = Gaudi::svcLocator();

  MsgStream log(Athena::getMessageSvc(),"HECModuleConstruction " );

  log << MSG::DEBUG << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  log << MSG::DEBUG << "+                                                             +" << endmsg;
  log << MSG::DEBUG << "+         Start of HECModule GeoModel definition              +" << endmsg;
  log << MSG::DEBUG << "+                                                             +" << endmsg;
  log << MSG::DEBUG << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in HECModuleConstruction, cannot access DetectorStore");
  }

  
  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in HECModuleConstruction, cannot access Material Manager");
  }
  
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in HECModuleConstruction, std::LiquidArgon is not found.");
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in HECModuleConstruction, std::Iron is not found.");
  
  const GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in HECModuleConstruction, std::Copper is not found.");
  
  const GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in HECModuleConstruction, std::Kapton is not found.");
  


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

  IRDBRecordset_ptr hadronicEndcap   = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  if(hadronicEndcap->size()>0) 
    log << MSG::DEBUG << "Using numbers from HadronicEndcap tag:  " << hadronicEndcap->tagName() << endmsg;
  else
    throw std::runtime_error("Error in HECConstruction:  hadronicEendcap not found");

  IRDBRecordset_ptr hecLongitudinalBlock = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 
  if(hecLongitudinalBlock->size()>0) 
    log << MSG::DEBUG << "Using numbers from HecLongitudinalBlock tag:  " << hecLongitudinalBlock->tagName() << endmsg;
  else
    throw std::runtime_error("Error in HECConstruction:  hecLongitudinalBlock not found");

  //----------------------------------------------------------------
  // Collect all the numbers we need from the database: 
  //----------------------------------------------------------------

  double shrinkCold = 1.0;  //thermal expansion factor: 1.0 = warm geometry
                            //kapton is treated the same at Cu and Fe at the moment

  int    moduleNumber          = (*hadronicEndcap)[0]->getInt("NSCT");
  double moduleRinner1         = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMN")*cm;
  double moduleRinner2         = shrinkCold * (*hecLongitudinalBlock)[1]->getDouble("BLRMN")*cm;
  double moduleRouter          = shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("BLRMX")*cm;
  if (m_tbyear==2004) moduleRouter = moduleRinner1 + 787*mm;  // Mini Modules for 2004 testbeam  CAUTION! Hard-coded number!!!
  double copperPad             = shrinkCold * (*hadronicEndcap)[0]->getDouble("COPPER")*cm;
  double gapSize               = shrinkCold * (*hadronicEndcap)[0]->getDouble("LARG")*cm;
  double drModOverlap          = shrinkCold * (*hadronicEndcap)[0]->getDouble("DRMODOVERLAP")*cm;
  double kaptonWidth[2];
  kaptonWidth[0]               = shrinkCold * (*hadronicEndcap)[0]->getDouble("ESTPL")*cm;
  kaptonWidth[1]               = shrinkCold * (*hadronicEndcap)[0]->getDouble("PADPL")*cm;
  int    gapN[7];
  for(int id=0;id<7;id++) gapN[id] = (int) (*hecLongitudinalBlock)[id]->getDouble("BLMOD");
  double firstFrontAbsThickness= shrinkCold * (*hecLongitudinalBlock)[0]->getDouble("PLATE0")*cm;
  double firstRearAbsThickness = shrinkCold * (*hecLongitudinalBlock)[3]->getDouble("PLATE0")*cm;
  double frontAbsThickness     = shrinkCold * (*hadronicEndcap)[0]->getDouble("PLATE_0")*cm;
  double rearAbsThickness      = shrinkCold * (*hadronicEndcap)[0]->getDouble("PLATE_1")*cm;
  double tieRodPositionX[4];
  double tieRodPositionY[4];
  tieRodPositionX[0]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSX_0")*cm;
  tieRodPositionY[0]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSR_0")*cm;
  tieRodPositionX[1]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSX_1")*cm;
  tieRodPositionY[1]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSR_1")*cm;
  tieRodPositionX[2]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSX_2")*cm;
  tieRodPositionY[2]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSR_2")*cm;
  tieRodPositionX[3]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSX_3")*cm;
  tieRodPositionY[3]           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODPOSR_3")*cm;
  double tieRodDiameter        = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODDIM_0")*cm;
  double spacerDiameter        = shrinkCold * (*hadronicEndcap)[0]->getDouble("SPCDIM_0")*cm;
  double spacerDeadZone        = shrinkCold * 3.0*mm;
  if (!m_frontWheel) 
    {
      tieRodDiameter           = shrinkCold * (*hadronicEndcap)[0]->getDouble("RODDIM_1")*cm;
      spacerDiameter           = shrinkCold * (*hadronicEndcap)[0]->getDouble("SPCDIM_1")*cm;
    }


  //--- Get the right Absorber thickness and Depth number depending on Front or Rear Wheel (and TB) 
  //    (2002 and 2004 testbeam have only 1/2 rear depth)
  int    depthNumber           = 3; 
  int    depthOffset           = depthNumber;  // need this to start rear wheel depth-number at 3 
  double absThickness          = frontAbsThickness;
  double firstAbsThickness     = firstFrontAbsThickness;
  if (!m_frontWheel) {
    depthNumber          = 4; 
    if (m_tb) depthNumber= 2;         
    absThickness         = rearAbsThickness;
    firstAbsThickness    = firstRearAbsThickness;
  }


  //---- Calculate the sizes of the 7 longitudinal Depths -------------------------------------
  double depthS[7];
  depthS[0]              = firstFrontAbsThickness + gapN[0]*(frontAbsThickness+gapSize);
  depthS[1]              = gapN[1]*(frontAbsThickness+gapSize);
  depthS[2]              = gapN[2]*(frontAbsThickness+gapSize);
  depthS[3]              = firstRearAbsThickness + gapN[3]*(rearAbsThickness+gapSize);
  depthS[4]              = gapN[4]*(rearAbsThickness+gapSize);
  depthS[5]              = gapN[5]*(rearAbsThickness+gapSize);
  depthS[6]              = gapN[6]*(rearAbsThickness+gapSize);

  //--- From the depth Size, calculate the boundary planes of the Module Mother volume ---------
  //    (Add 0.1 mm to avoid clashes)
  double zCoordinate[4];
  double depthSize[4];
  int    gapNumber[4];
  int    gaptally[4];
  double g4allow  = shrinkCold * 0.1*mm ;    
  double g4allowS = 0.001*mm ;  
  if (m_frontWheel){
    zCoordinate[0] = shrinkCold * 0.0*cm; 
    zCoordinate[1] = depthS[0]; 
    zCoordinate[2] = depthS[0] + g4allow; 
    zCoordinate[3] = depthS[0]+depthS[1]+depthS[2] + g4allow;                // End of front Module  
    gaptally[0]=0;
    for (int i=0; i<3; i++) {
      if (i>0) gaptally[i] = gaptally[i-1] + gapN[i-1];
      gapNumber[i] = gapN[i];         
      depthSize[i] = depthS[i];
    }
  }
  else{
    zCoordinate[0]         = shrinkCold * 0.0*cm; 
    zCoordinate[1]         = depthS[3]+depthS[4]+depthS[5]+depthS[6]+g4allow; // End of rear Atlas Module
    if(m_tb) zCoordinate[1]= depthS[3]+depthS[4]+g4allow;                     // For TB only 1/2 rear depth
    gaptally[0] = 0;
    for (int i=0; i<4; i++) {                        // Re-number the rear wheel  
      if (i>0) gaptally[i] =gaptally[i-1] + gapN[i+3-1];
      gapNumber[i] = gapN[i+3];                      // gaps and depths to have
      depthSize[i] = depthS[i+3];                    // their index start at Zero.
    }                                  
  }

  //--- Calculate where to put the tie-rod spacers into the available LAr gap.
  //    This depends on the configuration of the boards. 

  //   +/-ztie are the center z-coordinates of the LAr sub-gaps
  //   rodSize is the length of spacer to be fit into the LAr that is not occupied by boards
  //   Small tolerance to fit G4 volumes into one another 
  double ztie[2];
  double rodSize; 
 
  //-- For Three-board approach:
  if (m_threeBoards) {
    double halfGap    = gapSize/2.;
    double halfGapLAr = (gapSize - kaptonWidth[1])/2.;
    double restLAr    = (halfGapLAr-kaptonWidth[0])/2. ;
    ztie[0]           = restLAr/2 + kaptonWidth[1]/2.; 
    ztie[1]           = halfGap - restLAr/2.;          
    rodSize           = restLAr - g4allowS; 
  }
  else{
  //-- For One-board approach:
    double halfGap    = gapSize/2.;
    double halfGapLAr = (gapSize - kaptonWidth[1])/2. - kaptonWidth[0];
    ztie[0]           = (halfGap-halfGapLAr/2.);  
    ztie[1]           = 0.0; // fix complilation warning
    rodSize           =  halfGapLAr-g4allowS;    
  }


  //---- And here we have some more (yikes!) hard-coded values: ------------------------
  int    nZplane             = 4;
  if (!m_frontWheel) nZplane = 2;
  int    nRods               = 4;
  if (m_tbyear==2004) nRods  = 2;
  double modulePhistart      = 264.375*deg;          // (270.-11.25/2.)*deg 	  
  double moduleDeltaPhi      = 2*M_PI/moduleNumber;  //11.25*deg;  
  int sectMax                = 2;
  if(!m_frontWheel) sectMax  = 1;
  int rearOffset             = 24;   // The rear wheel slice numbering starts at 24
                                     // This complies with the old HECConstruction. 

  //--- A dead-zone in the LAr gap of depth deadZone all around the module 
  //    requires a radial shift of size larshift
  double deadZone   = shrinkCold * 3.*mm;                       
  double larShift   = deadZone / sin(moduleDeltaPhi/2.);    

  //--- To create a (dead) inter-module gap of 2mm (i.e. 1mm on either side of each module)
  //    between the absorbers, use a radial shift of -1.02*cm.
  //    This does that:
  double deadGap = shrinkCold * 1.*mm;                       
  double radialShift = deadGap / sin(moduleDeltaPhi/2.);    





  //-------------------------------------------------------------------//
  //-------------------------------------------------------------------//
  //   Now start building the Module geometry                          //
  //   First create all the little bits so that they are               //
  //   ready to be inserted into the bigger pieces when they get       //
  //   are created.                                                    //
  //-------------------------------------------------------------------//
  //-------------------------------------------------------------------//

  std::string moduleName    = "LAr::HEC::Module";
  std::string depthName     = moduleName + "::Depth";
  std::string sliceName     = depthName + "::Slice";
  std::string electrodeName = sliceName + "::Electrode";
  std::string copperName    = electrodeName + "::Copper";
  std::string tieRodName    = sliceName + "::TieRod";
  std::string deadTieName   = sliceName + "::TieRodDead";
  std::string absorberName  = depthName + "::Absorber";
  std::string firstAbsorberName = depthName + "::FirstAbsorber";
  std::string absTieRodName = absorberName + "::TieRod";
  std::string firstabsTieRodName = firstAbsorberName + "::TieRod";

  //---- Tie rods to go into Absorbers --------------------------------------

  GeoTubs* solidAbsorberTieRod         = new GeoTubs(0.*cm,tieRodDiameter/2.,absThickness/2.,0.*deg,360.*deg); 
  const GeoLogVol* logiAbsorberTieRod  = new GeoLogVol(absTieRodName,solidAbsorberTieRod,Iron);  //,0,0,0);
  GeoPhysVol* physiAbsorberTieRod      = new GeoPhysVol(logiAbsorberTieRod);

  GeoTubs* solidFirstAbsorberTieRod    = new GeoTubs(0.*cm,tieRodDiameter/2.,firstAbsThickness/2.,0.*deg,360.*deg); 
  const GeoLogVol* logiFirstAbsorberTieRod = new GeoLogVol(firstabsTieRodName,solidFirstAbsorberTieRod,Iron);  //,0,0,0);
  GeoPhysVol* physiFirstAbsorberTieRod = new GeoPhysVol(logiFirstAbsorberTieRod);


  //---- Tie rods to go into Slices (i.e. sensitive LAr gaps) -----------------

  GeoTubs* solidTieRod        = new GeoTubs(0.*cm,spacerDiameter/2.,rodSize/2.,0.*deg,360.*deg);  
  const GeoLogVol* logiTieRod = new GeoLogVol(tieRodName, solidTieRod, Iron); 
  GeoPhysVol* physiTieRod     = new GeoPhysVol(logiTieRod);


  GeoTubs* solidDeadTie        = new GeoTubs(g4allowS+spacerDiameter/2.,
					     spacerDeadZone+spacerDiameter/2.,
					     rodSize/2.,
					     0.*deg,360.*deg);  
  const GeoLogVol* logiDeadTie = new GeoLogVol(deadTieName, solidDeadTie, LAr); 
  GeoPhysVol* physiDeadTie     = new GeoPhysVol(logiDeadTie);


  //----  Create PAD board and EST boards, then also Slices (ie. LAr gaps) and put the boards into Slices ------------
  //      This requires a loop, because the frontWheel has two sections (iSect) of different radial size.
  //      The same goes for the Absorbers - create them in this loop, too.
  //      (NB: Each wheel has only one "firstAbsorber"; Create that ahead of everything else.)
  //      And finally equip the LAr Slices as well as the Absorbers with their respective tie-rods.  


  GeoTubs*         solidPadBoard[2] ;
  const GeoLogVol* logiPadBoard[2];
  GeoPhysVol*      physiPadBoard[2];

  GeoTubs*         solidCopperPad[2];
  const GeoLogVol* logiCopperPad[2];
  GeoPhysVol*      physiCopperPad[2];

  GeoTubs*         solidEstBoard[2];
  const GeoLogVol* logiEstBoard[2];
  GeoPhysVol*      physiEstBoard[2];

  GeoTubs*         solidSlice[2];
  const GeoLogVol* logiSlice[2];
  GeoPhysVol*      physiSlice[2];

  GeoTubs*         solidAbsorber[2];
  const GeoLogVol* logiAbsorber[2];
  GeoPhysVol*      physiAbsorber[2];

  // First Absorbers in front of first and third samplings (need only one type in each wheel)
  double rOuterF           = moduleRouter-radialShift;
  double rInnerF           = moduleRinner2-radialShift;
  if (m_frontWheel) rInnerF= moduleRinner1-radialShift;
  GeoTubs* solidFirstAbsorber         = new GeoTubs(rInnerF,rOuterF,
					    firstAbsThickness/2.,
					    modulePhistart,moduleDeltaPhi);                   
  const GeoLogVol* logiFirstAbsorber  = new GeoLogVol(firstAbsorberName, solidFirstAbsorber, Copper);
  GeoPhysVol* physiFirstAbsorber      = new GeoPhysVol(logiFirstAbsorber);


  for(int iSect=0; iSect<sectMax; iSect++)
    {
      //-------- First make the slice: ------------------
      // NB: larShift produces the inter-module gap,
      //     but the actual radial size of the board or slice also has to be
      //     made smaller by the value of deadZone.     

      double rOuter = moduleRouter - larShift - deadZone;  
      double rInner = moduleRinner2 - larShift + deadZone;
      if (iSect<1 && m_frontWheel) rInner = moduleRinner1 - larShift + deadZone;
      solidSlice[iSect] = new GeoTubs(rInner, rOuter, gapSize/2., modulePhistart, moduleDeltaPhi); 
      logiSlice[iSect]  = new GeoLogVol(sliceName, solidSlice[iSect], LAr);
      physiSlice[iSect] = new GeoPhysVol(logiSlice[iSect]);

      //--------  Now make the boards: -------- 
      rOuter = moduleRouter - larShift - deadZone -g4allowS;
      rInner = moduleRinner2 - larShift + deadZone +g4allowS;
      if (m_frontWheel && iSect==0) rInner = moduleRinner1 - larShift + deadZone +g4allowS ;
      
      // First, make a central PAD Board: Copper encased in Kapton
      // In case of the one-board approach, that's the only board (just thicker)
      if (m_threeBoards){
	solidPadBoard[iSect] = new GeoTubs(rInner,rOuter,
				    kaptonWidth[1]/2.,
				    modulePhistart,moduleDeltaPhi);
      }
      else{
	solidPadBoard[iSect] = new GeoTubs(rInner,rOuter,
				    (kaptonWidth[1]/2.+ kaptonWidth[0]),
				    modulePhistart,moduleDeltaPhi);
      }
      logiPadBoard[iSect]  = new GeoLogVol(electrodeName, solidPadBoard[iSect], Kapton );
      physiPadBoard[iSect] = new GeoPhysVol(logiPadBoard[iSect]);
      
      // The central board contains copper pads for readout
      solidCopperPad[iSect] = new GeoTubs(rInner,rOuter,copperPad/2.,
				   modulePhistart,moduleDeltaPhi);                   
      logiCopperPad[iSect]  = new GeoLogVol(copperName, solidCopperPad[iSect], Copper);
      physiCopperPad[iSect] = new GeoPhysVol(logiCopperPad[iSect]);
      physiPadBoard[iSect]->add(physiCopperPad[iSect]);  
      
      // For the three-board approach, make an individual EST board and place it twice in the LAr gap
      if(m_threeBoards){
	solidEstBoard[iSect] = new GeoTubs(rInner,rOuter,kaptonWidth[0]/2.,
				    modulePhistart,moduleDeltaPhi);
	logiEstBoard[iSect]  = new GeoLogVol(electrodeName, solidEstBoard[iSect], Kapton );
	physiEstBoard[iSect] = new GeoPhysVol(logiEstBoard[iSect]);	
      }

      //--- Insert the Boards into the Slice ---------
      
      physiSlice[iSect]->add(new GeoIdentifierTag(1));
      physiSlice[iSect]->add(physiPadBoard[iSect]);     
      
      
      // For the three-board approach, make an individual EST board and place it twice in the LAr gap
      if(m_threeBoards){
	double halfGapLAr = (gapSize - kaptonWidth[1])/2.;
	double ESTPos = (kaptonWidth[1]+halfGapLAr)/2.;
	physiSlice[iSect]->add(new GeoIdentifierTag(0));
	physiSlice[iSect]->add(new GeoTransform(Translate3D(0,0,(-ESTPos))));  
	physiSlice[iSect]->add(physiEstBoard[iSect]);     
	physiSlice[iSect]->add(new GeoIdentifierTag(2));
	physiSlice[iSect]->add(new GeoTransform(Translate3D(0,0,ESTPos)));   
	physiSlice[iSect]->add(physiEstBoard[iSect]);     
      }
 



  //--- Create the absorbers -----

      rOuter           = moduleRouter - radialShift;
      rInner           = moduleRinner2 - radialShift;
      if (iSect<1 && m_frontWheel) rInner = moduleRinner1 - radialShift;
      solidAbsorber[iSect] = new GeoTubs(rInner,rOuter,absThickness/2., modulePhistart,moduleDeltaPhi); 
      logiAbsorber[iSect]  = new GeoLogVol(absorberName, solidAbsorber[iSect], Copper);
      physiAbsorber[iSect] = new GeoPhysVol(logiAbsorber[iSect]);



  //--- Insert tie rods into the Absorbers and Slices -----
  //    There are three pairs (left/right symmetric-> handled by loop over isignX)
  //    and they need to be placed in z on either side of the board(s) (loop over isign)
  //    (Note; we did the radial shift for the absorber(and slice); have to compensate for that now in Y position)   


      for(int indexRod=1; indexRod<nRods; indexRod++) {
	for(double isignX=-1;isignX<2;isignX=isignX+2.){

	  physiAbsorber[iSect]->add(new GeoIdentifierTag(indexRod));
	  physiAbsorber[iSect]->add(new GeoTransform(Translate3D(isignX*tieRodPositionX[indexRod],
								     -tieRodPositionY[indexRod]+radialShift, 0)));     
	  physiAbsorber[iSect]->add(physiAbsorberTieRod);     
	  if (iSect==0){ // fill the FirstAbsorber with tie-rods at the same time)
	    physiFirstAbsorber->add(new GeoIdentifierTag(indexRod));
	    physiFirstAbsorber->add(new GeoTransform(Translate3D(isignX*tieRodPositionX[indexRod],
								    -tieRodPositionY[indexRod]+radialShift, 0)));     
	    physiFirstAbsorber->add(physiFirstAbsorberTieRod);     
	  }

          // For the LAr slices, we have to insert the spacers around the boards (in Z):

	  for(int itie=0; itie<2; itie++ ){  // itie=0 inserts spacers, itie=1 inserts spacer dead-zone
	    for(double isignZ=-1;isignZ<2;isignZ=isignZ+2.){
	      physiSlice[iSect]->add(new GeoIdentifierTag(indexRod));
	      physiSlice[iSect]->add(new GeoTransform
				     (Translate3D(isignX*tieRodPositionX[indexRod],
                                                  -tieRodPositionY[indexRod]+larShift,
                                                  isignZ*ztie[0])));
	      if (itie==0) { physiSlice[iSect]->add(physiTieRod); }     
	      else         { physiSlice[iSect]->add(physiDeadTie);}

	      if (m_threeBoards) {  
		physiSlice[iSect]->add(new GeoIdentifierTag(indexRod));
		physiSlice[iSect]->add(new GeoTransform
				       (Translate3D(isignX*tieRodPositionX[indexRod],
                                                    -tieRodPositionY[indexRod]+larShift,
                                                    isignZ*ztie[1])));     
		if (itie==0) { physiSlice[iSect]->add(physiTieRod); }     
		else         { physiSlice[iSect]->add(physiDeadTie);}     
	      }
	    }
	  }
      


	}
      }

      //--- This is for the lonely un-paired tie-rod at the narrow end of the module (Absorber): 
      physiAbsorber[iSect]->add(new GeoIdentifierTag(0));
      physiAbsorber[iSect]->add(new GeoTransform(Translate3D(tieRodPositionX[0],
                                                             -tieRodPositionY[0]+radialShift,0)));     
      physiAbsorber[iSect]->add(physiAbsorberTieRod);     
      if (iSect==0){ 
	physiFirstAbsorber->add(new GeoIdentifierTag(0));
	physiFirstAbsorber->add(new GeoTransform(Translate3D(tieRodPositionX[0],
                                                             -tieRodPositionY[0]+radialShift,0)));     
	physiFirstAbsorber->add(physiFirstAbsorberTieRod);     
      }

      //--- And the lonely Slice tie-rods: -------------
      for(int itie=0; itie<2; itie++ ){  // tie=0 inserts spacers, itie=1 inserts spacer dead-zone
	for(double isignZ=-1;isignZ<2;isignZ=isignZ+2){
	  physiSlice[iSect]->add(new GeoIdentifierTag(0));
	  physiSlice[iSect]->add(new GeoTransform(Translate3D(tieRodPositionX[0],
                                                              -tieRodPositionY[0]+larShift, 
                                                              isignZ*ztie[0])));    
	  if (itie==0) { physiSlice[iSect]->add(physiTieRod); }     
	  else         { physiSlice[iSect]->add(physiDeadTie);}

	  if (m_threeBoards) { 
	    physiSlice[iSect]->add(new GeoIdentifierTag(0));
	    physiSlice[iSect]->add(new GeoTransform(Translate3D(tieRodPositionX[0],
                                                                -tieRodPositionY[0]+larShift,
                                                                isignZ*ztie[1])));     
	    if (itie==0) { physiSlice[iSect]->add(physiTieRod); }      
	    else         { physiSlice[iSect]->add(physiDeadTie);}     
	  }
	} 
      }
      
    }//for iSect
  
  
  
  // Now that Slices and Absorbers are filled with their components,
  // create the Depths and place the slices and absorbers into them


  //----------------------------------------------------------------
  //    Depths
  //----------------------------------------------------------------
  // There are 3 depths in the front wheel, and 4 in the rear wheel.
  // Create them and equip them with fully assembled slices and absorbers.

  GeoTubs*           solidDepth[4] ;
  const GeoLogVol*   logiDepth[4]  ;
  GeoPhysVol*        physiDepth[4] ;

  for(int iDepth=0; iDepth<depthNumber; iDepth++)
    {
      double rOuter       = moduleRouter;
      double rInner       = moduleRinner2;
      if (iDepth<1 && m_frontWheel) rInner= moduleRinner1;
      solidDepth[iDepth]  = new GeoTubs(rInner-drModOverlap,rOuter, 
					depthSize[iDepth]/2.,modulePhistart,moduleDeltaPhi);                   
      logiDepth[iDepth]   = new GeoLogVol(depthName, solidDepth[iDepth], LAr);
      physiDepth[iDepth]  = new GeoPhysVol(logiDepth[iDepth]);


      // Position the sensitive LAr Slice-gaps and absorbers into the just created Depth:
      // Absorber size and position
      double firstAbs          = 0.;
      if (iDepth==0) firstAbs  = firstAbsThickness;       
      double slicePositionZ    = firstAbs + gapSize/2.0 -depthSize[iDepth]/2.0;
      double absorberPositionZ = firstAbs + gapSize + absThickness/2.0 - depthSize[iDepth]/2.0;

      // sliceCopyNo is the copy number of the slice and runs consecutively from
      // front to back throughout the module.
      // The first gap in the rear Wheel has number 24 (=rearOffset).
      // sliceNo refers to how many slice types there are (0,1 in Front ; 0 in Rear)
      // gapNumber is the number of gaps in a given Depth 
      int sliceNo           = 0;
      int sliceCopyNo       = 0;
      if(!m_frontWheel) sliceCopyNo = rearOffset ; 

      if (iDepth>0) 
	{
	  sliceCopyNo += gaptally[iDepth]; 
          if(m_frontWheel) sliceNo=1;
	}

      // Serially install all slices and _regular_ absorbers into the depths       
      GeoGenfun::Variable Index;
      GeoXF::TRANSFUNCTION TS = 
	TranslateY3D(-larShift)*GeoXF::Pow(TranslateZ3D(1.0),slicePositionZ + (absThickness+gapSize)*Index);
      GeoXF::TRANSFUNCTION TA = 
	TranslateY3D(-radialShift)*GeoXF::Pow(TranslateZ3D(1.0),absorberPositionZ + (absThickness+gapSize)*Index);

      GeoSerialIdentifier  *sI   = new GeoSerialIdentifier(sliceCopyNo);
      GeoSerialTransformer *sTS  = new GeoSerialTransformer(physiSlice[sliceNo],    &TS, gapNumber[iDepth]);
      GeoSerialTransformer *sTAF = new GeoSerialTransformer(physiAbsorber[sliceNo], &TA, gapNumber[iDepth]);
      physiDepth[iDepth]->add(sI);
      physiDepth[iDepth]->add(sTS);
      physiDepth[iDepth]->add(sI);
      physiDepth[iDepth]->add(sTAF);

    } //for iDepth     


  // The first absorber is special (1/2 thickness) install that now
  double firstAbsorberPositionZ = firstAbsThickness/2.- depthSize[0]/2.0;
  if(m_frontWheel) physiDepth[0]->add(new GeoIdentifierTag(0));
  else             physiDepth[0]->add(new GeoIdentifierTag(1));
  physiDepth[0]->add(new GeoTransform(Translate3D(0,-radialShift,firstAbsorberPositionZ)));     
  physiDepth[0]->add(physiFirstAbsorber);     
  

  //----------------------------------------------------------------
  //   HEC Module
  //----------------------------------------------------------------


  GeoPcon* solidHECModule = new GeoPcon(modulePhistart, moduleDeltaPhi);
  for (int i=0; i< nZplane; i++) 
     {
        double innerRadius = moduleRinner2;
        if (i<2 && m_frontWheel) innerRadius=moduleRinner1;  
        solidHECModule->addPlane(zCoordinate[i],innerRadius-drModOverlap,moduleRouter);
     }
  const GeoLogVol* logicHECModule = new GeoLogVol(moduleName, solidHECModule , LAr);
  GeoFullPhysVol* physiHECModule  = new GeoFullPhysVol(logicHECModule);


  //---  Place the fully instrumented depths into the Module: ----

  double depthPositionZ = 0.;

  for(int iDepth=0; iDepth<depthNumber; iDepth++)
    {
      depthPositionZ +=depthSize[iDepth]/2.;
      if(!m_frontWheel) physiHECModule->add(new GeoIdentifierTag(iDepth+depthOffset));
      else              physiHECModule->add(new GeoIdentifierTag(iDepth));
      physiHECModule->add(new GeoTransform(Translate3D(0,0,depthPositionZ)));     
      physiHECModule->add(physiDepth[iDepth]);     
      depthPositionZ +=depthSize[iDepth]/2.;
    } //for iDepth     



  // Return the physical volume that contains everything we've placed.

  //-- clean up:
  solidAbsorberTieRod->ref();       solidAbsorberTieRod->unref();     
  logiAbsorberTieRod->ref();        logiAbsorberTieRod->unref();  
  physiAbsorberTieRod->ref();       physiAbsorberTieRod->unref(); 
  solidFirstAbsorberTieRod->ref();  solidFirstAbsorberTieRod->unref(); 
  logiFirstAbsorberTieRod->ref();   logiFirstAbsorberTieRod->unref();
  physiFirstAbsorberTieRod->ref();  physiFirstAbsorberTieRod->unref();   
  solidTieRod->ref();               solidTieRod->unref();    
  logiTieRod->ref();                logiTieRod->unref(); 
  physiTieRod->ref();               physiTieRod->unref();
  solidDeadTie->ref();              solidDeadTie->unref();    
  logiDeadTie->ref();               logiDeadTie->unref(); 
  physiDeadTie->ref();              physiDeadTie->unref();
  solidFirstAbsorber->ref();        solidFirstAbsorber->unref(); 
  logiFirstAbsorber->ref();         logiFirstAbsorber->unref();  
  physiFirstAbsorber->ref();        physiFirstAbsorber->unref(); 
  for(int iSect=0; iSect<sectMax; iSect++)
    {
      solidCopperPad[iSect]->ref(); solidCopperPad[iSect]->unref(); 
      logiCopperPad[iSect]->ref();  logiCopperPad[iSect]->unref();  
      physiCopperPad[iSect]->ref(); physiCopperPad[iSect]->unref(); 
      solidPadBoard[iSect]->ref();  solidPadBoard[iSect]->unref(); 
      logiPadBoard[iSect]->ref();   logiPadBoard[iSect]->unref();  
      physiPadBoard[iSect]->ref();  physiPadBoard[iSect]->unref(); 
      if(m_threeBoards){
	solidEstBoard[iSect]->ref();  solidEstBoard[iSect]->unref(); 
	logiEstBoard[iSect]->ref();   logiEstBoard[iSect]->unref();  
	physiEstBoard[iSect]->ref();  physiEstBoard[iSect]->unref(); 
      }
      solidAbsorber[iSect]->ref();  solidAbsorber[iSect]->unref(); 
      logiAbsorber[iSect]->ref();   logiAbsorber[iSect]->unref();  
      physiAbsorber[iSect]->ref();  physiAbsorber[iSect]->unref(); 
      solidSlice[iSect]->ref();     solidSlice[iSect]->unref(); 
      logiSlice[iSect]->ref();      logiSlice[iSect]->unref();  
      physiSlice[iSect]->ref();     physiSlice[iSect]->unref(); 
    }
  for(int iDepth=0; iDepth<depthNumber; iDepth++){
    solidDepth[iDepth]->ref();  solidDepth[iDepth]->unref();  
    logiDepth[iDepth]->ref();   logiDepth[iDepth]->unref();   
    physiDepth[iDepth]->ref();  physiDepth[iDepth]->unref();  
  }

  return physiHECModule;
}
