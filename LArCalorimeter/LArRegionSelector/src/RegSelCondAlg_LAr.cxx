/**
 **   @file   RegSelCondAlg_LAr.cxx         
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


#include "RegSelCondAlg_LAr.h"

#include "GaudiKernel/EventIDRange.h"
#include "StoreGate/WriteCondHandle.h"

#include "RegionSelector/RegionSelectorLUT.h"

#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrRegion.h"
#include "LArRawUtils/LArRoI_Map.h"

#include "eformat/SourceIdentifier.h"
#include "LArIdentifier/LArReadoutModuleService.h"

#include <iostream>
#include <fstream>
#include <string>


RegSelCondAlg_LAr::RegSelCondAlg_LAr(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm( name, pSvcLocator ),
  m_managerName(""),
  m_printTable(false)
{ 
  ATH_MSG_INFO( "RegSelCondAlg_LAr::RegSelCondAlg_LAr() " << name );
  declareProperty( "PrintTable",  m_printTable=false );  
  declareProperty( "ManagerName", m_managerName );  
}




StatusCode RegSelCondAlg_LAr::initialize() {
  ATH_MSG_DEBUG("RegSelCondAlg_LAr::initialize() ");
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_tableKey.initialize());
  ATH_MSG_INFO("RegSelCondAlg_LAr::initialize() " << m_tableKey );
  return StatusCode::SUCCESS;
}



StatusCode RegSelCondAlg_LAr::execute(const EventContext& ctx)  const {

  ATH_MSG_DEBUG("RegSelCondAlg_LAr::execute() -- enter -- ");

   /// do stuff here ...  
  ATH_MSG_DEBUG( "Creating region selector table " << m_tableKey );

  SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
  if (lutCondData.isValid()) {
    /// inpractice, this should never be called, although in serial athena,                                                                          
    /// because the implementation of the conditions behaviour is flawed in                                                                          
    /// the framework, this routine will be called every event (!) regardless                                                                        
    /// of whether it should be called or not so we need this check to                                                                               
    /// prevent unecessary code execution on out our side                                                                                            
    ATH_MSG_DEBUG("CondHandle " << lutCondData.fullKey() << " is already valid." );
    return StatusCode::SUCCESS;
  }

   
  /// annoyingly take the pixel cabling to determine whether to build this
  /// calorimeter table using the EventIDRange.
  /// Once the calorimeter has it own conditions data cabling, then we can
  /// dispense with this and use the calorimeter conditions data directly

  SG::ReadCondHandle<PixelCablingCondData> cabling( m_cablingKey, ctx );

  EventIDRange id_range;
  
  if( !cabling.range( id_range ) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << cabling.key());
    return StatusCode::FAILURE;
  }   

  
  ATH_MSG_INFO( "creating new LAr table" );

  /// create the new lookup table

  std::unique_ptr<IRegSelLUT> rd = createTable();

  if ( !rd ) return StatusCode::FAILURE;

  ATH_MSG_INFO( "Storing the new look up table " << m_tableKey );

  /// create the conditions data for storage 

  IRegSelLUTCondData* rcd = new IRegSelLUTCondData( std::move(rd) );  

  try {
    /// leave this commented here since this is where it should really be,
    /// but we had to move it up in the code to handle the flawed conditions 
    /// handling in the serial athena use case
    ///    SG::WriteCondHandle<IRegSelLUTCondData> lutCondData( m_tableKey, ctx );
    if( lutCondData.record( id_range, rcd ).isFailure() ) {
      ATH_MSG_ERROR( "Could not record " << m_tableKey 
		     << " " << lutCondData.key()
		     << " with range " << id_range );  
      return StatusCode::FAILURE;   
    } 
    ATH_MSG_INFO( "RegSelCondAlg LUT recorded: " << m_tableKey);
  }
  catch (...) { 
    ATH_MSG_ERROR("RegSelCondAlg_LAr::execute() failed to record table: " << m_tableKey);
    return StatusCode::FAILURE;   
  }   
  
  
  ATH_MSG_DEBUG("RegSelCondAlg_LAr::execute() -- exit -- ");

  return StatusCode::SUCCESS;
}




std::unique_ptr<RegSelectorHashMap> RegSelCondAlg_LAr::createTable() const {

  std::unique_ptr<RegSelectorHashMap> lut(nullptr);

  /// get all the tools, managers and helpers locally - done't need to create 
  /// class variables for any of these 

  IToolSvc* toolSvc = nullptr;
  if ( service( "ToolSvc", toolSvc).isFailure() ) { 
    ATH_MSG_ERROR( " Successfully retrieved ToolSvc ");
    return lut;
  }

  const CaloTTDescrManager* manager; 
  const CaloLVL1_ID*        helper; 
  const LArRoI_Map*         roi_map;

  if ( detStore()->retrieve(manager).isFailure() ) {
    ATH_MSG_ERROR( "CaloTTMgr Manager not found" );
    return lut;
  }

  if ( detStore()->retrieve(helper).isFailure() ) {
    ATH_MSG_ERROR( "CaloTTMgr id helper not found" );
    return lut;
  }

  if ( toolSvc->retrieveTool( "LArRoI_Map", roi_map ).isFailure() ) {
    ATH_MSG_ERROR( "roi map not found" );
    return lut;
  }
  
  int   lut_size = 0;

  int   sampling  = 0; 
  int  fcal_layer = 1;
  bool      fcal  = false; 

  /// could probably do with a switch statement, but this is 
  /// good enough
  /// NB: In the original LArRegionSelectorTable code, all the tables were created
  ///     at the same time - we could do this here, but for consistency with the 
  ///     approach for the Tools, we generate them all separately since the individual
  ///     tools can then have their own CondAlg

  if      ( m_managerName == "TTEM" ) { 
    lut_size = 12544;
    sampling = 0;
    fcal     = false;
  }
  else if ( m_managerName == "TTHEC" ) {
    lut_size =  5632;
    sampling =  1;
    fcal     = false;
  }
  else if ( m_managerName == "FCALEM" ) { 
    lut_size   = 128;
    sampling   = 0;
    fcal_layer = 0;
    fcal       = true;
  }
  else if ( m_managerName == "FCALHAD" ) { 
    lut_size = 256;
    sampling = 1;
    fcal     = true;
  }
  else { 
    ATH_MSG_ERROR( "Incorrect table type" );
    return lut;
  }


  /// generic counters 

  int ncount  = 0; 
  int ncount1 = 0; 

  /// doesn't this need any configuration ??? 
  LArReadoutModuleService larReadoutModuleSvc;

  std::unique_ptr<RegionSelectorLUT> lut_stuff = std::make_unique<RegionSelectorLUT>( lut_size );

  CaloTTDescrManager::calo_region_const_iterator it2     = manager->calo_region_begin();
  CaloTTDescrManager::calo_region_const_iterator it2_end = manager->calo_region_end();
  
  for ( ; it2!=it2_end ; ++it2 ) {
    
    const CaloTTDescrRegion* region = (*it2); 
    
    Identifier layer_id = region->identify(); 
    int            samp = helper->sampling(layer_id);
    int           layer = helper->layer(layer_id);
    bool         isFcal = helper->is_fcal(layer_id); 

    if ( isFcal != fcal ) continue;
    if ( sampling==0 && samp!=0 ) continue;
    if ( sampling!=0 && samp==0 ) continue;
    if ( fcal_layer==0 && layer!=0 ) { 
      ATH_MSG_DEBUG( "Whoa! how did this happen ? should never be able to get here: " << samp << " " << layer << " " << isFcal );
      continue;
    }


    float dEta = region->deta() ; 
    float dPhi = region->dphi() ; 
    
    float eta1 = region->eta()-dEta/2. ; 
    float eta2 = eta1+dEta; 
    
    float phi1 = region->phi()-dPhi/2. ; 
    float phi2 = phi1+dPhi; 

    IdentifierHash hashid;

    IdContext IdCntx = helper->layer_context(); 

    helper->get_hash(layer_id, hashid, &IdCntx ) ; 

    // quite special treatment for FCALHAD
    if ( isFcal && ( samp != 0 ) ) {
      if ( helper->pos_neg_z(layer_id)==1 ) { 

	/// FIXME: a lot of these numbers appear to be hard coded
	///        it would be better to calculate these psuedorapidity 
	///        values from the geometry rather than all this messing 
	///        about with flags and layer ids    

	eta1 = 3.6; 
	eta2 = 5.2;
      }
      else { 
	eta1 = -5.2;
	eta2 = -3.6;
      };

      int pseudolayer = helper->eta(layer_id);

      if ( layer == 0 ) {

	layer = ( pseudolayer<2 ? 0 : 1 );
	
	if ( layer == 0 ){
	  if ( helper->pos_neg_z(layer_id)==1 ) { 
	    eta1 = 3.2; 
	    eta2 = 3.6;
	  }
	  else { 
	    eta1 = -3.6;
	    eta2 = -3.2;
	  };
	}
      } else {

	layer = ( pseudolayer<2 ? 2 : 3 );

	if ( layer == 2 ) {
	  if ( helper->pos_neg_z(layer_id)==1 ) { 
	    eta1 = 3.2; 
	    eta2 = 3.6;
	  }
	  else { 
	    eta1 = -3.6;
	    eta2 = -3.2;
	  };
	}
      }
    } // end of isFcal hadronic

    // get the RODID for this TT
    const std::vector<HWIdentifier>& vROBID = roi_map->CollectionID((unsigned int)hashid);
 
    // skip the TT that does not have readout, i.e., invalid.
    if( vROBID.size() > 0 ) { 

      if( vROBID.size() > 1 ) {
        ATH_MSG_DEBUG ( "More than 1  (" << vROBID.size()
                        << ") ROBID for this TT, sampling, layer, etamin, phimin= " 
                        << helper->show_to_string(layer_id) << " " 
                        << samp << " " << layer << " " << eta1 << " " << phi1
                        );
      }
      //     *ttmap << helper->show_to_string(layer_id) << " " << hashid 
  
      std::vector<HWIdentifier>::const_iterator it     = vROBID.begin(); 
      std::vector<HWIdentifier>::const_iterator it_end = vROBID.end(); 

      for ( ; it!=it_end ; ++it ) {
	  
	const HWIdentifier mId = (*it);
	
	eformat::SubDetector detid = (eformat::SubDetector) larReadoutModuleSvc.subDet(mId);
	
	uint16_t m = larReadoutModuleSvc.rodFragId(mId);
	
	//EventFormat::SourceID sid = EventFormat::SourceID(detid,type,m);
	eformat::helper::SourceIdentifier sid(detid,m);
	
	uint32_t rod_id = sid.code();
	
	lut_stuff->additem( (double)eta1, (double)eta2, (double)phi1, (double)phi2, samp, layer, (int)hashid, (int)rod_id );

	ATH_MSG_DEBUG( ncount << " filled LUT with following info"
		       << " hashid,sampling, layer, etamin, phimin= " 
		       << helper->show_to_string(layer_id) << " " 
		       << hashid << " "
		       << samp << " " << layer << " " << eta1 << " " << phi1 );
	
        ++ncount; 
      }
      
      ++ncount1; 

    } else {

      // no ROBID for this TT. not abnormal (not all layers exist in all TTs)
      ATH_MSG_DEBUG ( "No ROBID for this TT, sign, sampling, layer, etamin, phimin= " 
                      << helper->show_to_string(layer_id) 
		      << " " << samp << " " << layer << " " << eta1 << " " << phi1 );

    }
    
  } // end loop on "regions"


  /// The RegionSelectorLUT and RegSelectorHashMap classes are truly 
  /// awful and should be completely replaced, but for the time being 
  /// need to create the RegSelectorHashMap from the RegionSelectorLUT 
  /// (why not just fill the HashMap to start with ???)
  /// and then call mountDataStruct() - why this insane approach ? 

  lut = std::make_unique<RegSelectorHashMap>();
  lut->addLut( lut_stuff.get());
  lut->mountDataStruct(); 

  ATH_MSG_DEBUG( " Total number of records = " << ncount );
  ATH_MSG_DEBUG( " Total number of TTs = " << ncount1 );

  return lut;
}

