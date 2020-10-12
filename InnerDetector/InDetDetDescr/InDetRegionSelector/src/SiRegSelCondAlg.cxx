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


#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"

#include "SiRegSelCondAlg.h"

#include "IRegionSelector/IRegSelLUTCondData.h"
#include "RegSelLUT/RegSelSiLUT.h"


SiRegSelCondAlg::SiRegSelCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName(""),
  m_printTable(false)
{ 
  ATH_MSG_DEBUG( "SiRegSelCondAlg::SiRegSelCondAlg() " << name );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode SiRegSelCondAlg::initialize()
{
  ATH_MSG_DEBUG("SiRegSelCondAlg::initialize() ");
  ATH_CHECK(m_pixCablingKey.initialize());
  ATH_CHECK(m_sctCablingKey.initialize());
  ATH_CHECK(m_detEleCollKey.initialize());
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("SiRegSelCondAlg::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}




StatusCode SiRegSelCondAlg::execute(const EventContext& ctx)  const
{
  ATH_MSG_DEBUG("SiRegSelCondAlg::execute() -- enter -- ");
  
  /// do stuff here ...  
  ATH_MSG_DEBUG( "Creating region selector table " << m_tableKey );

  SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
  // Do we have a valid Write Cond Handle for current time?
  if (lutCondData.isValid()) {
    /// inpractice, this should never be called, although i serial athena, 
    /// because the implementation of the conditions behaviour is flawed in 
    /// the framework, this routine will be called every event (!) regardless 
    /// of whether it should be called or not so we need this check to 
    /// prevent unecessary code execution on out our side 
    ATH_MSG_DEBUG("CondHandle " << lutCondData.fullKey() << " is already valid." );
    return StatusCode::SUCCESS;
  }
 
  /// Need to get the SCT cabling conditions for the EventIDRange, but 
  /// still need to use the SCT_CablingTool to get the rob ids

  std::unique_ptr<SG::ReadCondHandle<PixelCablingCondData> > pixCabling;
  std::unique_ptr<SG::ReadCondHandle<SCT_CablingData> >      sctCabling;

  EventIDRange id_range;

  ATH_MSG_DEBUG( "RegSelCondAlg:" << name() );
  
  if (m_managerName=="SCT") { // SCT
    sctCabling = std::make_unique<SG::ReadCondHandle<SCT_CablingData> >( m_sctCablingKey, ctx );
    if( !sctCabling->range( id_range ) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << sctCabling->key());
      return StatusCode::FAILURE;
    }   
  }
  else { // PIXEL 

    ATH_MSG_DEBUG( "RegSelCondAlg: " << name() << "\t" << m_pixCablingKey );
    ATH_MSG_DEBUG( "RegSelCondAlg: " << ctx );
    ATH_MSG_DEBUG( "RegSelCondAlg: " << id_range << "( initial range )" );

    pixCabling = std::make_unique<SG::ReadCondHandle<PixelCablingCondData> >( m_pixCablingKey, ctx );
    if( !pixCabling->range( id_range ) ) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << pixCabling->key() << " : " << id_range );
      return StatusCode::FAILURE;
    }   
    ATH_MSG_DEBUG( "RegSelCondAlg: " << id_range << "( final range )" );

  }

  ATH_MSG_DEBUG( "RegSelCondAlg:" << name() << " done" );


  std::unique_ptr<RegSelSiLUT> rd;

  if   ( m_managerName=="Pixel" ) rd = std::make_unique<RegSelSiLUT>(RegSelSiLUT::PIXEL);
  else                            rd = std::make_unique<RegSelSiLUT>(RegSelSiLUT::SCT);

  // Get detector elements (=alignment) in condition store
  const InDetDD::SiDetectorElementCollection* elements = nullptr;
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> detEle(m_detEleCollKey, ctx);
  if (detEle.isValid()) {
    elements = detEle.retrieve();
  }
  if (elements==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve InDetDD::SiDetectorElementCollection for " << m_managerName);
    return StatusCode::FAILURE;
  }
  // Since this condition algorithm for HLT, we want to run this only at the first event of a run.
  // We do not take the intersect of ranges of cabling and detector elements.
  // We use the range of cabling.

  for (const InDetDD::SiDetectorElement* element : *elements) {

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
	  // Avoid use of SCT_CablingTool. Instead of
	  // robId=m_sctCablingTool->getRobIdFromOfflineId(element->identify());
	  robId = ((*sctCabling)->getOnlineIdFromHash(element->identifyHash())).rod();
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
  if ( m_printTable ) rd->write( name()+".map" );

  IRegSelLUTCondData* rcd = new IRegSelLUTCondData( std::move(rd) );
  
  try { 
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











