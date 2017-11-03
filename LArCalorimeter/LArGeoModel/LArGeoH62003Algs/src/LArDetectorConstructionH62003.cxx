/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArDetectorConstructionH62003

#include "LArDetectorConstructionH62003.h"
#include "LArGeoH6Cryostats/H6CryostatConstruction.h"
#include "LArGeoFcal/FCALConstruction.h"
#include "GeoModelKernel/GeoElement.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoFullPhysVol.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPgon.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoXF.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"


#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "LArG4RunControl/LArG4TBPosOptions.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/GenericFunctions/Variable.hh"

// For units:
#include "CLHEP/Units/PhysicalConstants.h"

// For the database:
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoModelUtilities/DecodeVersionKey.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include <string>
#include <cmath>
#include <iostream>

using CLHEP::g;
using CLHEP::cm3;
using CLHEP::mm;
using CLHEP::cm;
using CLHEP::m;
using CLHEP::deg;
using CLHEP::Hep3Vector;
using CLHEP::HepRotationX;
using CLHEP::HepRotationY;
using HepGeom::Transform3D;
using HepGeom::Translate3D;
using HepGeom::TranslateZ3D;
using HepGeom::RotateX3D;
using HepGeom::RotateY3D;
using HepGeom::RotateZ3D;


LArGeo::LArDetectorConstructionH62003::LArDetectorConstructionH62003():
H62003EnvelopePhysical(NULL),
_fcalVisLimit(-1),
_axisVisState(false),
pAccessSvc(NULL){}

LArGeo::LArDetectorConstructionH62003::~LArDetectorConstructionH62003() {}

GeoVPhysVol* LArGeo::LArDetectorConstructionH62003::GetEnvelope()
{

  if (H62003EnvelopePhysical) return H62003EnvelopePhysical;

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorConstructionH62003, cannot access MessageSvc");
  }

  MsgStream log(msgSvc, "LArGeo::LArDetectorConstructionH62003"); 

  log << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                     +" << std::endl;
  log << "+    HELLO from LArGeo::LArDetectorConstructionH62003 +" << std::endl;
  log << "+                                                     +" << std::endl;
  log << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorConstructionH62003, cannot access DetectorStore");
  }
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;

  StatusCode sc;
  sc=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }

  DecodeVersionKey larVersion("LAr");
  std::string detectorKey  = larVersion.tag();
  std::string detectorNode = larVersion.node();

  log << MSG::DEBUG << "detectorKey is "  << detectorKey << endmsg; 
  log << MSG::DEBUG << "detectorNode is " << detectorNode << endmsg; 


  // Get the materials from the material manager:----------------------//
  //                                                                                                 
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Air is not found.");
 
  GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Lead is not found.");
  
  GeoMaterial *PStyrene  = materialManager->getMaterial("std::Polystyrene");
  if (!PStyrene) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Polystyrene is not found.");

  GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Iron is not found.");

  GeoMaterial *Aluminum  = materialManager->getMaterial("std::Aluminium");
  if (!Aluminum) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Aluminum is not found.");  

  GeoMaterial *Concrete  = materialManager->getMaterial("std::Concrete");
  if (!Concrete) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Concrete is not found.");  

  GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::LiquidArgon is not found.");  

  GeoMaterial *Vac  = materialManager->getMaterial("std::Vacuum");
  if (!Vac) 
    throw std::runtime_error("Error in LArDetectorConstructionH62003, std::Vacuum is not found.");  

  // Rohacell foam has density: 0.011g/cm3
  const std::string RohacellName = "Rohacell";
  const double RoDensity = 0.011*g/cm3;
  GeoElement*  C=materialManager->getElement("Carbon");
  GeoElement*  H=materialManager->getElement("Hydrogen");
  GeoElement*  O=materialManager->getElement("Oxygen");
  GeoElement*  N=materialManager->getElement("Nitrogen");
  GeoMaterial* Rohacell = new GeoMaterial(RohacellName, RoDensity);
  Rohacell->add(C,0.6465);
  Rohacell->add(H,0.07836);
  Rohacell->add(O,0.19137);
  Rohacell->add(N,0.08377);
  Rohacell->lock();
  
  
  // Stainless Steel
  const std::string SSteelName = "StainlessSteel";
  double SSDensity = 7.9*g/cm3;
  GeoElement* Fe=materialManager->getElement("Iron");
  GeoElement* Cr=materialManager->getElement("Chromium");
  GeoMaterial* StainlessSteel = new GeoMaterial(SSteelName, SSDensity);
  StainlessSteel->add(Fe,0.90);
  StainlessSteel->add(Cr,0.10);
  StainlessSteel->lock();

  // Define the Geometries:--------------------------------------------//
  // Set up strings for volume names.
  std::string baseName = "LAr::TBH62003";
  
  // Define a translation variable to write detector positions so that if we add beampipe, etc, we can just translate the detectors. 

  //  double trans = -535.68*cm; // chosen so that FCal is in original ATLAS FCal position.
  double trans = -543.12*cm; //=-535.68-7.44; chosen so that FCal is in original ATLAS FCal position.

  // Define the Experimental Hall  
  std::string H62003MotherName = baseName + "::MotherVolume";
  const GeoBox* H62003MotherShape = new GeoBox(2.5*m, 2.5*m, 30.*m);
  const GeoLogVol* H62003MotherLogical =
    new GeoLogVol(H62003MotherName, H62003MotherShape, Air);
  
  H62003EnvelopePhysical = new GeoFullPhysVol(H62003MotherLogical);
  H62003EnvelopePhysical->add( new GeoNameTag("LArEndcapPos") );
  
  // number of volumes added to H62003EnvelopePhysical.
  // Every time a volume is added to H62003EP, increment.  Used for debugging
  unsigned int NH6EP = 0; 

  // Tubular axis, dummy; used for dedugging
  // Make it a jobOption.
  if ( _axisVisState ) {
    std::cout << "Axis is ON" << std::endl;
    createAxis(H62003EnvelopePhysical, Air);
  }

  // Cryostat
  H6CryostatConstruction cryoConstruction;
  GeoVFullPhysVol* cryoPhys = cryoConstruction.GetEnvelope();
  GeoPhysVol* LArPhysical = cryoConstruction.GetLArPhysical();

  // The Cryostat will be the reference to which other detectors are positioned.
  std::string nickname   = "POSITION1";
  int nicknumber = 0; 

  if(LArPhysical != 0)
    {
      IRDBRecordset_ptr larTBPos = 
	pAccessSvc->getRecordsetPtr("LArTBPosition", detectorKey, detectorNode);
          
      LArG4TBPosOptions *posOptions = NULL;
      StatusCode status = detStore->retrieve(posOptions,"LArG4TBPosOptions");
      if (status.isSuccess()) {
	nickname = posOptions->PositionNickname();
	nicknumber = posOptions->PositionNicknumber();
      }
      // These will enventually go in the database
      double CryoXPos = 0.0;
      double CryoYPos = 0.0;
      //double CryoZPos = 0.0;
      if( nickname == "POSITION1")   {CryoXPos = 1.8*cm;}
      if( nickname == "POSITION2")   {CryoXPos = -1.45*cm;}
      if( nickname == "POSITION3")   {CryoXPos = - 4.7*cm;}
      if((nickname == "POSITION4L") || (nickname == "POSITION4H")) 
	  {CryoXPos = -15.5*cm;}
	// {CryoXPos = -15.697*cm;}
	//	{CryoXPos = -18.0*cm;}
     
      log << MSG::DEBUG << "TB2003 Cryostat Position is " 
	  << nickname << endmsg;;
      log << MSG::DEBUG << "CryoXPos = "  
	  << CryoXPos << endmsg;
      
      //      double CryoXRot = 90.0*deg;
      double CryoXRot = -90.0*deg;
      //double CryoRotateY =  0.0*deg;
      
      H62003EnvelopePhysical->add( new GeoIdentifierTag( 1 ) );
      GeoAlignableTransform  *xf = 
	new GeoAlignableTransform(Transform3D(
                                              Translate3D(CryoXPos,
                                                          CryoYPos,
                                                          trans)
                                              *RotateX3D(CryoXRot)));
 
      double xrot=0,yrot=0,zrot=0;
      if(larTBPos->size()!=0) {
	for (unsigned int i=0;i<larTBPos->size();i++) {
	  if((*larTBPos)[i]->getString("MOVABLE_NAME") == "LARCRYO_EC_POS") {
	    if((*larTBPos)[i]->getString("POSITION_NICKNAME")==nickname){
	      if (((*larTBPos)[i]->getInt("POSITION_NICKNUMBER")==0) || ((*larTBPos)[i]->getInt("POSITION_NICKNUMBER")==nicknumber)) {
		//xt = (*larTBPos)[i]->getDouble("XTRANS")*cm;
		//yt = (*larTBPos)[i]->getDouble("YTRANS")*cm;
		//zt = (*larTBPos)[i]->getDouble("ZTRANS")*cm;
		xrot = (*larTBPos)[i]->getDouble("XROT")*deg;
		yrot = (*larTBPos)[i]->getDouble("YROT")*deg;
		zrot = (*larTBPos)[i]->getDouble("ZROT")*deg;
		log << MSG::DEBUG << "xrot is " << xrot/deg << " degrees"
		    << endmsg; 
		log << MSG::DEBUG << "yrot is " << yrot/deg << " degrees"
		    << endmsg; 
		log << MSG::DEBUG << "zrot is " << zrot/deg << " degrees"
		    << endmsg; 
		// The rotation axis of the cryostat is now the z-axis.
		xf->setDelta(Transform3D(RotateZ3D(yrot)));
	      }
	    }
	  }
	}
      }
      
      H62003EnvelopePhysical->add(xf);
      H62003EnvelopePhysical->add(cryoPhys);
      NH6EP++;
           
      // Add the bathtub
      // The bathtub (and therefore, the FCal) was rotated about the 
      // y-axis in the Cryostat to produce the angle of incidence of 
      // the real ATLAS setup.     

      std::string tubName  = baseName + "::Tub::Envelope";
      const double tubXPos = -9.0*cm; // initial offset of FCal
      const double tubYPos = 1.5*cm;
      const double tubZPos = 7.44*cm;
          
      const double tubXRot = 90.0*deg;
      const double tubYRot = 1.49*deg; // initial rotation of FCal 
      
      const double tubThickness = 0.079*cm; // tub is 1/16 of inch

      const double tubWidth   = 64.75*cm;  // XDim for ends
      const double tubHeighth = 72.75*cm;  // YDim for ends
      const double tubLength  = 84.50*cm; // ZDim for sides and bottom

      const GeoShape* tubEnvelopeShape  = 
	new GeoBox(tubWidth+2*tubThickness,
		   tubHeighth+2*tubThickness, 
		   tubLength);
      const GeoLogVol* tubEnvelopeLog   = 
	new GeoLogVol(tubName,tubEnvelopeShape,LAr);
      //new GeoLogVol(tubName,tubEnvelopeShape,Air);
      GeoPhysVol* tubEnvelopePhys = new GeoPhysVol(tubEnvelopeLog);
      // Recall that in the coords of the cryostat, a transformation
      // in z wrt to the world will be a transformation in -y in the 
      // cryostat and a transformation in y wrt to world will be 
      // a transformation in z wrt to cryostat.      
      LArPhysical->add(new GeoTransform(Translate3D(tubXPos,
                                                    -tubZPos,
                                                    tubYPos)));
      LArPhysical->add(new GeoTransform(RotateX3D(tubXRot)));
      LArPhysical->add(new GeoTransform(RotateY3D(tubYRot)));
      LArPhysical->add(tubEnvelopePhys);

      // Add the ends to the tub
      std::string tubEndName = baseName + "::Tub::End";
      const GeoShape* tubEndShape = 
	new GeoBox(tubWidth,tubHeighth,tubThickness); 
      const GeoLogVol* tubEndLog = 
	new GeoLogVol(tubEndName, tubEndShape, StainlessSteel);
      GeoPhysVol* tubEndPhys = new GeoPhysVol(tubEndLog);

      // All transformations of the tub sides will be w.r.t the 
      // tubEnvelope
      tubEnvelopePhys->add(new GeoTransform(Translate3D(0.0*cm,
                                                        0.0*cm,
					       tubLength-tubThickness)));
      tubEnvelopePhys->add(tubEndPhys);
      tubEnvelopePhys->add(new GeoTransform(Translate3D(0.0*cm,
                                                        0.0*cm,
					       -tubLength+tubThickness)));
      tubEnvelopePhys->add(tubEndPhys);

      // Add the sides to the bathtub
      std::string tubSideName = baseName + "::Tub::Side";
      const GeoShape* tubSideShape = 
	new GeoBox(tubHeighth,tubLength,tubThickness); 
      const GeoLogVol* tubSideLog = 
	new GeoLogVol(tubSideName, tubSideShape, StainlessSteel);
      GeoPhysVol* tubSidePhys = new GeoPhysVol(tubSideLog);

      tubEnvelopePhys->add(new GeoTransform(Translate3D(tubWidth,
                                                        0.0*cm,
                                                        0.0*cm)));
      tubEnvelopePhys->add(new GeoTransform(RotateX3D(90*deg)));
      tubEnvelopePhys->add(new GeoTransform(RotateY3D(90*deg)));
      tubEnvelopePhys->add(tubSidePhys);
      tubEnvelopePhys->add(new GeoTransform(Translate3D(-tubWidth,
                                                        0.0*cm,
                                                        0.0*cm)));
      tubEnvelopePhys->add(new GeoTransform(RotateX3D(90*deg)));
      tubEnvelopePhys->add(new GeoTransform(RotateY3D(90*deg)));
      tubEnvelopePhys->add(tubSidePhys);
      // Add the bottom
      std::string tubBottomName = baseName + "::Tub::Bottom";
      const GeoShape* tubBottomShape = 
	new GeoBox(tubWidth,tubLength,tubThickness); 
      const GeoLogVol* tubBottomLog = 
	new GeoLogVol(tubBottomName, tubBottomShape, StainlessSteel);
      GeoPhysVol* tubBottomPhys = new GeoPhysVol(tubBottomLog);
      tubEnvelopePhys->add(new GeoTransform(Translate3D(0.0*cm,
                                                        -tubHeighth,
                                                        0.0*cm)));
      tubEnvelopePhys->add(new GeoTransform(RotateX3D(90*deg)));
      tubEnvelopePhys->add(tubBottomPhys);

      // FCAL:
      // See notes in LArGeoFcal
      static FCALConstruction fcalConstruction;
      fcalConstruction.setFCALVisLimit(_fcalVisLimit);
      // Only need one FCal    
      bool isPositive(false); // C side for TB 2003; C side is negative
      GeoVFullPhysVol* fcalEnvelope = fcalConstruction.GetEnvelope(isPositive);
      if(fcalEnvelope == 0)
	{
	  throw std::runtime_error("Error getting FCAL envelope");
	}

      // uses LArGeoFcal/FCALConstruction.cxx: */
      IRDBRecordset_ptr fcalMod = 
	pAccessSvc->getRecordsetPtr("FCalMod", detectorKey,detectorNode);
      if (fcalMod->size()==0) {
	fcalMod=pAccessSvc->getRecordsetPtr("FCalMod", "FCalMod-00");
	if (fcalMod->size()==0) {
	  throw std::runtime_error("Error getting FCAL Module parameters from database");
	}
      } 
      
      tubEnvelopePhys->add(new GeoIdentifierTag(1));
      tubEnvelopePhys->add(new GeoTransform(RotateX3D(2*tubXRot)));
      tubEnvelopePhys->add(new GeoTransform(RotateZ3D(2*tubXRot)));
      tubEnvelopePhys->add(fcalEnvelope);

      // The 'beampipe' part of FCal is evacuated in TB2003
      const std::string FCalBeamName = baseName + "::FCalBeamPipe";

      const double FCal1ZStart = (*fcalMod)[0]->getDouble("STARTPOSITION");
      const double FCal3ZStart = (*fcalMod)[2]->getDouble("STARTPOSITION");
      const double FCal3Depth  = (*fcalMod)[2]->getDouble("FULLMODULEDEPTH");
      const double FCal3Stop   = FCal3ZStart + FCal3Depth;
      const double halfDepth   = (FCal3Stop - FCal1ZStart)/2.0;

      const double innerRadius = 0.0*cm;
      const double outerRadius = (*fcalMod)[0]->getDouble("INNERMODULERADIUS");

      GeoTubs *FCalBeamTubs = 
		  new GeoTubs(innerRadius, outerRadius,halfDepth,0,360*deg);
      GeoLogVol* FCalBeamLog = new GeoLogVol(FCalBeamName,FCalBeamTubs,Vac);
      GeoPhysVol* FCalBeamPhys = new GeoPhysVol(FCalBeamLog);

      tubEnvelopePhys->add(new GeoNameTag(FCalBeamName));
      tubEnvelopePhys->add(FCalBeamPhys);

      //  Aluminum plate attached to inside of bathtub
      std::string AlPlateName = baseName + "::AlPlate";
      const double AlXDim = 15.0*cm;
      const double AlYDim = 8.25*cm;
      const double AlZDim = 2.54*cm; // There are two pieces at thickness 25.4mm each
      
      const GeoShape *AlPlateBox  = new GeoBox(AlXDim,AlYDim,AlZDim);
      const GeoShape *AlPlateSlit = new GeoBox(5.0*cm,2.75*cm,AlZDim+0.5*mm); 
      //      GeoShape *AlPlateHole = new GeoTube(0.0*cm,7.0*cm,1.27*cm);    
      
      Translate3D AlSlitOffset(4.5*cm,-5.5*cm,0.0*cm);
      
      const GeoShape & AlPlateShape = 
		  (AlPlateBox->subtract(*AlPlateSlit<<AlSlitOffset));
      
      const GeoLogVol* AlPlateLog = 
		  new GeoLogVol(AlPlateName, &AlPlateShape, Aluminum);
      
      const double AlTrans = tubLength - (tubThickness + AlZDim);      
      GeoPhysVol* AlPlatePhys = new GeoPhysVol(AlPlateLog);

      tubEnvelopePhys->
      	add(new GeoTransform(Translate3D(20.0*cm,  
                                         2.75*cm,
                                         AlTrans)));
      tubEnvelopePhys->add(AlPlatePhys);
      
      // Add Excluder in cryostat
      std::string ExcludertubName = baseName + "::Excluder";
      
      const GeoShape* ExcluderTubShape = 
		  new GeoTrap(16.0*cm,
					  0.0*deg, 0.0*deg,
					  19.0*cm,  26.0*cm,  26.0*cm,  0.0*deg,
					  17.25*cm, 19.75*cm, 19.75*cm, 0.0*deg);

      const GeoShape* ExcluderSlit = new GeoBox(7.0*cm, 16.25*cm, 5.0*cm);
      
      //      Translate3D ExSlitOffset(6.5*cm, 2.75*cm, -27.1*cm);
      Translate3D ExSlitOffset(6.5*cm, -2.75*cm, -11.01*cm);
      
      const GeoShape & ExcluderShape = 
		  (ExcluderTubShape->subtract(*ExcluderSlit<<ExSlitOffset));
      const GeoLogVol* ExcluderTubLog = 
		  new GeoLogVol(ExcludertubName, &ExcluderShape, Rohacell);
      GeoPhysVol* ExcluderTubPhys = 
		  new GeoPhysVol(ExcluderTubLog);
      
      // The excuder is right up against the cryostat
      const double ExTrans = 107.94*cm; // 125.5 - 17.56
      const double ExXPos = 5.5*cm;
      // Recall that in the coords of the cryostat, a transformation
      // in z wrt to the world will be a transformation in -y in the 
      // cryostat and a transformation in y wrt to world will be 
      // a transformation in z wrt to cryostat.
      LArPhysical->add(new GeoTransform(Translate3D(tubXPos+ExXPos,
                                                    -ExTrans,
                                                    tubYPos)));
      LArPhysical->add(new GeoTransform(RotateX3D(tubXRot)));
      LArPhysical->add(new GeoTransform(RotateY3D(tubYRot)));
      LArPhysical->add(ExcluderTubPhys);
    }

  // Beam chambers
  IRDBRecordset_ptr ScintGeom = 
	  pAccessSvc->getRecordsetPtr("LArScintTB",detectorKey, detectorNode); 
  // If nothing is in the database, we don't build. 
  if (ScintGeom->size()!=0) {    
	  
	  log << MSG::DEBUG << "LArH62003:  Creating beam chambers ..." << std::endl;
	  
	  // Scintillators:  S1, S2 are behind cryostat and S3 is in front
	  
	  std::vector<double> v_ScintXDim;
	  std::vector<double> v_ScintYDim;
	  std::vector<double> v_ScintZDim;
	  
	  std::vector<double> v_ScintXPos;
	  std::vector<double> v_ScintYPos;
	  std::vector<double> v_ScintZPos;
	  
	  // Use these values for now; need to update the DB
	  //Scint1
	  v_ScintXDim.push_back(63.4*cm);
	  v_ScintYDim.push_back(63.4*cm);
	  v_ScintZDim.push_back(0.5*cm);
	  v_ScintXPos.push_back(0.0*cm);
	  v_ScintYPos.push_back(5.0*cm);
	  //Scint2
	  v_ScintXDim.push_back(145.0*cm);
	  v_ScintYDim.push_back(145.0*cm);
	  v_ScintZDim.push_back(1.27*cm); // This should be 0.5*cm!
	  v_ScintXPos.push_back(0.0*cm);
	  v_ScintYPos.push_back(-20.0*cm);
	  //Scint3
	  v_ScintXDim.push_back(100.0*cm);
	  v_ScintYDim.push_back(100.0*cm);
	  v_ScintZDim.push_back(0.5*cm);
	  v_ScintXPos.push_back(0.0*cm);
	  v_ScintYPos.push_back(-28.0*cm);
	  // There are three Scintillators
	  // Must put new values into DB
	  for( unsigned int i=0; i < 3; i++)
      {
		  std::string ScintName = baseName + "::Scintillator";
		  /*  Use these values when updated in DB
		  v_ScintXDim.push_back((*ScintGeom)[i]->getDouble("XDIM")*cm);
		  v_ScintYDim.push_back((*ScintGeom)[i]->getDouble("YDIM")*cm);
		  v_ScintZDim.push_back((*ScintGeom)[i]->getDouble("ZDIM")*cm);
		  
		  v_ScintXPos.push_back((*ScintGeom)[i]->getDouble("XPOS")*cm);
		  v_ScintYPos.push_back((*ScintGeom)[i]->getDouble("YPOS")*cm);
		  */
		  v_ScintZPos.push_back((*ScintGeom)[i]->getDouble("ZPOS")*cm);
		  
		  const GeoBox* ScintShape = 
			  new GeoBox(v_ScintXDim[i],v_ScintYDim[i],v_ScintZDim[i] );
		  // Convert integer i into string;
		  std::ostringstream o;
		  o << (i+1);
		  ScintName += o.str(); 
		  const GeoLogVol* ScintLogical = 
			  new GeoLogVol(ScintName,ScintShape,PStyrene);
		  GeoPhysVol* ScintPhysical = new GeoPhysVol(ScintLogical);
		  
		  H62003EnvelopePhysical->add(new GeoIdentifierTag(1));
		  H62003EnvelopePhysical->
			  add(new GeoTransform(Translate3D(v_ScintXPos[i], 
                                                           v_ScintYPos[i], 
                                                           //zside*(trans+v_ScintZPos[i]))));
												  trans-v_ScintZPos[i])));
												  H62003EnvelopePhysical->add( new GeoNameTag(ScintName));
												  H62003EnvelopePhysical->add(ScintPhysical);
												  NH6EP++;
      } 
	  
	  // Scintillators on Table
	  double tableHeight;
	  if(nickname == "POSITION4H")
      {
		  tableHeight = 47.0*mm;
      }
	  else if( nickname == "POSITION4L")
      {
		  tableHeight = -21.0*mm;
      }
	  else
      {
		  tableHeight = 0.0*mm;
      }

	  std::vector<double> v_TableScintXDim;
	  std::vector<double> v_TableScintYDim;
	  std::vector<double> v_TableScintZDim;
	  
	  std::vector<double> v_TableScintXPos;
	  std::vector<double> v_TableScintYPos;
	  std::vector<double> v_TableScintZPos;
	  
	  // Use these values for now; need to update the DB
	  //Scint1
	  v_TableScintXDim.push_back(5.0*cm);
	  v_TableScintYDim.push_back(5.0*cm);
	  v_TableScintZDim.push_back(0.5*cm);
	  v_TableScintXPos.push_back(0.0*cm);
	  v_TableScintYPos.push_back(0.0*cm);
	  //Scint2
	  v_TableScintXDim.push_back(5.0*cm);
	  v_TableScintYDim.push_back(5.0*cm);
	  v_TableScintZDim.push_back(0.5*cm);
	  v_TableScintXPos.push_back(0.0*cm);
//	  v_TableScintYPos.push_back(2.8*cm);
	  v_TableScintYPos.push_back(0.0*cm);
	  //Scint3
	  v_TableScintXDim.push_back(3.5*cm);
	  v_TableScintYDim.push_back(3.5*cm);
	  v_TableScintZDim.push_back(0.5*cm);
	  v_TableScintXPos.push_back(0.0*cm);
//	  v_TableScintYPos.push_back(4.8*cm);
	  v_TableScintYPos.push_back(0.0*cm);
	  
	  // There are three Scintillators on the table so far and the DB ID's start at 3.
	  for( unsigned int i=3; i < 6; i++)
      {
		  /*  Use these values when they get updated in DB
		  v_TableScintXDim.push_back((*ScintGeom)[i]->getDouble("XDIM")*cm);
		  v_TableScintYDim.push_back((*ScintGeom)[i]->getDouble("YDIM")*cm);
		  v_TableScintZDim.push_back((*ScintGeom)[i]->getDouble("ZDIM")*cm);
		  
		  v_TableScintXPos.push_back((*ScintGeom)[i]->getDouble("XPOS")*cm);
		  v_TableScintYPos.push_back((*ScintGeom)[i]->getDouble("YPOS")*cm);
		  */
		  v_TableScintZPos.push_back((*ScintGeom)[i]->getDouble("ZPOS")*cm);
      }
	  
	  for( unsigned int i=0; i < 3; i++)
      {
		  std::string TableScintName = baseName + "::TableScintillator";
		  const GeoBox* TableScintShape = new GeoBox(v_TableScintXDim[i], 
													 v_TableScintYDim[i], 
													 v_TableScintZDim[i] );
		  // Convert integer i into string;
		  std::ostringstream o;
		  o << (i+1);
		  TableScintName += o.str(); 
		  const GeoLogVol* TableScintLogical = 
			  new GeoLogVol(TableScintName,TableScintShape,PStyrene );
		  GeoPhysVol* TableScintPhysical = 
			  new GeoPhysVol( TableScintLogical );
		  
		  //	H62003EnvelopePhysical->add( new GeoIdentifierTag( 1 ) );
		  H62003EnvelopePhysical->
			  add(new GeoTransform(Translate3D(v_TableScintXPos[i], 
                                                           (tableHeight + 
                                                            v_TableScintYPos[i]), 
                                                           trans -  
                                                           v_TableScintZPos[i])));
		  H62003EnvelopePhysical->add(new GeoNameTag(TableScintName));
		  H62003EnvelopePhysical->add(TableScintPhysical);
		  NH6EP++;
      }

	  // Veto Counter
	  std::string VetoName = baseName + "::VetoCounter";
	  const double VetoXDim = 31.7*cm;
	  const double VetoYDim = 31.7*cm;
	  const double VetoZDim =  2.5*cm;
	  
	  const double VetoXPos = 0.0*cm;
	  const double VetoYPos = 0.0*cm;
	  const double VetoZPos = -472.2*cm;
	  
	  const GeoShape* VetoBoxShape = new GeoBox(VetoXDim, VetoYDim, VetoZDim);
	  const GeoShape* VetoHole = new GeoTube(0.0*cm, 3.25*cm,VetoZDim);
	  
	  const GeoShape & VetoShape = 
		  VetoBoxShape->subtract(*VetoHole);
	  
	  const GeoLogVol* VetoLog = new GeoLogVol(VetoName, &VetoShape, PStyrene);
	  GeoPhysVol* VetoPhys = new GeoPhysVol(VetoLog);
	  
	  H62003EnvelopePhysical->
		  add(new GeoTransform(Translate3D(VetoXPos, 
                                                   tableHeight + VetoYPos, 
                                                   trans-VetoZPos)));
	  H62003EnvelopePhysical->add( new GeoNameTag(VetoName));
	  H62003EnvelopePhysical->add(VetoPhys);
	  NH6EP++;
	  
	  // Muon Counter.  Made the same size as the Scintillators for now.
	  const double MuonCtrXDim = (*ScintGeom)[6]->getDouble("XDIM")*cm;
	  const double MuonCtrYDim = (*ScintGeom)[6]->getDouble("YDIM")*cm;
	  const double MuonCtrZDim = (*ScintGeom)[6]->getDouble("ZDIM")*cm;
	  
	  const double MuonCtrXPos = (*ScintGeom)[6]->getDouble("XPOS")*cm;
	  const double MuonCtrYPos = (*ScintGeom)[6]->getDouble("YPOS")*cm;
	  const double MuonCtrZPos = (*ScintGeom)[6]->getDouble("ZPOS")*cm;
	  
	  std::string MuonCtrName = baseName + "::MuonCounter";
	  
	  const GeoBox* MuonCtrShape = new 
		  GeoBox(MuonCtrXDim,MuonCtrYDim,MuonCtrZDim);
	  const GeoLogVol* MuonCtrLogical = 
		  new GeoLogVol(MuonCtrName,MuonCtrShape,PStyrene);
	  GeoPhysVol* MuonCtrPhysical = new GeoPhysVol(MuonCtrLogical);
	  
	  H62003EnvelopePhysical->
		  add(new GeoTransform(Translate3D(MuonCtrXPos, 
                                                   MuonCtrYPos, 
                                                   trans-MuonCtrZPos)));
	  H62003EnvelopePhysical->add( new GeoNameTag(MuonCtrName));
	  H62003EnvelopePhysical->add(MuonCtrPhysical);
	  NH6EP++;
	  
	  // Tail Catcher
	  //Scintillator Component of Tail Catcher
	  const double TCScintXDim = (*ScintGeom)[7]->getDouble("XDIM")*cm;
	  const double TCScintYDim = (*ScintGeom)[7]->getDouble("YDIM")*cm;
	  const double TCScintZDim = (*ScintGeom)[7]->getDouble("ZDIM")*cm;
	  
	  const double TCScintXPos = (*ScintGeom)[7]->getDouble("XPOS")*cm;
	  const double TCScintYPos = (*ScintGeom)[7]->getDouble("YPOS")*cm;
	  const double TCScintZPos = (*ScintGeom)[7]->getDouble("ZPOS")*cm;
	  // double TCScintZPos = 200*cm;
	  
	  std::string TCScintLogName = baseName + "::TCScint";
	  
	  const GeoBox *TCScintBox = 
		  new GeoBox(TCScintXDim, TCScintYDim, TCScintZDim);
	  
	  const GeoLogVol* TCScintLogical = 
		  new GeoLogVol(TCScintLogName,TCScintBox, PStyrene);
	  GeoSerialDenominator *TCScintName = 
		  new GeoSerialDenominator(TCScintLogName);
    
	  // Iron Component of Tail Catcher      
	  const double TCIronXDim = (*ScintGeom)[8]->getDouble("XDIM")*cm;
	  const double TCIronYDim = (*ScintGeom)[8]->getDouble("YDIM")*cm;
	  const double TCIronZDim = (*ScintGeom)[8]->getDouble("ZDIM")*cm;
	  
	  const double TCIronXPos = (*ScintGeom)[8]->getDouble("XPOS")*cm;
	  const double TCIronYPos = (*ScintGeom)[8]->getDouble("YPOS")*cm;
	  const double TCIronZPos = (*ScintGeom)[8]->getDouble("ZPOS")*cm;
	  
	  
	  std::string TCIronLogName = baseName + "::TCIron";
	  const GeoBox *TCIronBox = new GeoBox(TCIronXDim, TCIronYDim, TCIronZDim);
	  
	  const GeoLogVol* TCIronLogical = 
		  new GeoLogVol(TCIronLogName,TCIronBox, Iron);
	  GeoSerialDenominator *TCIronName = 
		  new GeoSerialDenominator(TCIronLogName);
	  
	  H62003EnvelopePhysical->add(TCScintName);      
	  for(int i = 0; i<6; i++)
      {
		  GeoPhysVol* TCScintPhysical = new GeoPhysVol(TCScintLogical);
		  GeoAlignableTransform* TCScintTrans = 
			  new GeoAlignableTransform(Translate3D(TCScintXPos,
                                                                TCScintYPos,
                                                                trans+
                                                                (2*i+1)*(TCIronZDim
                                                                         +TCScintZDim)
                                                                -TCScintZPos));
		  H62003EnvelopePhysical->add(new GeoNameTag(TCScintLogName));
		  H62003EnvelopePhysical->add(TCScintTrans);
		  H62003EnvelopePhysical->add(TCScintPhysical);
		  NH6EP++;
      }
    
	  H62003EnvelopePhysical->add(TCIronName);
	  for(int i = 0; i<6; i++)
      {
		  GeoPhysVol* TCIronPhysical = new GeoPhysVol(TCIronLogical);
		  GeoAlignableTransform* TCIronTrans = 
			  new GeoAlignableTransform(Translate3D(TCIronXPos,
													   TCIronYPos,
													   trans+
													   2*i*(TCScintZDim+TCIronZDim)
													   -TCIronZPos));
		  H62003EnvelopePhysical->add(TCIronTrans);
		  H62003EnvelopePhysical->add(new GeoNameTag(TCIronLogName));
		  H62003EnvelopePhysical->add(TCIronPhysical);
		  NH6EP++;
      }
	  
	  // Iron Wall in front of Cryostat
	  const double IronWallXDim = 100.0*cm;
	  const double IronWallYDim = 100.0*cm;
	  const double IronWallZDim =  20.0*cm;
	  
	  const double IronWallXPos =    0.0*cm;
	  const double IronWallYPos =  -28.0*cm;
	  const double IronWallZPos = -181.0*cm;
	  
	  const double IronWallSlitXDim =  5.0*cm; // This Dim has to be checked
	  const double IronWallSlitYDim = 40.0*cm;
	  const double IronWallSlitZDim = 20.0*cm;
	  
	  const double IronWallSlitXPos =  0.0*cm;
	  const double IronWallSlitYPos = 21.0*cm;
	  const double IronWallSlitZPos =  0.0*cm;
	  
	  std::string IronWallName = baseName + "::IronWall"; 
	  const GeoShape* WallShape = 
		  new GeoBox(IronWallXDim,IronWallYDim,IronWallZDim);
	  const GeoShape* SlitShape = 
		  new GeoBox(IronWallSlitXDim,IronWallSlitYDim,IronWallSlitZDim);
	  
	  Translate3D 
		  SlitOffset(IronWallSlitXPos,IronWallSlitYPos,IronWallSlitZPos);
	  
	  const GeoShape & IronWallShape = 
		  (WallShape->subtract(*SlitShape<<SlitOffset));
	  GeoLogVol* IronWallLogical     = 
		  new GeoLogVol(IronWallName, &IronWallShape, Iron);
	  GeoPhysVol* IronWallPhysical   = new GeoPhysVol(IronWallLogical);
	  
	  H62003EnvelopePhysical->
		  add(new GeoTransform(Translate3D(
                                                   IronWallXPos, 
                                                   IronWallYPos, 
                                                   trans-IronWallZPos)));
	  H62003EnvelopePhysical->add(new GeoNameTag(IronWallName));
	  H62003EnvelopePhysical->add(IronWallPhysical);
	  NH6EP++;
	  
	  // Concrete Walls behind cryostat.    
	  const double ConSizeX = 130.0*cm;
	  const double ConSizeY = 120.0*cm;
	  const double ConSizeZ =  79.3*cm;
	  
	  const double ConAXPos =   0.0*cm;
	  const double ConAYPos =   0.0*cm;
	  const double ConAZPos = 518.0*cm;
	  
	  const double ConBXPos =   0.0*cm;
	  const double ConBYPos =   0.0*cm;
	  const double ConBZPos = 360.0*cm;
	  std::string ConcreteWallAName = baseName + "::ConcreteWallA"; 
	  const GeoBox* ConAShape = new GeoBox(ConSizeX, ConSizeY, ConSizeZ);
	  const GeoLogVol* ConALogical = 
		  new GeoLogVol(ConcreteWallAName,ConAShape,Concrete);
	  GeoPhysVol* ConAPhysical = new GeoPhysVol(ConALogical);
	  
	  H62003EnvelopePhysical->
		  add(new GeoTransform(Translate3D(ConAXPos,
                                                   ConAYPos, 
                                                   trans-ConAZPos)));
	  H62003EnvelopePhysical->add(new GeoNameTag(ConcreteWallAName));
	  H62003EnvelopePhysical->add(ConAPhysical);
	  NH6EP++;
	  
	  std::string ConcreteWallBName = baseName + "::ConcreteWallB"; 
	  const GeoBox* ConBShape = new GeoBox(ConSizeX, ConSizeY, ConSizeZ);
	  const GeoLogVol* ConBLogical = 
		  new GeoLogVol(ConcreteWallBName, ConBShape, Concrete);
	  GeoPhysVol* ConBPhysical = new GeoPhysVol(ConBLogical);
	  
	  H62003EnvelopePhysical->add(new GeoTransform(Translate3D(ConBXPos,
                                                                   ConBYPos, 
                                                                   trans-ConBZPos)));
	  H62003EnvelopePhysical->add(new GeoNameTag(ConcreteWallBName));
	  H62003EnvelopePhysical->add(ConBPhysical);
	  NH6EP++;

	  //For Debugging: Get x,y,z position of beamline detectors
	  for(unsigned int i=0; i < NH6EP; i++)
      { 
		  log << MSG::DEBUG
		  << "The beamline material " 
		  << H62003EnvelopePhysical->getNameOfChildVol(i).c_str() 
		  << " is located at: "
		  << "\n"
		  << "x =  " 
		  << H62003EnvelopePhysical->getXToChildVol(i).getTranslation().x()
		  << "\n"
		  << "y =  " 
		  << H62003EnvelopePhysical->getXToChildVol(i).getTranslation().y()
		  << "\n"
		  << "z =  " 
		  << H62003EnvelopePhysical->getXToChildVol(i).getTranslation().z()
		  << "\n"
		  << endmsg;
      }

  } // End if(ScintGeom.size()!=0)
  
  // End beamline detectors
  
  return H62003EnvelopePhysical;
}

void LArGeo::LArDetectorConstructionH62003::createAxis(GeoFullPhysVol* H62003EnvelopePhysical, GeoMaterial* mat)
{
	std::string baseName = "LAr::TBH62003::";
	double axisXYHalfLength = 2.5*m;
	double axisZHalfLength = 5*m;
	
	GeoTube* ZaxisShape = new GeoTube( 0.*cm, 1.*cm, axisZHalfLength );
	GeoTube* XYaxisShape = new GeoTube( 0.*cm, 1.*cm, axisXYHalfLength );
	
	// x-axis
	std::string XAxisName = baseName + "::XAxis";
	const GeoLogVol* XAxisLogical = new GeoLogVol(XAxisName,XYaxisShape,mat);
	GeoPhysVol* XAxisPhysVol = new GeoPhysVol(XAxisLogical);
	
	H62003EnvelopePhysical->
		add(new GeoTransform(Transform3D(HepRotationY( 90.*deg ), 
                                                 Hep3Vector(axisXYHalfLength, 
                                                            0.*m, 
                                                            0.*m))));
	H62003EnvelopePhysical->add(XAxisPhysVol);
	
	// y-axis
	std::string YAxisName = baseName + "::YAxis";
	const GeoLogVol* YAxisLogical = new GeoLogVol(YAxisName,XYaxisShape,mat);
	GeoPhysVol* YAxisPhysVol = new GeoPhysVol( YAxisLogical );
	
	H62003EnvelopePhysical->
		add(new GeoTransform(Transform3D(HepRotationX( -90.*deg ),
                                                 Hep3Vector(
                                                            0.*m,
                                                            axisXYHalfLength, 
                                                            0.*m))));
	H62003EnvelopePhysical->add(YAxisPhysVol);
	
	//z-axis
	std::string ZAxisName = baseName + "::ZAxis";
	const GeoLogVol* ZAxisLogical = new GeoLogVol(ZAxisName,ZaxisShape,mat);
	GeoPhysVol* ZAxisPhysVol = new GeoPhysVol(ZAxisLogical);
	
	H62003EnvelopePhysical->
		add(new GeoTransform(TranslateZ3D(axisZHalfLength)));
	H62003EnvelopePhysical->add(ZAxisPhysVol);
}
