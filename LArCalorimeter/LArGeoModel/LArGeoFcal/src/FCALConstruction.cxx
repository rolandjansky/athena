/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "LArGeoFcal/FCALConstruction.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoXF.h"

// volumes used:  Pcon, Tubs, Cons, Box, Trap 
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/GeoDBUtils.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
// For functions:
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/ArrayFunction.hh"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBQuery.h"

// For units:
#include "CLHEP/Units/PhysicalConstants.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include <string>
#include <cmath>
#include <cfloat>
#include <sstream>
#include <fstream>
#include <stdexcept>


//===================constructor

LArGeo::FCALConstruction::FCALConstruction():
  m_fcalPhysical(0),
  m_absPhysical1(0),
  m_absPhysical2(0),
  m_absPhysical3(0),
  m_VisLimit(0),
  m_svcLocator(0),
  m_fullGeo(true)
{
}  


//===================destructor

LArGeo::FCALConstruction::~FCALConstruction()
{
}

//================== get envelope

GeoVFullPhysVol* LArGeo::FCALConstruction::GetEnvelope(bool bPos)
{
  IRDBAccessSvc* rdbAccess=0;
   IGeoModelSvc * geoModel;


  if(m_absPhysical1==0) {
    // Access Geometry DB
    m_svcLocator = Gaudi::svcLocator();
    
    if(m_svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
      throw std::runtime_error("Error in FCALConstruction, cannot access GeoModelSvc");
    if(m_svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
      throw std::runtime_error("Error in FCALConstruction, cannot access RDBAccessSvc");
    DecodeVersionKey larVersionKey(geoModel, "LAr");
    
    m_fcalMod = rdbAccess->getRecordsetPtr("FCalMod", larVersionKey.tag(),larVersionKey.node());
    if (m_fcalMod->size()==0) {
      m_fcalMod=rdbAccess->getRecordsetPtr("FCalMod", "FCalMod-00");
      if (m_fcalMod->size()==0) {
	throw std::runtime_error("Error getting FCAL Module parameters from database");
      }
    } 
    
    m_LArPosition  =  rdbAccess->getRecordsetPtr("LArPosition", larVersionKey.tag(), larVersionKey.node());
    if (m_LArPosition->size()==0 ) {
      m_LArPosition = rdbAccess->getRecordsetPtr("LArPosition", "LArPosition-00");
      if (m_LArPosition->size()==0 ) {
	throw std::runtime_error("Error, no lar position table in database!");
      }
    }

  }

  // Flags to turn on volumes.
  const bool F1=true,F2=true,F3=true; 

  StoreGateSvc *detStore;
  if (m_svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FCALConstruction, cannot access DetectorStore");
  }

  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in FCALConstruction, std::Copper is not found.");
  
  GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in FCALConstruction, std::Iron is not found.");
  
  GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in FCALConstruction, std::Lead is not found.");
  
  GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in FCALConstruction, std::LiquidArgon is not found.");
  
  GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in FCALConstruction, std::Air is not found.");
  
  GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in FCALConstruction, std::Kapton is not found.");
  
  GeoMaterial *Glue  = materialManager->getMaterial("LAr::Glue");
  if (!Glue) throw std::runtime_error("Error in FCALConstruction, LAr::Glue is not found.");
  
  GeoMaterial *G10  = materialManager->getMaterial("LAr::G10");
  if (!G10) throw std::runtime_error("Error in FCALConstruction, LAr::G10 is not found.");
  
  
  GeoMaterial *FCal1Absorber = materialManager->getMaterial("LAr::FCal1Absorber");
  if (!FCal1Absorber) throw std::runtime_error("Error in FCALConstruction, LAr::FCal1Absorber is not found.");

  GeoMaterial *FCal23Absorber = materialManager->getMaterial("LAr::FCal23Absorber");
  if (!FCal23Absorber) throw std::runtime_error("Error in FCALConstruction, LAr::FCal23Absorber is not found.");

  GeoMaterial *FCalCableHarness = materialManager->getMaterial("LAr::FCalCableHarness");
  if (!FCalCableHarness) throw std::runtime_error("Error in FCALConstruction, LAr::FCalCableHarness is not found.");

  GeoMaterial *FCal23Slugs = materialManager->getMaterial("LAr::FCal23Slugs");
  if (!FCal23Slugs) throw std::runtime_error("Error in FCALConstruction, LAr::FCal23Slugs is not found.");


  static FCAL_ChannelMap *cmap = new FCAL_ChannelMap(0);

  GeoFullPhysVol *fcalPhysical(NULL);

  std::string baseName = "LAr::FCAL::";

  double startZFCal1 = (*m_fcalMod)[0]->getDouble("STARTPOSITION"); //466.85 * CLHEP::cm;
  //double startZFCal2 = (*m_fcalMod)[1]->getDouble("STARTPOSITION"); //512.83 * CLHEP::cm;
  double startZFCal3 = (*m_fcalMod)[2]->getDouble("STARTPOSITION"); //560.28 * CLHEP::cm;

  double outerModuleRadius1=(*m_fcalMod)[0]->getDouble("OUTERMODULERADIUS");
  double outerModuleRadius2=(*m_fcalMod)[1]->getDouble("OUTERMODULERADIUS");
  double outerModuleRadius3=(*m_fcalMod)[2]->getDouble("OUTERMODULERADIUS");
  double innerModuleRadius1=(*m_fcalMod)[0]->getDouble("INNERMODULERADIUS");
  double innerModuleRadius2=(*m_fcalMod)[1]->getDouble("INNERMODULERADIUS");
  double innerModuleRadius3=(*m_fcalMod)[2]->getDouble("INNERMODULERADIUS");
  double fullModuleDepth1=(*m_fcalMod)[0]->getDouble("FULLMODULEDEPTH");
  double fullModuleDepth2=(*m_fcalMod)[1]->getDouble("FULLMODULEDEPTH");
  double fullModuleDepth3=(*m_fcalMod)[2]->getDouble("FULLMODULEDEPTH");
  double fullGapDepth1=(*m_fcalMod)[0]->getDouble("FULLGAPDEPTH");
  double fullGapDepth2=(*m_fcalMod)[1]->getDouble("FULLGAPDEPTH");
  double fullGapDepth3=(*m_fcalMod)[2]->getDouble("FULLGAPDEPTH");
  double outerGapRadius1=(*m_fcalMod)[0]->getDouble("OUTERGAPRADIUS");
  double outerGapRadius2=(*m_fcalMod)[1]->getDouble("OUTERGAPRADIUS");
  double outerGapRadius3=(*m_fcalMod)[2]->getDouble("OUTERGAPRADIUS");
  double innerGapRadius1=(*m_fcalMod)[0]->getDouble("INNERGAPRADIUS");
  double innerGapRadius2=(*m_fcalMod)[1]->getDouble("INNERGAPRADIUS");
  double innerGapRadius3=(*m_fcalMod)[2]->getDouble("INNERGAPRADIUS");


  // FCAL VOLUME.  IT DOES NOT INCLUDE THE COPPER PLUG, ONLY THE LAR AND MODS 1-3
  {

    double outerRadius = std::max(outerModuleRadius1,std::max(outerModuleRadius2,outerModuleRadius3));
    double innerRadius = std::min(innerModuleRadius1,std::min(innerModuleRadius2,innerModuleRadius3));
    double depthZFCal3 = fullModuleDepth3;
    double stopZFCal3  = startZFCal3 + depthZFCal3;
    
    double totalDepth  = stopZFCal3 - startZFCal1;
    double halfDepth   = totalDepth/2.;

    std::string name = baseName + "LiquidArgonC";
    GeoTubs *tubs = new GeoTubs(innerRadius,outerRadius,halfDepth,0,360*CLHEP::deg);
    GeoLogVol *logVol= new GeoLogVol(name, tubs, LAr);
    fcalPhysical = new GeoFullPhysVol(logVol);
  }

 

  if (F1) 
    {

      // Module 1
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fullModuleDepth1/2;
	double innerRadius     = innerModuleRadius1;
	double outerRadius     = outerModuleRadius1;
	GeoFullPhysVol *physVol;

	if(m_absPhysical1)
	  physVol = m_absPhysical1->clone();
	else
	{
	  GeoTubs *tubs          = new GeoTubs( innerRadius, outerRadius, halfDepth, 0, 2*M_PI);
	  GeoLogVol  *logVol     = new GeoLogVol(baseName + "Module1::Absorber", tubs, FCal1Absorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	// Alignable transform
	
	const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(m_LArPosition, bPos ? "FCAL1_POS":"FCAL1_NEG");
	if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	HepGeom::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	GeoAlignableTransform *xfAbs1 = new GeoAlignableTransform(xfPos);
	
	fcalPhysical->add(xfAbs1);
	if (!bPos)  fcalPhysical->add(new GeoTransform(HepGeom::RotateY3D(180*CLHEP::deg)));
	fcalPhysical->add(physVol);
	modPhysical = physVol;
	
	std::string tag = bPos? std::string("FCAL1_POS") : std::string("FCAL1_NEG");
	StatusCode status;
	
	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
	StoredAlignX *sAlignX = new StoredAlignX(xfAbs1);
	status=detStore->record(sAlignX,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
      }   
      // 16 Troughs representing  Cable Harnesses:
      if(m_fullGeo)
	if(m_absPhysical1==0) {
	  double troughDepth       = 1.0 * CLHEP::cm;
	  double outerRadius       = outerModuleRadius1;
	  double innerRadius       = outerRadius - troughDepth;
	  double halfLength        = fullModuleDepth1/ 2.0;
	  double deltaPhi          = 5.625 * CLHEP::deg;
	  double startPhi          = 11.25 * CLHEP::deg - deltaPhi/2.0;
	  GeoTubs * tubs = new GeoTubs(innerRadius,outerRadius,halfLength,startPhi,deltaPhi );
	  GeoLogVol *logVol = new GeoLogVol(baseName+"Module1::CableTrough",tubs,FCalCableHarness);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  Genfun::Variable i;
	  Genfun::GENFUNCTION rotationAngle = 22.5*CLHEP::deg*i;
	  GeoXF::TRANSFUNCTION xf = GeoXF::Pow(HepGeom::RotateZ3D(1.0),rotationAngle);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol,&xf,16);
	  modPhysical->add(st);
	}

      if(m_absPhysical1==0) {
	  double halfDepth    = fullGapDepth1/2.0;
	  double innerRadius  = innerGapRadius1;
	  double outerRadius  = outerGapRadius1;
	  GeoPhysVol *physVol = 0;
	  if(m_fullGeo) {
	    GeoTubs *tubs       = new GeoTubs(innerRadius,outerRadius,halfDepth,0.0, 2.0*M_PI);
	    GeoLogVol *logVol   = new GeoLogVol(baseName + "Module1::Gap",tubs, LAr);
	    physVol = new GeoPhysVol(logVol);
	    modPhysical->add(new GeoSerialIdentifier(0));
	  }
	  
	  int counter=0;	  
	  // Electrodes:
	  int myGroup=1;

	  // Field names
	  unsigned fieldModNumber(2); //std::string fieldModNumber("LARFCALELECTRODES_DATA.MODNUMBER");
	  unsigned fieldTileName(1); //std::string fieldTileName("LARFCALELECTRODES_DATA.TILENAME");
	  unsigned fieldI(4); //std::string fieldI("LARFCALELECTRODES_DATA.I");
	  unsigned fieldJ(5); //std::string fieldJ("LARFCALELECTRODES_DATA.J");
	  unsigned fieldId(3); //std::string fieldId("LARFCALELECTRODES_DATA.ID");
	  unsigned fieldX(6); //std::string fieldX("LARFCALELECTRODES_DATA.X");
	  unsigned fieldY(7); //std::string fieldY("LARFCALELECTRODES_DATA.Y");
	  unsigned fieldHvFt(8); //std::string fieldHvFt("LARFCALELECTRODES_DATA.HVFEEDTHROUGHID");

          IRDBQuery* query(0);
          if (!rdbAccess)  throw std::runtime_error("Error, no rdbAccess");
          if (!geoModel)  throw std::runtime_error("Error, no geoModel");
          DecodeVersionKey larVersionKey(geoModel, "LAr");
          query = rdbAccess->getQuery("LArFCalElectrodes", larVersionKey.tag(),larVersionKey.node());
          if(!query) {
            query = rdbAccess->getQuery("LArFCalElectrodes", "LArFCalElectrodes-00");
            if(!query)
              throw std::runtime_error("Error getting Session and Query pointers");
          }
          query->execute();
          if(query->size()==0)
            throw std::runtime_error("Error, unable to fetch fcal electrodes from the database!");


	  while(query->next()) {

	    if(myGroup!=query->data<int>(fieldModNumber)) continue;

//	    std::string thisTileStr=row["LARFCALELECTRODES_DATA.TILENAME"].data<std::string>();
	    int    thisTubeI=query->data<int>(fieldI);
	    int    thisTubeJ= query->data<int>(fieldJ);
	    int    thisTubeID = query->data<int>(fieldId);
	    int    thisTubeMod = myGroup;
	    double thisTubeX= query->data<double>(fieldX);
	    double thisTubeY= query->data<double>(fieldY);
//	    std::string thisHVft=row["LARFCALELECTRODES_DATA.HVFEEDTHROUGHID"].data<std::string>();
	    
	    cmap->add_tube(query->data<std::string>(fieldTileName), 
			   thisTubeMod, thisTubeID, thisTubeI,thisTubeJ, thisTubeX, thisTubeY, 
			   query->data<std::string>(fieldHvFt));/// Gabe: New add_tube
	    
	    if (m_VisLimit != -1 && (counter++ > m_VisLimit)) continue;
	    if(m_fullGeo) {	      
	      GeoTransform *xf = new GeoTransform(HepGeom::Translate3D(thisTubeX*CLHEP::cm, thisTubeY*CLHEP::cm,0));
	      modPhysical->add(xf);
	      modPhysical->add(physVol);
	    }
	  }

	  m_absPhysical1 = modPhysical;
         if(query) {
           query->finalize();
           delete query;
         }

      } 
  }  // if (F1)
  if (F2) 
    {
      // Module 2
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fullModuleDepth2/2;
	double innerRadius     = innerModuleRadius2;
	double outerRadius     = outerModuleRadius2;
	GeoFullPhysVol *physVol;
	
	if(m_absPhysical2)
	  physVol = m_absPhysical2->clone();
	else
	  {
	    GeoTubs *tubs          = new GeoTubs( innerRadius, outerRadius, halfDepth, 0, 2*M_PI);
	  GeoLogVol  *logVol     = new GeoLogVol(baseName + "Module2::Absorber", tubs, FCal23Absorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	// Alignable transform

	const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(m_LArPosition, bPos ? "FCAL2_POS":"FCAL2_NEG");
	if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	HepGeom::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	GeoAlignableTransform *xfAbs2 = new GeoAlignableTransform(xfPos);
	
	fcalPhysical->add(xfAbs2);
	if (!bPos)  fcalPhysical->add(new GeoTransform(HepGeom::RotateY3D(180*CLHEP::deg)));
	fcalPhysical->add(physVol);
	modPhysical = physVol;
	
	std::string tag = bPos? std::string("FCAL2_POS") : std::string("FCAL2_NEG");
	StatusCode status;
	
	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
	StoredAlignX *sAlignX = new StoredAlignX(xfAbs2);
	status=detStore->record(sAlignX,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
      }   
      // 16 Troughs representing  Cable Harnesses:
      if(m_fullGeo)
	if(m_absPhysical2==0) {
	  double troughDepth       = 1.0 * CLHEP::cm;
	  double outerRadius       = outerModuleRadius2;
	  double innerRadius       = outerRadius - troughDepth;
	  double halfLength        = fullModuleDepth2/ 2.0;
	  double deltaPhi          = 5.625 * CLHEP::deg;
	  double startPhi          = 11.25 * CLHEP::deg - deltaPhi/2.0;
	  GeoTubs * tubs = new GeoTubs(innerRadius,outerRadius,halfLength,startPhi,deltaPhi );
	  GeoLogVol *logVol = new GeoLogVol(baseName+"Module2::CableTrough",tubs,FCalCableHarness);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  Genfun::Variable i;
	  Genfun::GENFUNCTION rotationAngle = 22.5*CLHEP::deg*i;
	  GeoXF::TRANSFUNCTION xf = GeoXF::Pow(HepGeom::RotateZ3D(1.0),rotationAngle);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol,&xf,16);
	  modPhysical->add(st);
	}
      
      // Electrodes:
      if(m_absPhysical2==0) {
	double halfDepth    = fullGapDepth2/2.0;
	double innerRadius  = innerGapRadius2;
	double outerRadius  = outerGapRadius2;
	
	GeoPhysVol *gapPhys    = 0;
	GeoPhysVol *rodPhys    = 0;
	if(m_fullGeo) {
	  GeoTubs *gapTubs       = new GeoTubs(0,outerRadius,halfDepth,0.0, 2.0*M_PI);
	  GeoLogVol *gapLog      = new GeoLogVol(baseName + "Module2::Gap",gapTubs, LAr);
	  gapPhys    = new GeoPhysVol(gapLog);

	  GeoTubs *rodTubs       = new GeoTubs(0,innerRadius,halfDepth,0.0, 2.0*M_PI);
	  GeoLogVol *rodLog      = new GeoLogVol(baseName + "Module2::Rod",rodTubs, FCal23Slugs);
	  rodPhys    = new GeoPhysVol(rodLog);
	  gapPhys->add(rodPhys);
	  modPhysical->add(new GeoSerialIdentifier(0));
	}
	
	int counter=0;
	
	int myGroup=2;

	// Field names
	unsigned fieldModNumber(2); //std::string fieldModNumber("LARFCALELECTRODES_DATA.MODNUMBER");
	unsigned fieldTileName(1); //std::string fieldTileName("LARFCALELECTRODES_DATA.TILENAME");
	unsigned fieldI(4); //std::string fieldI("LARFCALELECTRODES_DATA.I");
	unsigned fieldJ(5); //std::string fieldJ("LARFCALELECTRODES_DATA.J");
	unsigned fieldId(3); //std::string fieldId("LARFCALELECTRODES_DATA.ID");
	unsigned fieldX(6); //std::string fieldX("LARFCALELECTRODES_DATA.X");
	unsigned fieldY(7); //std::string fieldY("LARFCALELECTRODES_DATA.Y");
	unsigned fieldHvFt(8); //std::string fieldHvFt("LARFCALELECTRODES_DATA.HVFEEDTHROUGHID");

        IRDBQuery* query(0);
        if (!rdbAccess)  throw std::runtime_error("Error, no rdbAccess");
        if (!geoModel)  throw std::runtime_error("Error, no geoModel");
        DecodeVersionKey larVersionKey(geoModel, "LAr"); 
        query = rdbAccess->getQuery("LArFCalElectrodes", larVersionKey.tag(),larVersionKey.node());
        if(!query) {
          query = rdbAccess->getQuery("LArFCalElectrodes", "LArFCalElectrodes-00");
          if(!query)
            throw std::runtime_error("Error getting Session and Query pointers");
        }
        query->execute();
        if(query->size()==0)
          throw std::runtime_error("Error, unable to fetch fcal electrodes from the database!");

        while(query->next()) {

          if(myGroup!=query->data<int>(fieldModNumber)) continue;


//	    std::string thisTileStr=row["LARFCALELECTRODES_DATA.TILENAME"].data<std::string>();
	    int    thisTubeI=query->data<int>(fieldI);
	    int    thisTubeJ= query->data<int>(fieldJ);
	    int    thisTubeID = query->data<int>(fieldId);
	    int    thisTubeMod = myGroup;
	    double thisTubeX= query->data<double>(fieldX);
	    double thisTubeY= query->data<double>(fieldY);
//	    std::string thisHVft=row["LARFCALELECTRODES_DATA.HVFEEDTHROUGHID"].data<std::string>();
	    
	    cmap->add_tube(query->data<std::string>(fieldTileName), 
			   thisTubeMod, thisTubeID, thisTubeI,thisTubeJ, thisTubeX, thisTubeY, 
			   query->data<std::string>(fieldHvFt));/// Gabe: New add_tube
	    
	    if (m_VisLimit != -1 && (counter++ > m_VisLimit)) continue;
	    if(m_fullGeo) {	      
	      GeoTransform *xf = new GeoTransform(HepGeom::Translate3D(thisTubeX*CLHEP::cm, thisTubeY*CLHEP::cm,0));
	      modPhysical->add(xf);
	      modPhysical->add(gapPhys);
	    }
	}

	m_absPhysical2 = modPhysical;
        if(query) {
           query->finalize();
           delete query;
        }

      }
  } // if (F2)

  if (F3) 
    {
      // Module 3
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fullModuleDepth3/2;
	double innerRadius     = innerModuleRadius3;
	double outerRadius     = outerModuleRadius3;
	GeoFullPhysVol *physVol;

	if(m_absPhysical3)
	  physVol = m_absPhysical3->clone();
	else
	{
	  GeoTubs *tubs          = new GeoTubs( innerRadius, outerRadius, halfDepth, 0, 2*M_PI);
	  GeoLogVol  *logVol     = new GeoLogVol(baseName + "Module3::Absorber", tubs, FCal23Absorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	// Alignable transform
	const IRDBRecord *posRec = GeoDBUtils::getTransformRecord(m_LArPosition, bPos ? "FCAL3_POS":"FCAL3_NEG");
	if (!posRec) throw std::runtime_error("Error, no lar position record in the database") ;
	HepGeom::Transform3D xfPos = GeoDBUtils::getTransform(posRec);
	GeoAlignableTransform *xfAbs3 = new GeoAlignableTransform(xfPos);
	
	fcalPhysical->add(xfAbs3);
	if (!bPos)  fcalPhysical->add(new GeoTransform(HepGeom::RotateY3D(180*CLHEP::deg)));
	fcalPhysical->add(physVol);
	modPhysical = physVol;

	std::string tag = bPos? std::string("FCAL3_POS") : std::string("FCAL3_NEG");
	StatusCode status;

	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());

	StoredAlignX *sAlignX = new StoredAlignX(xfAbs3);
	status=detStore->record(sAlignX,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());

	
      }   
      // 16 Troughs representing  Cable Harnesses:
      if(m_fullGeo)
	if(m_absPhysical3==0) {
	  static double rotAngles[] =
	    { 11.25 * CLHEP::deg,
	      22.50 * CLHEP::deg,
	      45.00 * CLHEP::deg,
	      56.25 * CLHEP::deg,
	      67.50 * CLHEP::deg,
	      90.00 * CLHEP::deg,  // first quarter
	      101.25 * CLHEP::deg,
	      112.50 * CLHEP::deg,
	      135.00 * CLHEP::deg,
	      146.25 * CLHEP::deg,
	      157.50 * CLHEP::deg,
	      180.00 * CLHEP::deg,  // second quarter
	      191.25 * CLHEP::deg,
	      202.50 * CLHEP::deg,
	      225.00 * CLHEP::deg,
	      236.25 * CLHEP::deg,
	      247.50 * CLHEP::deg,
	      270.00 * CLHEP::deg,  // third quarter
	      281.25 * CLHEP::deg,
	      292.50 * CLHEP::deg,
	      315.00 * CLHEP::deg,
	      326.25 * CLHEP::deg,
	      337.50 * CLHEP::deg,
	      360.00 * CLHEP::deg };
	
	  Genfun::ArrayFunction rotationAngle(rotAngles,rotAngles+24);
	  double troughDepth       = 1.0 * CLHEP::cm;
	  double outerRadius       = outerModuleRadius3;
	  double innerRadius       = outerRadius - troughDepth;
	  double halfLength        = fullModuleDepth3/ 2.0;
	  double deltaPhi          = 5.625 * CLHEP::deg;
	  double startPhi          = 11.25 * CLHEP::deg - deltaPhi/2.0;
	  GeoTubs * tubs = new GeoTubs(innerRadius,outerRadius,halfLength,startPhi,deltaPhi );
	  GeoLogVol *logVol = new GeoLogVol(baseName+"Module3::CableTrough",tubs,FCalCableHarness);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  GeoXF::TRANSFUNCTION xf = GeoXF::Pow(HepGeom::RotateZ3D(1.0),rotationAngle);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol,&xf,24);
	  modPhysical->add(st);
	}
      
      // Electrodes:
      if(m_absPhysical3==0)
      {
	
	
	double halfDepth    = fullGapDepth3/2.0;
	double innerRadius  = innerGapRadius3;
	double outerRadius  = outerGapRadius3;
	
	GeoPhysVol *gapPhys    = 0;
	GeoPhysVol *rodPhys    = 0;
	if(m_fullGeo) {
	  GeoTubs *gapTubs       = new GeoTubs(0,outerRadius,halfDepth,0.0, 2.0*M_PI);
	  GeoLogVol *gapLog      = new GeoLogVol(baseName + "Module3::Gap",gapTubs, LAr);
	  gapPhys    = new GeoPhysVol(gapLog);

	  GeoTubs *rodTubs       = new GeoTubs(0,innerRadius,halfDepth,0.0, 2.0*M_PI);
	  GeoLogVol *rodLog      = new GeoLogVol(baseName + "Module3::Rod",rodTubs, FCal23Slugs);
	  rodPhys    = new GeoPhysVol(rodLog);
	  gapPhys->add(rodPhys);
	  modPhysical->add(new GeoSerialIdentifier(0));
	}

	int counter=0;
	
	int myGroup=3;

	// Field names
	unsigned fieldModNumber(2); //std::string fieldModNumber("LARFCALELECTRODES_DATA.MODNUMBER");
	unsigned fieldTileName(1); //std::string fieldTileName("LARFCALELECTRODES_DATA.TILENAME");
	unsigned fieldI(4); //std::string fieldI("LARFCALELECTRODES_DATA.I");
	unsigned fieldJ(5); //std::string fieldJ("LARFCALELECTRODES_DATA.J");
	unsigned fieldId(3); //std::string fieldId("LARFCALELECTRODES_DATA.ID");
	unsigned fieldX(6); //std::string fieldX("LARFCALELECTRODES_DATA.X");
	unsigned fieldY(7); //std::string fieldY("LARFCALELECTRODES_DATA.Y");
	unsigned fieldHvFt(8); //std::string fieldHvFt("LARFCALELECTRODES_DATA.HVFEEDTHROUGHID");

        IRDBQuery* query(0);
        if (!rdbAccess)  throw std::runtime_error("Error, no rdbAccess");
        if (!geoModel)  throw std::runtime_error("Error, no geoModel");
        DecodeVersionKey larVersionKey(geoModel, "LAr"); 
        query = rdbAccess->getQuery("LArFCalElectrodes", larVersionKey.tag(),larVersionKey.node());
        if(!query) {
          query = rdbAccess->getQuery("LArFCalElectrodes", "LArFCalElectrodes-00");
          if(!query)
            throw std::runtime_error("Error getting Session and Query pointers");
        }
        query->execute();
        if(query->size()==0)
          throw std::runtime_error("Error, unable to fetch fcal electrodes from the database!");

        while(query->next()) {

          if(myGroup!=query->data<int>(fieldModNumber)) continue;

//	    std::string thisTileStr=row["LARFCALELECTRODES_DATA.TILENAME"].data<std::string>();
	    int    thisTubeI=query->data<int>(fieldI);
	    int    thisTubeJ= query->data<int>(fieldJ);
	    int    thisTubeID = query->data<int>(fieldId);
	    int    thisTubeMod = myGroup;
	    double thisTubeX= query->data<double>(fieldX);
	    double thisTubeY= query->data<double>(fieldY);
//	    std::string thisHVft=row["LARFCALELECTRODES_DATA.HVFEEDTHROUGHID"].data<std::string>();
	    
	    cmap->add_tube(query->data<std::string>(fieldTileName), 
			   thisTubeMod, thisTubeID, thisTubeI,thisTubeJ, thisTubeX, thisTubeY, 
			   query->data<std::string>(fieldHvFt));/// Gabe: New add_tube
	    
	    if (m_VisLimit != -1 && (counter++ > m_VisLimit)) continue;
	    if(m_fullGeo) {	      
	      GeoTransform *xf = new GeoTransform(HepGeom::Translate3D(thisTubeX*CLHEP::cm, thisTubeY*CLHEP::cm,0));
	      modPhysical->add(xf);
	      modPhysical->add(gapPhys);
	    }
	}

	m_absPhysical3 = modPhysical;
        if(query) {
           query->finalize();
           delete query;
        }

      }
    } // if (F3)
  
  // Did I already store it?
  //FCAL_ChannelMap *aChannelMap(NULL);
  //  if (detStore->retrieve(aChannelMap)==StatusCode::FAILURE) {
  if (!detStore->contains<FCAL_ChannelMap>("FCAL_ChannelMap")) {
    cmap->finish();
    StatusCode status=detStore->record(cmap,"FCAL_ChannelMap");
    if(!status.isSuccess()) throw std::runtime_error ("Cannot store FCAL_ChannelMap");
  }

  return fcalPhysical;
}

void LArGeo::FCALConstruction::setFullGeo(bool flag)
{
  m_fullGeo = flag;
}
