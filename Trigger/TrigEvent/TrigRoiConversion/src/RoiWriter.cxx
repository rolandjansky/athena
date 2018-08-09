/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiWriter.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: RoiWriter.cxx, v0.0   Wed 18 Nov 2015 12:22:35 CET sutt $


/// class header
#include "TrigRoiConversion/RoiWriter.h"


/// xAOD storage class
#include "xAODTrigger/RoiDescriptorStore.h"
#include "xAODTrigger/RoiDescriptorStoreAuxInfo.h"

/// utility to serialise the RoiDescriptorCollection
#include "TrigRoiConversion/RoiSerialise.h"


RoiWriter::RoiWriter( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{ } 



StatusCode RoiWriter::initialize() { 
  return StatusCode::SUCCESS;
}


StatusCode RoiWriter::execute() { 

  ATH_MSG_DEBUG( "RoiWriter::execute() in" );

  bool just_dandy = true;
  std::vector<std::string> keys;
  evtStore()->keys<TrigRoiDescriptorCollection>( keys );
  
  for ( unsigned ik=0 ; ik<keys.size() ; ik++ ) { 

    std::string collectionname = "TrigRoiDescriptorCollection";
    
    std::string keybase = keys[ik];
    keybase.erase( 0, keybase.find(collectionname)+collectionname.size() );
    
    std::string newkey      = "HLT_xAOD__RoiDescriptorStore" + keybase;
    std::string newstorekey = "HLT_xAOD__RoiDescriptorStore" + keybase + "Aux.";

    /// don't continue, overwrite instead
    //    if ( evtStore()->contains<xAOD::RoiDescriptorStore>( newkey ) ) continue; 

    const TrigRoiDescriptorCollection* roicollection = 0;

    if ( evtStore()->retrieve( roicollection, keys[ik] ).isSuccess() && roicollection!=0 ) {
      
      ATH_MSG_DEBUG( "\tcollection" << keys[ik] << " " << roicollection->size() );

      std::vector< std::vector<uint32_t> > roiserial;
      RoiUtil::serialise( *roicollection, roiserial );

      /// The Store always needs it's own AuxInfo, so why the AuxInfo and setStore can't 
      /// be done by default in the Store constructor is anyone's guess - presumably 
      /// when reading back from persistant storage the constructors could be created
      /// with some default parameter - I have code to *not* create the AuxInfo by
      /// default, but to do if given a flag. Is this frowned upon?
      /// this requires the xAOD container to have a method store() that returns the
      /// internal auxilliary store
      /// ideally, one would want an xAOD::RoiDescriptorStore constructor that takes 
      /// a vector<vector<uint32_t>> then it could all be done in one line 

      xAOD::RoiDescriptorStore*        roistore    = new xAOD::RoiDescriptorStore;
      xAOD::RoiDescriptorStoreAuxInfo* roistoreaux = new xAOD::RoiDescriptorStoreAuxInfo;      
      roistore->setStore( roistoreaux );	      
      
      roistore->setSerialised( roiserial );

      //      CHECK( evtStore()->record( roistore,    newkey ) );  
      //      CHECK( evtStore()->record( roistoreaux, newstorekey ) );  

      CHECK( evtStore()->overwrite( roistore,    newkey ) );  
      CHECK( evtStore()->overwrite( roistoreaux, newstorekey ) );  

      //   TrigRoiDescriptorCollection coll;	
      //   RoiWriter::deserialiser( coll, newkey ); 
      //   std::cout << "\tcollection size " << newkey << " " << coll.size() << std::endl;
      //   for ( unsigned j=0 ; j<coll.size() ; j++ ) std::cout << "\t\t" << j << " " << *coll[j] << std::endl;
           
    }
    else just_dandy = false;
  }

  ATH_MSG_DEBUG( "RoiWriter::execute() out " << just_dandy);

  //  ATH_MSG_DEBUG( "Dump storegate " << evtStore()->dump() );

  if ( just_dandy ) return StatusCode::SUCCESS;
  else              return StatusCode::FAILURE;

}


StatusCode RoiWriter::finalize() { 
  return StatusCode::SUCCESS;
}




void RoiWriter::deserialiser( TrigRoiDescriptorCollection& collection, const std::string key ) const {
  xAOD::RoiDescriptorStore* fetchedstore = 0;
  if ( evtStore()->retrieve( fetchedstore, key ).isSuccess() && fetchedstore!=0 ) { 
    RoiUtil::deserialise( fetchedstore->serialised(), collection );
  }
}

