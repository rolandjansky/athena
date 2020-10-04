/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
  
#include "InDetRegionSelector/SiRegionSelectorTable.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "RegSelLUT/StoreGateIDRS_ClassDEF.h" 
#include "RegSelLUT/RegSelModule.h" 
#include "RegSelLUT/RegSelSiLUT.h" 

#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <fstream>
#include <fstream>

using namespace InDetDD;
using namespace std;

#define USE_STOREGATE


/////////////////////////////////////////////////////////////////////////////

SiRegionSelectorTable::SiRegionSelectorTable(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  :  AthAlgTool(type,name,parent),
     m_regionLUT(nullptr),
     m_managerName(""),
     m_roiFileName("RoITable.txt"),
     m_printHashId(true),
     m_printTable(false),
     m_noDBM(true)
{
  declareInterface<IRegionIDLUT_Creator>(this);
  declareProperty("ManagerName", m_managerName);

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
  //  log << MSG::INFO << "initialize()" << endmsg;
  msg(MSG::INFO) << "initialize() " << name() << " " << PACKAGE_VERSION << endmsg;

  msg(MSG::INFO)  << "Tool Properties" << endmsg;
  msg(MSG::INFO)  << " Detector Manager: " << m_managerName << endmsg;
  if( msgLvl(MSG::DEBUG) ) {
    msg(MSG::DEBUG) << " Output File:      " << m_roiFileName <<endmsg;
    msg(MSG::DEBUG) << " Print hashId:     " << ((m_printHashId) ? "true" : "false") <<endmsg;
    msg(MSG::DEBUG) << " Print Table:      " << ((m_printTable) ? "true" : "false") <<endmsg;
  }    

  if (m_managerName.empty()) {
    msg(MSG::WARNING) << "Tool disabled." << endmsg;
  } 
 
  //  ATH_CHECK(m_condCablingKey.initialize());

  ATH_MSG_WARNING("So far, this prevents the conditions migration!! The createTable() should NOT be used in the initilization step...");

  return StatusCode::SUCCESS;
}


SiRegionSelectorTable::~SiRegionSelectorTable()
{
}


// Get the lookup table.
RegSelSiLUT* SiRegionSelectorTable::getLUT()
{
  return m_regionLUT;
}
	 



StatusCode 
SiRegionSelectorTable::createTable (const EventContext& /* ctx */ )
{
  ATH_MSG_INFO( "SiRegionSelectorTable::createTable() - no longer in use" );
  return StatusCode::SUCCESS;
}



StatusCode SiRegionSelectorTable::finalize() {
  msg(MSG::INFO) << "finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

