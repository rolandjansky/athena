/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/




#include <cstring>
#include <boost/algorithm/string.hpp>
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/StorableConversions.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/debug.h"
#include "xAODCore/AuxContainerBase.h"

#include "TrigSerializeResult/StringSerializer.h"

#include "TriggerEDMSerialiserTool.h"

TriggerEDMSerialiserTool::TriggerEDMSerialiserTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  base_class( type, name, parent ) {
}

TriggerEDMSerialiserTool::~TriggerEDMSerialiserTool() {}

StatusCode TriggerEDMSerialiserTool::initialize() {
  
  ATH_CHECK( m_serializerSvc.retrieve() );
  ATH_CHECK( m_clidSvc.retrieve() );
  for ( std::string typeKeyAux: m_collectionsToSerialize ) {
    const std::string type = typeKeyAux.substr( 0, typeKeyAux.find('#') );
    if ( type.find('_') == std::string::npos ) {
      ATH_MSG_ERROR( "Unversioned object to be recorded " << typeKeyAux );
      return StatusCode::FAILURE;
    }
    const std::string transientType = typeKeyAux.substr( 0, typeKeyAux.find('_') );

    const std::string key = typeKeyAux.substr( typeKeyAux.find('#')+1, typeKeyAux.find('.')-typeKeyAux.find('#') );    
    CLID clid;
    if ( m_clidSvc->getIDOfTypeName(transientType, clid).isFailure() )  {
      ATH_MSG_ERROR( "Can not find CLID for " << transientType << " that is needed to stream " << key );
      return StatusCode::FAILURE;
    } 

    RootType classDesc = RootType::ByName( type );
    if ( ! classDesc.IsComplete() ) {
      ATH_MSG_ERROR( "The type " << type <<  " is not known" );
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG( "Type " << type << " key " << key <<  " serializable" );

    xAOD::AuxSelection sel;
    if ( typeKeyAux.find('.') != std::string::npos ) {
      ATH_MSG_DEBUG( "with aux content: "  );
      std::string allVars = typeKeyAux.substr( typeKeyAux.find('.')+1 );
      std::set<std::string> variableNames;
      boost::split( variableNames, allVars, [](const char c){ return c == '.'; } );
      for ( auto el: variableNames ) 
	ATH_MSG_DEBUG( " " << el  );
      sel.selectAux( variableNames );
    }

    const bool isAux = key.find("Aux") != std::string::npos;

    m_toSerialize.push_back( Address{ type+"#"+key, type, clid, key, isAux, sel } );      
  }
  return StatusCode::SUCCESS;
}


StatusCode TriggerEDMSerialiserTool::makeHeader(const Address& address, std::vector<uint32_t>& buffer  ) const {
  buffer.push_back(0); // fragment size placeholder
  buffer.push_back( address.clid ); // type info via CLID
  
  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  ss.serialize( address.typeKey, serializedLabel );
  buffer.push_back( serializedLabel.size() );
  buffer.insert( buffer.end(), serializedLabel.begin(), serializedLabel.end() ); // plain SG key
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::fillPayload( const void* data, size_t sz, std::vector<uint32_t>& buffer ) const {
  ATH_CHECK( sz != 0 );
  ATH_CHECK( data != nullptr );
    
  buffer.push_back( sz ); // size in bytes
  const size_t neededSize = std::ceil( double(sz)/sizeof(uint32_t) );
  // ideally we could use the vector<uint32_t> right away
  auto intTempBuffer  = std::make_unique<uint32_t[]>( neededSize );
  intTempBuffer[ neededSize-1 ]  = 0; // empty last bytes
  std::memcpy( intTempBuffer.get(), data, sz);
    
  // copy to buffer
  buffer.insert( buffer.end(), intTempBuffer.get(), intTempBuffer.get()+neededSize  );
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::fillDynAux( const Address& address, DataObject* dObj, std::vector<uint32_t>& buffer ) const {
  // TODO, check if we can cache this informion after it is filled once
  ATH_MSG_DEBUG("About to start streaming aux data of " << address.key );
  DataBucketBase* dObjAux = dynamic_cast<DataBucketBase*>(dObj);
  ATH_CHECK( dObjAux != nullptr );  

  const SG::IAuxStoreIO* auxStoreIO = dObjAux->template cast<SG::IAuxStoreIO> (nullptr, true);
  if ( auxStoreIO == nullptr ) {
    ATH_MSG_DEBUG( "Can't obtain AuxContainerBase of " << address.key <<  " no dynamic variables presumably" );
    return StatusCode::SUCCESS;
  }
  //  ATH_MSG_DEBUG( "dump aux store" );
  //  SGdebug::dump_aux_vars( *auxStore );
  
  const SG::auxid_set_t& selected = address.sel.getSelectedAuxIDs( auxStoreIO->getDynamicAuxIDs() );
  
  if ( selected.empty() ) {
    ATH_MSG_VERBOSE( "Empty set of dynamic variables to store, do nothing" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Ready for serialisation of " << selected.size() << " dynamic variables");
  
  for (SG::auxid_t auxVarID : selected ) {
    
    const std::string typeName = SG::AuxTypeRegistry::instance().getVecTypeName(auxVarID);
    const std::string name = SG::AuxTypeRegistry::instance().getName(auxVarID);
    ATH_MSG_DEBUG("Streaming " << name << " of type " << typeName );


    CLID clid;
    if ( m_clidSvc->getIDOfTypeName(typeName, clid).isFailure() )  {
      ATH_MSG_ERROR( "Can not obtain CLID of: " << typeName );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "CLID " << clid );

    RootType classDesc = RootType::ByName( typeName );  

    const void* rawptr = auxStoreIO->getIOData( auxVarID );
    ATH_CHECK( rawptr != nullptr );

    size_t sz=0;    
    void* mem = m_serializerSvc->serialize( rawptr, classDesc, sz );
    
    if ( mem == nullptr or sz == 0 ) {
      ATH_MSG_ERROR( "Serialisation of " << address.type <<"#" << address.key << "."<< name << " unsuccessful" );
      return StatusCode::FAILURE;
    }

    std::vector<uint32_t> fragment;
    Address auxAddress = { "", typeName, clid, address.key+"."+name, false };
    ATH_CHECK( makeHeader( auxAddress, fragment ) );
    ATH_CHECK( fillPayload( mem, sz, fragment ) );
    fragment[0] = fragment.size();

    if ( mem ) delete [] static_cast<const char*>( mem );
    
    ATH_MSG_DEBUG("Fragment size " << fragment.size() );
    
    buffer.insert( buffer.end(), fragment.begin(), fragment.end() );        
    
  }
  
  
  return StatusCode::SUCCESS;
}


StatusCode TriggerEDMSerialiserTool::fill( HLT::HLTResultMT& resultToFill ) const {
  
  std::vector<uint32_t> payload;    
  for ( const Address& address: m_toSerialize ) {
    ATH_MSG_DEBUG( "Streaming " << address.typeKey  );
    // obtain object
    DataObject* dObj = evtStore()->accessData( address.clid, address.key );
    if ( dObj == nullptr ) {
      ATH_MSG_DEBUG("Data Object with the CLID " << address.clid <<" and the key " << address.key << " is missing");
      continue;
    }


    const void* rawptr = SG::fromStorable( dObj, address.clid, nullptr, msgLvl(MSG::DEBUG) );
    if ( rawptr == nullptr ) {
      ATH_MSG_DEBUG( "Data Object with key " << address.key <<
		     " can not be converted to void* for streaming" );
      continue;      
    }
    ATH_MSG_DEBUG("Obtained raw pointer " << rawptr );


    RootType classDesc = RootType::ByName( address.type );    
    size_t sz=0;    
    void* mem = m_serializerSvc->serialize( rawptr, classDesc, sz );

    ATH_MSG_DEBUG( "Streamed to buffer at address " << mem << " of " << sz << " bytes" );
    
    if ( mem == nullptr or sz == 0 ) {
      ATH_MSG_ERROR( "Serialisation of " << address.typeKey << " unsuccessful" );
      return StatusCode::FAILURE;
    }
        
    // prepare fragment
    std::vector<uint32_t> fragment;
    ATH_CHECK( makeHeader( address, fragment ) );
    ATH_CHECK( fillPayload( mem, sz, fragment ) );

    
    if ( mem ) delete [] static_cast<const char*>( mem );
    
    ATH_MSG_DEBUG("Fragment size " << fragment.size() );

    if ( address.isAux ) {
      ATH_CHECK( fillDynAux( address, dObj, fragment ) );
      ATH_MSG_DEBUG("Fragment size with Aux data " << fragment.size() );
    }
    fragment[0] = fragment.size();
    
    payload.insert( payload.end(), fragment.begin(), fragment.end() );
    ATH_MSG_DEBUG( "Payload size after inserting " << address.typeKey << " " << payload.size()*sizeof(uint32_t) << " bytes" );
    
  }
  
  ATH_CHECK( resultToFill.addSerialisedDataWithCheck( m_moduleID, payload ) );
  
  return StatusCode::SUCCESS;
}

