/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/FCAL_ChannelMap.h"
#include "FCALConstructionH62004.h"

#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoPerfUtils.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/Units.h"

// volumes used:  Pcon, Tubs, Cons, Box, Trap 
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/GeoDBUtils.h"

// For functions:
#include "GeoGenericFunctions/Abs.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "GeoGenericFunctions/Sqrt.h"
#include "GeoGenericFunctions/ATan.h"
#include "GeoGenericFunctions/Rectangular.h"
#include "GeoGenericFunctions/Mod.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/FixedConstant.h"
#include "GeoGenericFunctions/ArrayFunction.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// For units:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <string>
#include <cmath>
#include <cfloat>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
 

//===================constructor

LArGeo::FCALConstructionH62004::FCALConstructionH62004():
  m_absPhysical1(0),
  m_absPhysical2(0),
  m_absPhysical3(0),
  m_VisLimit(0)
{
  m_svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  
  if(m_svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in FCALConstructionH62004, cannot access RDBAccessSvc");
  DecodeVersionKey larVersionKey("LAr");

  m_fcalElectrode = rdbAccess->getRecordsetPtr("LArFCalElectrodes","LArFCalElectrodes-H6-00");
  if (m_fcalElectrode->size()==0)
    throw std::runtime_error("Error getting FCAL electrode from database");

  m_fcalMod = rdbAccess->getRecordsetPtr("FCalMod", larVersionKey.tag(),larVersionKey.node());
  if (m_fcalMod->size()==0) {
    m_fcalMod=rdbAccess->getRecordsetPtr("FCalMod", "FCalMod-00");
    if (m_fcalMod->size()==0) {
      throw std::runtime_error("Error getting FCAL Module parameters from database");
    }
  } 

}  


//===================destructor

LArGeo::FCALConstructionH62004::~FCALConstructionH62004()
{
}

//================== get envelope

GeoVFullPhysVol* LArGeo::FCALConstructionH62004::GetEnvelope()
{

  // Flags to turn on volumes.
  const bool F1=true,F2=true,F3=true; // F3 is a cold tail-catcher here
  /*
  struct Electrode_t {
    std::string name;
    unsigned int id;
    int i;
    int j;
    double x;
    double y;
  };
  */

  struct FCALData_t
  {
    double      innerModuleRadius;
    double      outerModuleRadius;
    double      fullModuleDepth;
    double      innerGapRadius;
    double      outerGapRadius;
    double      fullGapDepth;
    int         FCalSampling;
  };

  
  IMessageSvc * msgSvc;
  if (m_svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FCALConstructionH62004, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "FCALConstructionH62004"); 
  
  log << MSG::INFO;
  
  log  << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of FCAL GeoModel definition        +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  StoreGateSvc *detStore;
  if (m_svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in FCALConstructionH62004, cannot access DetectorStore");
  }




  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return NULL;
  
  const GeoMaterial *Copper  = materialManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in FCALConstructionH62004, std::Copper is not found.");
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in FCALConstructionH62004, std::Iron is not found.");
  
  const GeoMaterial *Lead  = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in FCALConstructionH62004, std::Lead is not found.");
  
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in FCALConstructionH62004, std::LiquidArgon is not found.");
  
  const GeoMaterial *Air  = materialManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in FCALConstructionH62004, std::Air is not found.");
  
  const GeoMaterial *Kapton  = materialManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in FCALConstructionH62004, std::Kapton is not found.");
  
  const GeoMaterial *Glue  = materialManager->getMaterial("LAr::Glue");
  if (!Glue) throw std::runtime_error("Error in FCALConstructionH62004, LAr::Glue is not found.");
  
  const GeoMaterial *G10  = materialManager->getMaterial("LAr::G10");
  if (!G10) throw std::runtime_error("Error in FCALConstructionH62004, LAr::G10 is not found.");
  
  
  const GeoMaterial *FCal1Absorber = materialManager->getMaterial("LAr::FCal1Absorber");
  if (!FCal1Absorber) throw std::runtime_error("Error in FCALConstructionH62004, LAr::FCal1Absorber is not found.");

  const GeoMaterial *FCal23Absorber = materialManager->getMaterial("LAr::FCal23Absorber");
  if (!FCal23Absorber) throw std::runtime_error("Error in FCALConstructionH62004, LAr::FCal23Absorber is not found.");

  const GeoMaterial *FCalCableHarness = materialManager->getMaterial("LAr::FCalCableHarness");
  if (!FCalCableHarness) throw std::runtime_error("Error in FCALConstructionH62004, LAr::FCalCableHarness is not found.");

  const GeoMaterial *FCal23Slugs = materialManager->getMaterial("LAr::FCal23Slugs");
  if (!FCal23Slugs) throw std::runtime_error("Error in FCALConstructionH62004, LAr::FCal23Slugs is not found.");


  FCALData_t fcalData[3];
  for(int i=0; i<3; ++i) {
     fcalData[i].innerModuleRadius = (*m_fcalMod)[i]->getDouble("INNERMODULERADIUS");
     fcalData[i].outerModuleRadius = (*m_fcalMod)[i]->getDouble("OUTERMODULERADIUS");
     fcalData[i].fullModuleDepth = (*m_fcalMod)[i]->getDouble("FULLMODULEDEPTH");
     fcalData[i].innerGapRadius = (*m_fcalMod)[i]->getDouble("INNERGAPRADIUS");
     fcalData[i].outerGapRadius = (*m_fcalMod)[i]->getDouble("OUTERGAPRADIUS");
     fcalData[i].fullGapDepth = (*m_fcalMod)[i]->getDouble("FULLGAPDEPTH");
     fcalData[i].FCalSampling = (*m_fcalMod)[i]->getInt("FCALSAMPLING");
  }

  static FCAL_ChannelMap *cmap = new FCAL_ChannelMap(0);

  GeoFullPhysVol *fcalPhysical(NULL);

  std::string baseName = "LAr::FCAL::";

  double fcalHalfDepth=0;
  double startZFCal1 = (*m_fcalMod)[0]->getDouble("STARTPOSITION"); //466.85 * Gaudi::Units::cm;
  double startZFCal2 = (*m_fcalMod)[1]->getDouble("STARTPOSITION"); //512.83 * Gaudi::Units::cm;
  double startZFCal3 = (*m_fcalMod)[2]->getDouble("STARTPOSITION"); //560.28 * Gaudi::Units::cm;

  // Should go to Db (change FCalNominals ????)
  double fcalstartPhi = 90.*Gaudi::Units::deg;
  double fcaldeltaPhi = 90.*Gaudi::Units::deg;
  // FCAL VOLUME.  IT DOES NOT INCLUDE THE COPPER PLUG, ONLY THE LAR AND MODS 1-3
  {

    double outerRadius = std::max(fcalData[0].outerModuleRadius,std::max(fcalData[1].outerModuleRadius,fcalData[2].outerModuleRadius));
    double innerRadius = std::min(fcalData[0].innerModuleRadius,std::min(fcalData[1].innerModuleRadius,fcalData[2].innerModuleRadius));
    double depthZFCal3 = fcalData[2].fullModuleDepth;
    double stopZFCal3  = startZFCal3 + depthZFCal3;
    
    double totalDepth  = stopZFCal3 - startZFCal1;
    double halfDepth   = totalDepth/2.;

    std::string name = baseName + "LiquidArgonC";
    GeoTubs *tubs = new GeoTubs(innerRadius,outerRadius,halfDepth, 0.99*fcalstartPhi, 1.01*fcaldeltaPhi);
    GeoLogVol *logVol= new GeoLogVol(name, tubs, LAr);
    fcalPhysical = new GeoFullPhysVol(logVol);

    fcalHalfDepth = halfDepth;
  }

 
  if (F1) 
    {
      // Module 1
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fcalData[0].fullModuleDepth/2;
	double innerRadius     = fcalData[0].innerModuleRadius;
	double outerRadius     = fcalData[0].outerModuleRadius;
	GeoFullPhysVol *physVol;

	if(m_absPhysical1)
	  physVol = m_absPhysical1->clone();
	else
	{
	  GeoTubs *tubs          = new GeoTubs( innerRadius, outerRadius, halfDepth, fcalstartPhi, fcaldeltaPhi);
	  GeoLogVol  *logVol     = new GeoLogVol(baseName + "Module1::Absorber", tubs, FCal1Absorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	fcalPhysical->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,-(fcalHalfDepth-halfDepth))));
	fcalPhysical->add(physVol);
	modPhysical = physVol;
	
	std::string tag = std::string("FCAL1") ;
	StatusCode status;
	
	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
      }   
      // 16 Troughs representing  Cable Harnesses:
      if(m_absPhysical1==0)
	{
	  double troughDepth       = 0.9999 * Gaudi::Units::cm;
	  double outerRadius       = fcalData[0].outerModuleRadius;
	  double innerRadius       = outerRadius - troughDepth;
	  double halfLength        = fcalData[0].fullModuleDepth/ 2.0;
	  double deltaPhi          = 5.625 * Gaudi::Units::deg;
	  double startPhi          = 11.25 * Gaudi::Units::deg - deltaPhi/2.0;
	  GeoTubs * tubs = new GeoTubs(innerRadius,outerRadius,halfLength,startPhi,deltaPhi );
	  GeoLogVol *logVol = new GeoLogVol(baseName+"Module1::CableTrough",tubs,FCalCableHarness);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  GeoGenfun::Variable i;
	  GeoGenfun::GENFUNCTION rotationAngle = fcalstartPhi + 22.5*Gaudi::Units::deg*i;
	  GeoXF::TRANSFUNCTION xf = GeoXF::Pow(GeoTrf::RotateZ3D(1.0),rotationAngle);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol,&xf,4);
	  modPhysical->add(st);
	}
      
      if (m_absPhysical1==0) // Until fixed. (m_absPhysical1==0)
	{
	  double halfDepth    = fcalData[0].fullGapDepth/2.0;
	  double innerRadius  = fcalData[0].innerGapRadius;
	  double outerRadius  = fcalData[0].outerGapRadius;
	  GeoTubs *tubs       = new GeoTubs(innerRadius,outerRadius,halfDepth, 0.*Gaudi::Units::deg, 360.*Gaudi::Units::deg);
	  GeoLogVol *logVol   = new GeoLogVol(baseName + "Module1::Gap",tubs, LAr);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  
	  int counter=0;
	  modPhysical->add(new GeoSerialIdentifier(0));
	  
	  
	  // Electrodes:
	  int myGroup=1;
	  if ((*m_fcalElectrode).size()>0) {
	    for (unsigned int i=0;i<(*m_fcalElectrode).size();i++) {
	      const IRDBRecord * record = (*m_fcalElectrode)[i];
		
	      int    thisGroup=record->getInt("MODNUMBER");
	      if (thisGroup!=myGroup) continue;
	      double thisTubeX= record->getDouble("X");
	      double thisTubeY= record->getDouble("Y");
	      if (!(thisTubeX<0. && thisTubeY>0.)) continue;
	      
	      std::string thisTileStr=record->getString("TILENAME");
	      int    thisTubeI=record->getInt("I");
	      int    thisTubeJ= record->getInt("J");
	      int    thisTubeID = record->getInt("ID");
	      int    thisTubeMod = record->getInt("MODNUMBER");
	      
	      cmap->add_tube(thisTileStr, thisTubeMod, thisTubeID, thisTubeI,thisTubeJ, thisTubeX, thisTubeY);
		
	      if (m_VisLimit != -1 && (counter++ > m_VisLimit)) continue;
	      //std::cout<<thisTileStr<<" "<<thisTubeX<<" "<<thisTubeY<<std::endl;
	      
	      GeoTransform *xf = new GeoTransform(GeoTrf::Translate3D(thisTubeX*Gaudi::Units::cm, thisTubeY*Gaudi::Units::cm,0));
	      modPhysical->add(xf);
	      modPhysical->add(physVol);
	    }
	  }
	  
	  
	  m_absPhysical1 = modPhysical;
	} // if (F1)
    }
  if (F2) 
    {
      // Module 2
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fcalData[1].fullModuleDepth/2;
	double innerRadius     = fcalData[1].innerModuleRadius;
	double outerRadius     = fcalData[1].outerModuleRadius;
	GeoFullPhysVol *physVol;
	
	if(m_absPhysical2)
	  physVol = m_absPhysical2->clone();
	else
	  {
	    GeoTubs *tubs          = new GeoTubs( innerRadius, outerRadius, halfDepth, fcalstartPhi, fcaldeltaPhi);
	  GeoLogVol  *logVol     = new GeoLogVol(baseName + "Module2::Absorber", tubs, FCal23Absorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	fcalPhysical->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,-(fcalHalfDepth-(startZFCal2-startZFCal1)-halfDepth))));
	fcalPhysical->add(physVol);
	modPhysical = physVol;
	
	std::string tag = std::string("FCAL2");
	StatusCode status;
	
	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
	
      }   
      // 16 Troughs representing  Cable Harnesses:
      if(m_absPhysical2==0)
	{
	  double troughDepth       = 1.0 * Gaudi::Units::cm;
	  double outerRadius       = fcalData[1].outerModuleRadius;
	  double innerRadius       = outerRadius - troughDepth;
	  double halfLength        = fcalData[1].fullModuleDepth/ 2.0;
	  double deltaPhi          = 5.625 * Gaudi::Units::deg;
	  double startPhi          = 11.25 * Gaudi::Units::deg - deltaPhi/2.0;
	  GeoTubs * tubs = new GeoTubs(innerRadius,outerRadius,halfLength,startPhi,deltaPhi );
	  GeoLogVol *logVol = new GeoLogVol(baseName+"Module2::CableTrough",tubs,FCalCableHarness);
	  GeoPhysVol *physVol = new GeoPhysVol(logVol);
	  GeoGenfun::Variable i;
	  GeoGenfun::GENFUNCTION rotationAngle = fcalstartPhi + 22.5*Gaudi::Units::deg*i;
	  GeoXF::TRANSFUNCTION xf = GeoXF::Pow(GeoTrf::RotateZ3D(1.0),rotationAngle);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol,&xf,4);
	  modPhysical->add(st);
	}
      
      // Electrodes:
      if(m_absPhysical2==0) // Until fixed. m_absPhysical2==0)
      {
	
	double halfDepth    = fcalData[1].fullGapDepth/2.0;
	double innerRadius  = fcalData[1].innerGapRadius;
	double outerRadius  = fcalData[1].outerGapRadius;
	
	GeoTubs *gapTubs       = new GeoTubs(0,outerRadius,halfDepth, fcalstartPhi, fcaldeltaPhi);
	GeoLogVol *gapLog      = new GeoLogVol(baseName + "Module2::Gap",gapTubs, LAr);
	GeoPhysVol *gapPhys    = new GeoPhysVol(gapLog);
	
	GeoTubs *rodTubs       = new GeoTubs(0,innerRadius,halfDepth, fcalstartPhi, fcaldeltaPhi);
	GeoLogVol *rodLog      = new GeoLogVol(baseName + "Module2::Rod",rodTubs, FCal23Slugs);
	GeoPhysVol *rodPhys    = new GeoPhysVol(rodLog);
	
	gapPhys->add(rodPhys);
	
	int counter=0;
	modPhysical->add(new GeoSerialIdentifier(0));
	
	int myGroup=2;
	if ((*m_fcalElectrode).size()>0) {
	  for (unsigned int i=0;i<(*m_fcalElectrode).size();i++) {
	    const IRDBRecord * record = (*m_fcalElectrode)[i];
	    
	    int    thisGroup=record->getInt("MODNUMBER");
	    if (thisGroup!=myGroup) continue;
	    
	    double thisTubeX= record->getDouble("X");
	    double thisTubeY= record->getDouble("Y");
	    
	    if (!(thisTubeX<0. && thisTubeY>0.)) continue;
	    
	    std::string thisTileStr=record->getString("TILENAME");
	    int    thisTubeI=record->getInt("I");
	    int    thisTubeJ= record->getInt("J");
	    int    thisTubeID = record->getInt("ID");
	    int    thisTubeMod = record->getInt("MODNUMBER");
	    
	    cmap->add_tube(thisTileStr, thisTubeMod, thisTubeID, thisTubeI,thisTubeJ, thisTubeX, thisTubeY);
	    
	    if (m_VisLimit!=-1 && (counter++ > m_VisLimit)) continue;
	    
	    GeoTransform *xf = new GeoTransform(GeoTrf::Translate3D(thisTubeX*Gaudi::Units::cm, thisTubeY*Gaudi::Units::cm,0));
	    modPhysical->add(xf);
	    modPhysical->add(gapPhys);
	  }
	}
	m_absPhysical2 = modPhysical;
      }
    } // if (F2)
  
  if (F3) 
    {
      // ColdTC......
      GeoFullPhysVol *modPhysical =0;
      {
	double halfDepth       = fcalData[2].fullModuleDepth/2;
	double innerRadius     = fcalData[2].innerModuleRadius;
	double outerRadius     = fcalData[2].outerModuleRadius;
	GeoFullPhysVol *physVol;

	// We need few more materials
	// ColdTC effective absorber: Cu with a little bit of inactive argon
	GeoMaterial *thisAbsorber = new GeoMaterial("ColdTCAbsorber",8.701*GeoModelKernelUnits::g/Gaudi::Units::cm3);
	thisAbsorber->add(Copper,0.994);
	thisAbsorber->add(LAr,0.006);
	thisAbsorber->lock();


	if(m_absPhysical3)
	  physVol = m_absPhysical3->clone();
	else
	{
	  GeoTubs *tubs      = new GeoTubs( innerRadius, outerRadius, halfDepth, 0.999*fcalstartPhi, 1.001*fcaldeltaPhi);
	  GeoLogVol  *logVol = new GeoLogVol(baseName + "ColdTC::Absorber", tubs, thisAbsorber);
	  physVol     = new GeoFullPhysVol(logVol);
	}

	
	fcalPhysical->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,fcalHalfDepth-halfDepth)));
	fcalPhysical->add(physVol);
	modPhysical = physVol;

	std::string tag = std::string("ColdTC");
	StatusCode status;

	StoredPhysVol *sPhysVol = new StoredPhysVol(physVol);
	status=detStore->record(sPhysVol,tag);
	if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());
      }   

      // Add electrode and gaps......
     if(m_absPhysical3==0){
	double halfDepth       = fcalData[2].fullGapDepth/2;
	double innerRadius     = fcalData[2].innerGapRadius;
	double outerRadius     = fcalData[2].outerGapRadius;

	// Where in DB should go this ?
	double ElectrodeDepth = 0.85*Gaudi::Units::cm;
        double ActiveDepth = 0.2*Gaudi::Units::cm;

	// big argon gap solid
	GeoTubs *gapSolid = new GeoTubs(innerRadius,outerRadius,halfDepth, fcalstartPhi, fcaldeltaPhi);
	GeoLogVol *gapLV = new GeoLogVol(baseName + "ColdTC::Gap", gapSolid, LAr);
	// electrode solid
	GeoTubs  *electrodeSolid = new GeoTubs(innerRadius,outerRadius,ElectrodeDepth/2.,1.00001*fcalstartPhi, 0.99999*fcaldeltaPhi);
	GeoLogVol *electrodeLV = new GeoLogVol(baseName + "ColdTC::Electrode", electrodeSolid, G10);
	// active argon solid
	GeoTubs  *activeSolid = new GeoTubs(innerRadius,outerRadius,ActiveDepth/2.,1.0001*fcalstartPhi, 0.9999*fcaldeltaPhi);
	GeoLogVol *activeLV = new GeoLogVol(baseName + "ColdTC::Active", activeSolid, LAr);

	GeoPhysVol *activePhys = new GeoPhysVol(activeLV);
	GeoPhysVol *electrodePhys = new GeoPhysVol(electrodeLV);
	GeoPhysVol *gapPhys = new GeoPhysVol(gapLV);

	// active gaps in electrode
	int iCopy = 1;
	double zPos = -ElectrodeDepth/2. + 1.5 * Gaudi::Units::mm + ActiveDepth/2.;
	GeoTransform *t1 = new GeoTransform(GeoTrf::Translate3D(0.,0.,zPos));
	electrodePhys->add(new GeoSerialIdentifier(iCopy));
	electrodePhys->add(t1);
	electrodePhys->add(activePhys);
	++iCopy;
	zPos += 3.5 * Gaudi::Units::mm;
	electrodePhys->add(new GeoSerialIdentifier(iCopy));
	electrodePhys->add(t1);
	electrodePhys->add(activePhys);
	// electrode in inactive argon
        zPos = -halfDepth + ElectrodeDepth/2.;
	GeoTransform *t2 = new GeoTransform(GeoTrf::Translate3D(0.,0.,zPos));
	gapPhys->add(new GeoSerialIdentifier(1));
	gapPhys->add(t2);
	gapPhys->add(electrodePhys);
	// big gaps in copper block
        zPos = -fcalData[2].fullModuleDepth/2. + 2.2 * Gaudi::Units::cm + halfDepth; 
        for ( iCopy = 1; iCopy < 9; ++iCopy ){
	  modPhysical->add(new GeoSerialIdentifier(iCopy)); 
	  modPhysical->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,zPos)));
	  modPhysical->add(gapPhys);
	  zPos += 3.5*Gaudi::Units::cm;
	}
	m_absPhysical3 = modPhysical;
     }
    } // if (F3)
  
  // Did I already store it?
  //FCAL_ChannelMap *aChannelMap(NULL);
  if (!detStore->contains<FCAL_ChannelMap>("FCAL_ChannelMap")) {
    cmap->finish();
    StatusCode status=detStore->record(cmap,"FCAL_ChannelMap");
    if(!status.isSuccess()) throw std::runtime_error ("Cannot store FCAL_ChannelMap");
  }

  return fcalPhysical;
  
}

