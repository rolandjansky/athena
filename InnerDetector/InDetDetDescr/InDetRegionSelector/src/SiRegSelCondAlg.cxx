/**
 **   @file   SiRegSelCondAlg.cxx         
 **            
 **           conditions algorithm to create the Si detector 
 **           lookup tables    
 **            
 **   @author sutt
 **   @date   Sun 22 Sep 2019 10:21:50 BST
 **
 **
 **   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"


#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "SiRegSelCondAlg.h"




SiRegSelCondAlg::SiRegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName(""),
  m_printTable(false),
  m_sctCablingToolInc("SCT_CablingToolInc")
{ 
  ATH_MSG_DEBUG( "SiRegSelCondAlg::SiRegSelCondAlg() " << this->name() );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode SiRegSelCondAlg::initialize()
{
  ATH_MSG_DEBUG("SiRegSelCondAlg::initialize() ");
  ATH_CHECK(m_condCablingKey.initialize());
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("SiRegSelCondAlg::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}




StatusCode SiRegSelCondAlg::execute(const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("SiRegSelCondAlg::execute() -- enter -- ");
  
  /// do stuff here ...  
  ATH_MSG_DEBUG( "Creating region selector table " << m_tableKey );
 
  StatusCode sc;

  // Retrieve manager
  const  InDetDD::SiDetectorManager* manager;

  sc=detStore()->retrieve( manager, m_managerName );
  
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not find Manager: " << m_managerName << " !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Found Manager " << m_managerName );
  }

  /// Since there are only really ~ 10 lines that would be different between the 
  /// SCT and pixel versions of this code, we chose to do everything in one class
  /// Sadly, the pixel and SCT cabling don't inherit from the same interface 
  /// so we can't get the cabling using a generic call, and have to have separate 
  /// code blocks for each
  /// As such, we need to set the cabling from inside conditional blocks, so have 
  /// to create the cabling as pointers so that we can set inside the blocks and 
  /// subsequently access them outside the blocks 

  /// FIXME: since we store the LUT as conditions data, we need an EventIDRange for  
  ///        the WriteCondHandle, but the SCT cabling is not yet conditions data so
  ///        have to get the pixelCabling for the SCT case also so that the
  ///        EventIDRange can be extracted
  ///        Once the SCT cabling is iself conditions data, we will be able to get
  ///        the corresponding EventIDRange from there

  std::unique_ptr<SG::ReadCondHandle<PixelCablingCondData> > pixCabling;

  EventIDRange id_range;
  
  if (!manager->isPixel()) { // SCT
    if (m_sctCablingToolInc.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Can't get the SCT_CablingToolInc." << endmsg;
      return StatusCode::FAILURE;
    }
  }
  //  else { 
    pixCabling = std::make_unique<SG::ReadCondHandle<PixelCablingCondData> >( m_condCablingKey, ctx );

    if( !pixCabling->range( id_range ) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << pixCabling->key());
      return StatusCode::FAILURE;
    }   
  //  }

  RegSelSiLUT* rd;

  if   ( manager->isPixel() ) rd = new RegSelSiLUT(RegSelSiLUT::PIXEL);
  else                        rd = new RegSelSiLUT(RegSelSiLUT::SCT);

  InDetDD::SiDetectorElementCollection::const_iterator iter = manager->getDetectorElementBegin();

  for ( ; iter != manager->getDetectorElementEnd(); ++iter){

    const InDetDD::SiDetectorElement* element = *iter; 

    if (element) {

      IdentifierHash hashId = element->identifyHash();    
      
      ATH_MSG_VERBOSE( "Found element with HashId = " << hashId );
   
      // new region selector detector element extent

      double rMin, rMax, zMin, zMax, phiMin, phiMax;

      rMin    = element->rMin();
      rMax    = element->rMax();
      zMin    = element->zMin();
      zMax    = element->zMax();
      phiMin  = element->phiMin();
      phiMax  = element->phiMax();


      int      barrelEC  = 0; 
      int      layerDisk = 0;
      uint32_t     robId = 0;

      if (element->isPixel()) {

	const PixelID* pixelId = dynamic_cast<const PixelID*>(element->getIdHelper());
	if ( pixelId!=0 ) { 
	  barrelEC  = pixelId->barrel_ec(element->identify());
	  if ( std::fabs(barrelEC)>3 ) continue; // skip DBM modules
	  layerDisk = pixelId->layer_disk(element->identify());
	  robId=(*pixCabling)->find_entry_offrob(element->identify());
	}
	else { 
	  ATH_MSG_ERROR("Could not get PIXEL_ID for " << element->getIdHelper() );
	}

      } else { // it's an SCT element

	const SCT_ID* sctId = dynamic_cast<const SCT_ID*>(element->getIdHelper());
	if ( sctId!=0 ) {      
	  barrelEC  = sctId->barrel_ec(element->identify());
	  layerDisk = sctId->layer_disk(element->identify());
	  robId=m_sctCablingToolInc->getRobIdFromOfflineId(element->identify());
	}
	else { 
	  ATH_MSG_ERROR("Could not get SCT_ID for " << element->getIdHelper() );
	}

      }


      // create module for lookup table      

      RegSelModule smod( zMin, zMax, rMin, rMax, phiMin, phiMax, layerDisk, barrelEC, robId, hashId );
      
      rd->addModule( smod );
      
      ATH_MSG_DEBUG( smod );
      
      ATH_MSG_VERBOSE( "\t robId = " << robId
		       << " barrelEC = " << barrelEC 
		       << ", layerDisk = " << layerDisk 
		       << ", phiMin, phiMax = " << phiMin/CLHEP::degree << " " << phiMax/CLHEP::degree
		       << ", rMin = " << rMin/CLHEP::mm << " mm, rMax = " << rMax/CLHEP::mm << " mm" );  
      
    }
  }
  
  
  ATH_MSG_DEBUG( " initialising new map " );

  rd->initialise();

  // write out new new LUT to a file if need be
  if ( m_printTable ) {
    if ( manager->isPixel() ) rd->write("RegSelLUT_Pixel");
    else                      rd->write("RegSelLUT_SCT");
  }

  RegSelLUTCondData* rcd = new RegSelLUTCondData( *rd );
  
  try { 
    SG::WriteCondHandle<RegSelLUTCondData> lutCondData( m_tableKey, ctx );
    if( lutCondData.record( id_range, rcd ).isFailure() ) {
      ATH_MSG_ERROR( "Could not record " << m_tableKey 
		     << " " << lutCondData.key()
		     << " with range " << id_range );  
      return StatusCode::FAILURE;   
    } 
    ATH_MSG_INFO( "RegSelCondAlg LUT recorded: " << m_tableKey);
  }
  catch (...) { 
    ATH_MSG_ERROR("SiRegSelCondAlg::execute() failed to record table: " << m_tableKey);
    return StatusCode::FAILURE;   
  }   
  
  
  ATH_MSG_DEBUG("SiRegSelCondAlg::execute() -- exit -- ");

  return StatusCode::SUCCESS;
}











