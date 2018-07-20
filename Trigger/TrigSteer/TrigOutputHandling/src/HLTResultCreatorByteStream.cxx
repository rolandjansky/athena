/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include <cstring>
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/StorableConversions.h"
#include "TrigSerializeResult/StringSerializer.h"

#include "HLTResultCreatorByteStream.h"

HLTResultCreatorByteStream::HLTResultCreatorByteStream( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  base_class( type, name, parent ) {
}

HLTResultCreatorByteStream::~HLTResultCreatorByteStream() {}

StatusCode HLTResultCreatorByteStream::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  
  ATH_CHECK( m_hltResultKey.initialize() );
  ATH_CHECK( m_serializerSvc.retrieve() );
  ATH_CHECK( m_dictLoaderSvc.retrieve() );

  for ( std::string typeAndKey: m_collectionsToSerialize ) {
    const std::string type = typeAndKey.substr( 0, typeAndKey.find('#') );
    const std::string key = typeAndKey.substr( typeAndKey.find('#')+1 );    
    CLID clid;
    if ( m_clidSvc->getIDOfTypeName(type, clid).isFailure() )  {
      ATH_MSG_ERROR( "Can not find CLID for " << type << " that is needed to stream " << key );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Type " << type << " known to ClassID srrvice, CLID: " << clid );
      m_toSerialize.push_back( Address{ type, clid, key } );
    }
    if ( not m_dictLoaderSvc->load_type( type ) ) {
      ATH_MSG_WARNING("Can not load dictionary for: " << type );
    }
    
    if ( not m_dictLoaderSvc->has_type( type ) ) {
      ATH_MSG_ERROR("No type dictionary for: " << type );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}


void HLTResultCreatorByteStream::makeHeader(const Address& address, std::vector<uint32_t>& buffer  ) const {
  buffer.push_back(0); // fragment size placeholder
  buffer.push_back( address.clid ); // type info via CLID
  
  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  ss.serialize( address.key, serializedLabel );
  buffer.push_back( serializedLabel.size() );
  buffer.insert( buffer.end(), serializedLabel.begin(), serializedLabel.end() ); // plain SG key
}



void HLTResultCreatorByteStream::fillPayload( void* data, size_t sz, std::vector<uint32_t>& buffer ) const {
  buffer.push_back( sz ); // size in bytes
  const size_t neededSize = sz/sizeof(uint32_t) + (sz%sizeof(uint32_t) ? 1 : 0);
  // ideally we could use the vector<uint32_t> right away
  auto intTempBuffer  = std::make_unique<uint32_t[]>( neededSize );
  intTempBuffer[ neededSize-1 ]  = 0; // empty last bytes
  std::memcpy(data, intTempBuffer.get(), sz);
    
  // copy to buffer
  buffer.insert( buffer.end(), intTempBuffer.get(), intTempBuffer.get()+neededSize  );
}




StatusCode HLTResultCreatorByteStream::createOutput(const EventContext& context ) const {

  auto result = std::make_unique<HLT::HLTResult>();
    
  for ( const Address& address: m_toSerialize ) {
    ATH_MSG_DEBUG( "Streaming " << address.type << "#" << address.key  );
    // obtain object
    DataObject* dObj = evtStore()->accessData( address.clid, address.key );
    if ( dObj == 0 ) {
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

    const RootType rt = m_dictLoaderSvc->load_type(address.type);
    
    size_t sz=0;    
    void* mem = m_serializerSvc->serialize( rawptr, rt, sz );
    ATH_MSG_DEBUG( "Streamed to buffer at address " << mem << " of " << sz << " bytes" );
    
    if ( mem == nullptr and sz == 0 ) {
      ATH_MSG_WARNING( "Serialisation of " << address.type <<"#" << address.key << " unsuccessful" );
      continue;
    }
        
    // prepare fragment
    std::vector<uint32_t> fragment;
    makeHeader( address, fragment );
    fillPayload( mem, sz, fragment );
    fragment[0] = fragment.size();
    
    std::vector<uint32_t>& place = result->getNavigationResult();
    place.insert( place.end(), fragment.begin(), fragment.end() );

    if ( mem ) delete [] static_cast<const char*>( mem );
    ATH_MSG_DEBUG( "Navigation size after inserting " << address.type << "#" << address.key << " " << place.size()*sizeof(uint32_t) << " bytes" );
  }
  
  auto resultHandle = SG::makeHandle( m_hltResultKey, context );
  ATH_CHECK( resultHandle.record( std::move(result) ) );
  
  return StatusCode::SUCCESS;
}

StatusCode HLTResultCreatorByteStream::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


