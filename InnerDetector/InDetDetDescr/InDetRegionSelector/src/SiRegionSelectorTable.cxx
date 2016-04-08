/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRegionSelector/SiRegionSelectorTable.h"

// #include "GaudiKernel/MsgStream.h"
// #include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include <iostream>
#include <fstream>
#include <fstream>

using namespace std;

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 

#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 

using namespace InDetDD;

#define USE_STOREGATE


/////////////////////////////////////////////////////////////////////////////

SiRegionSelectorTable::SiRegionSelectorTable(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  :  AthAlgTool(type,name,parent),
     //     m_detStore(NULL),
     m_regionLUT(NULL),
     m_managerName(""),
     m_deltaZ(168 * CLHEP::mm),
     m_roiFileName("RoITable.txt"),
     m_printHashId(true),
     m_printTable(false),
     m_noDBM(true),
     m_pixIdMapping("PixelCablingSvc", name),
     m_sctCablingSvc("SCT_CablingSvc",name)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  declareProperty("ManagerName", m_managerName);
  declareProperty("DeltaZ",      m_deltaZ);

  // The remaining properties are for debugging purposes.
  declareProperty("OutputFile",  m_roiFileName);
  declareProperty("PrintHashId", m_printHashId);
  declareProperty("PrintTable",  m_printTable);
  declareProperty("NoDBM",       m_noDBM=true);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
SiRegionSelectorTable::initialize(){

  //  MsgStream log(msgSvc(), name());
  //  log << MSG::INFO << "initialize()" << endreq;
  msg(MSG::INFO) << "initialize() " << name() << " " << PACKAGE_VERSION << endreq;

  msg(MSG::INFO)  << "Tool Properties" << endreq;
  msg(MSG::INFO)  << " Detector Manager: " << m_managerName << endreq;
  msg(MSG::INFO)  << " DeltaZ:           " << m_deltaZ/CLHEP::mm << " mm <<< NB: this parameter is now OBSOLETE" << endreq;
  if( msgLvl(MSG::DEBUG) ) {
    msg(MSG::DEBUG) << " Output File:      " << m_roiFileName <<endreq;
    msg(MSG::DEBUG) << " Print hashId:     " << ((m_printHashId) ? "true" : "false") <<endreq;
    msg(MSG::DEBUG) << " Print Table:      " << ((m_printTable) ? "true" : "false") <<endreq;
  }    

  if (m_managerName.empty()) {
    msg(MSG::WARNING) << "Tool disabled." << endreq;
    return StatusCode::FAILURE;
  } 
 
  StatusCode  sc;

  sc = createTable();
  return sc;
}


SiRegionSelectorTable::~SiRegionSelectorTable()
{
  // table is stored in storegate so nothing to delete.
#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
}


// Get the lookup table.
RegSelSiLUT* SiRegionSelectorTable::getLUT() const
{
  return m_regionLUT;
}
	 



StatusCode 
SiRegionSelectorTable::createTable()
{

  if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Creating region selector table"  << endreq;

  StatusCode sc;

  // Retrieve manager
  const SiDetectorManager * manager;
  sc=detStore()->retrieve(manager, m_managerName);

  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find the Manager: "
	<< m_managerName << " !" << endreq;
    return StatusCode::FAILURE;
  } else {
    if ( msgLvl(MSG::DEBUG) )  msg(MSG::DEBUG) << "Manager found" << endreq;
  }

  if (manager->isPixel()) {
    if (m_pixIdMapping.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Can't get the Pixel Mapping tool." << endreq;
      return StatusCode::FAILURE;
    }
  } else { // SCT
    if (m_sctCablingSvc.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Can't get the SCT cabling service." << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Create RegionSelectorLUT pointers for Pixel or Sct
  //  RegionSelectorLUT*  rslut = new RegionSelectorLUT;
  
  RegSelSiLUT* rd;

  if   ( manager->isPixel() ) rd = new RegSelSiLUT(RegSelSiLUT::PIXEL);
  else                        rd = new RegSelSiLUT(RegSelSiLUT::SCT);


  SiDetectorElementCollection::const_iterator iter;
  for (iter = manager->getDetectorElementBegin(); iter != manager->getDetectorElementEnd(); ++iter){

    const SiDetectorElement* element = *iter; 

    if (element) {

      IdentifierHash hashId = element->identifyHash();    
      
      if ( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "Found element with HashId = " << hashId << endreq;
   
      ////   double etaMin,etaMax,phiMin,phiMax,rz;
      ////   element->getEtaPhiRegion(m_deltaZ,etaMin,etaMax,phiMin,phiMax,rz);

      // new region selector detector element extent.
      double rMin, rMax, zMin, zMax, phiMin, phiMax;

      rMin    = element->rMin();
      rMax    = element->rMax();
      zMin    = element->zMin();
      zMax    = element->zMax();
      phiMin  = element->phiMin();
      phiMax  = element->phiMax();


      int barrelEC = 0; 
      int layerDisk = 0;
      uint32_t robId = 0;

      if (element->isPixel()) {

	const PixelID* pixelId = dynamic_cast<const PixelID*>(element->getIdHelper());
	if ( pixelId!=0 ) { 
	  barrelEC  = pixelId->barrel_ec(element->identify());

	  if ( m_noDBM && std::fabs(barrelEC)>3 ) continue; // skip DBM modules

	  layerDisk = pixelId->layer_disk(element->identify());
	  robId=m_pixIdMapping->getRobID(element->identify());
	}
	else { 
	  msg(MSG::ERROR) << " could not get PixelID for " << element->getIdHelper() << endreq;
	}
      } else { // Its an SCT.

	const SCT_ID* sctId = dynamic_cast<const SCT_ID*>(element->getIdHelper());
	if ( sctId!=0 ) {      
	  barrelEC  = sctId->barrel_ec(element->identify());
	  layerDisk = sctId->layer_disk(element->identify());
	  robId=m_sctCablingSvc->getRobIdFromOfflineId(element->identify());       
	}
	else { 
	  msg(MSG::ERROR) << " could not get SCT_ID for " << element->getIdHelper() << endreq;
	}
      }

      // write in new Region Selector Si LUT
      // create module      
      RegSelModule smod(zMin,zMax,rMin,rMax,phiMin,phiMax,layerDisk,barrelEC,robId,hashId);
	
      if ( robId ) {
	// add to the new RegionSelector map     
	rd->addModule(smod);
      }
      else { 
	msg(MSG::WARNING) << "module with RobID=0x0 - not added to look up table " << smod << endreq;
      }
      
      if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << smod << endreq;
	
      if ( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << "      " 
						    << " deltaZ = " << m_deltaZ/CLHEP::mm << " mm, " 
						    << " robId = " << robId
						    << " barrelEC = " << barrelEC 
						    << ", layerDisk = " << layerDisk 
						    << ", phiMin, phiMax = " << phiMin/CLHEP::degree << " " << phiMax/CLHEP::degree
						    << ", rMin = " << rMin/CLHEP::mm << " mm, rMax = " << rMax/CLHEP::mm << " mm"  
						    << endreq;


      //      if   ( manager->isPixel() ) std::cout << "SUTT-DBM " << smod << std::endl;

    }
  }

  msg(MSG::INFO) << " initialising new map " << endreq;

  rd->initialise();

  // write out new new LUT to a file if need be
  if ( m_printTable ) {
    if ( manager->isPixel() ) rd->write("NewPixel"+m_roiFileName);
    else                      rd->write("NewSCT"+m_roiFileName);
  }

  //  std::string key;
  std::string detName;

  std::string newkey;

  if (manager->isPixel()) {
    newkey = "PixelRegSelSiLUT";
    detName = "Pixel";
  } else {
    newkey = "SCTRegSelSiLUT";
    detName = "SCT";
  }

#ifndef USE_STOREGATE
  if ( m_regionLUT ) delete m_regionLUT;
#endif
  m_regionLUT = rd;

#ifdef USE_STOREGATE
  // save new map in StoreGate RegSelSiLUT
  sc = detStore()->contains< RegSelSiLUT >(newkey);
  if (sc == StatusCode::SUCCESS ) {
    msg(MSG::FATAL) << " RegSelSiLUT " << newkey << " already exists " << endreq;
  } else {
    // create and store LUT
    // needs to be modifiable so we can enable/disable modules 
    // from the RegSelSvc
    sc = detStore()->record(rd, newkey, true);
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << " could not register " << detName << " RegSelSiLUT" << endreq;
      return( StatusCode::FAILURE );
    } else {
      msg(MSG::INFO) << detName << " RegSelSiLUT successfully saved in detector Store" << endreq;
    }
  }
#endif
 
  return StatusCode::SUCCESS;
}



StatusCode SiRegionSelectorTable::finalize() {
  msg(MSG::INFO) << "finalize()" << endreq;
  return StatusCode::SUCCESS;
}

