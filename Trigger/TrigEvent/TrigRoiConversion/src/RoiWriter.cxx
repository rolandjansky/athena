/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiWriter.cxx
//   
//
//   @author M.Sutton
// 

// EDM include(s):
#include "xAODTrigger/RoiDescriptorStore.h"
#include "xAODTrigger/RoiDescriptorStoreAuxInfo.h"

// Local include(s):
#include "RoiWriter.h"
#include "TrigRoiConversion/RoiSerialise.h"

RoiWriter::RoiWriter( const std::string& name, ISvcLocator* pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ) {

   // Declare the property:
   declareProperty( "ForceOverwrite", m_forceOverwrite = false );
}

StatusCode RoiWriter::execute() { 

   ATH_MSG_DEBUG( "In execute()..." );

   // Get the keys of the AOD container(s):
   std::vector< std::string > keys;
   evtStore()->keys< TrigRoiDescriptorCollection >( keys );

   bool just_dandy = true;

   // Loop over these container(s):
   for( const std::string& key : keys ) {

      // Prefix of the AOD container:
      static const std::string prefix = "HLT_TrigRoiDescriptorCollection";
      static const std::string newPrefix = "HLT_xAOD__RoiDescriptorStore";

      // Construct the key of the new container:
      const std::string newKey = ( ( key.find( prefix ) == 0 ) ?
				   ( newPrefix + key.substr( prefix.size() ) ) :
                                   key );
      
      // Check if we need to do anything:
      if( evtStore()->contains< xAOD::RoiDescriptorStore >( newKey ) &&
          !m_forceOverwrite ) {
         continue;
      }

      // Get the roi desciptor collection
      const TrigRoiDescriptorCollection* roiCollection = 0;

      /// check object actually points to something - make sure that we do all 
      /// collections that we can, and not barf on the first one that might fail  
      if ( evtStore()->retrieve( roiCollection, key ).isSuccess() && roiCollection!=0 ) {

	// Create the "new payload" from it:
	std::vector< std::vector< uint32_t > > roiserial;
	RoiUtil::serialise( *roiCollection, roiserial );
	
	// Create the xAOD objects:
	std::unique_ptr< xAOD::RoiDescriptorStore >         store( new xAOD::RoiDescriptorStore() );
	std::unique_ptr< xAOD::RoiDescriptorStoreAuxInfo >    aux( new xAOD::RoiDescriptorStoreAuxInfo() );
	store->setStore( aux.get() );
	store->setSerialised( roiserial );
	
	// Record the new container:
	if( evtStore()->contains< xAOD::RoiDescriptorStore >( newKey ) ) {
	  ATH_CHECK( evtStore()->overwrite( std::move( store ), newKey, false ) );
	  ATH_CHECK( evtStore()->overwrite( std::move( aux ),   newKey + "Aux.", false ) );
	  
	} else {
	  ATH_CHECK( evtStore()->record( std::move( store ), newKey ) );
	  ATH_CHECK( evtStore()->record( std::move( aux ),   newKey + "Aux." ) );
	}
      }
      else just_dandy = false;
   }

   // Return gracefully:
   if ( just_dandy ) return StatusCode::SUCCESS;
   else              return StatusCode::FAILURE;
}

