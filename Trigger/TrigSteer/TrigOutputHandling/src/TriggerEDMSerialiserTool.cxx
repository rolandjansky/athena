/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/




#include <cstring>
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/StorableConversions.h"
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
  for ( std::string typeAndKey: m_collectionsToSerialize ) {
    const std::string type = typeAndKey.substr( 0, typeAndKey.find('#') );
    if ( type.find('_') == std::string::npos ) {
      ATH_MSG_ERROR( "Unversioned object to be recorded " << typeAndKey );
      return StatusCode::FAILURE;
    }
    const std::string transientType = typeAndKey.substr( 0, typeAndKey.find('_') );

    const std::string key = typeAndKey.substr( typeAndKey.find('#')+1 );    
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
    m_toSerialize.push_back( Address{ type, clid, classDesc, key } );      
  }
  return StatusCode::SUCCESS;
}


StatusCode TriggerEDMSerialiserTool::makeHeader(const Address& address, std::vector<uint32_t>& buffer  ) const {
  buffer.push_back(0); // fragment size placeholder
  buffer.push_back( address.clid ); // type info via CLID
  
  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  ss.serialize( address.key, serializedLabel );
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


StatusCode TriggerEDMSerialiserTool::fill( HLT::HLTResultMT& resultToFill ) const {

  ATH_CHECK ( resultToFill.getSerialisedData().find(m_moduleID)  == resultToFill.getSerialisedData().end() ); // do not want overwrite
  
  std::vector<uint32_t> payload;    
  for ( const Address& address: m_toSerialize ) {
    ATH_MSG_DEBUG( "Streaming " << address.type << "#" << address.key  );
    // obtain object
    DataObject* dObj = evtStore()->accessData( address.clid, address.key );
    if ( dObj == nullptr ) {
      ATH_MSG_DEBUG("Data Object with key " << address.key << " is missing");
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
      ATH_MSG_ERROR( "Serialisation of " << address.type <<"#" << address.key << " unsuccessful" );
      return StatusCode::FAILURE;
    }
        
    // prepare fragment
    std::vector<uint32_t> fragment;
    ATH_CHECK( makeHeader( address, fragment ) );
    ATH_CHECK( fillPayload( mem, sz, fragment ) );
    fragment[0] = fragment.size();
    ATH_MSG_DEBUG("Fragment size " << fragment.size() );
    payload.insert( payload.end(), fragment.begin(), fragment.end() );
    
    if ( mem ) delete [] static_cast<const char*>( mem );
    ATH_MSG_DEBUG( "Payload size after inserting " << address.type << "#" << address.key << " " << payload.size()*sizeof(uint32_t) << " bytes" );
  }

  resultToFill.addSerialisedData( m_moduleID, payload );
  
  return StatusCode::SUCCESS;
}

