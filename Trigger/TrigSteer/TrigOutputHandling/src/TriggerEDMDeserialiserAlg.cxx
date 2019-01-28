/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <cstring>
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/DataBucketBase.h"
#include "SGTools/DataProxy.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TriggerEDMDeserialiserAlg.h"
#include "BareDataBucket.h"
#include "TBufferFile.h"
#include "TClass.h"



TriggerEDMDeserialiserAlg::TriggerEDMDeserialiserAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

TriggerEDMDeserialiserAlg::~TriggerEDMDeserialiserAlg()
{
}

StatusCode TriggerEDMDeserialiserAlg::initialize()
{
  ATH_CHECK( m_resultKey.initialize() );
  ATH_CHECK( m_clidSvc.retrieve() );
  ATH_CHECK( m_serializerSvc.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMDeserialiserAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMDeserialiserAlg::execute(const EventContext& context) const
{

  size_t  buffSize = m_initialSerialisationBufferSize;
  std::unique_ptr<char[]> buff( new char[buffSize] );

  // returns a char* buffer that is at minimum as bg as specified in the argument
  auto resize = [&]( size_t neededSize )  {
		  if ( neededSize > buffSize ) {
		    buffSize = neededSize;
		    buff.reset( new char[buffSize] );
		  }
		};
    
  auto resultHandle = SG::makeHandle( m_resultKey, context );
  if ( resultHandle.isValid() )
    ATH_MSG_DEBUG("Obtained HLTResultMT " << m_resultKey.key() );
  
  const Payload* dataptr = nullptr;
  // TODO: revise if there are use cases where result may be not available in some events
  if ( resultHandle->getSerialisedData( m_moduleID, dataptr ).isFailure() ) {
    ATH_MSG_DEBUG("No payload available with moduleId " << m_moduleID << " in this event");
    return StatusCode::SUCCESS;
  }
  PayloadIterator start = dataptr->begin();
  
  while ( start != dataptr->end() )  {
    const CLID clid{ collectionCLID( start ) };
    const std::string name{ collectionName( start ) };
    const size_t bsize{ dataSize( start ) };
    std::string transientType;
    ATH_CHECK( m_clidSvc->getTypeNameOfID( clid, transientType ) );
    const std::string actualTypeName{ name.substr(0, name.find('#')) };
    const std::string key{ name.substr( name.find('#')+1 ) };
        
    ATH_MSG_DEBUG( "fragment: clid, type, key, size " << clid << " " << transientType<< " " << actualTypeName << " " << key << " " << bsize );
    resize( bsize );
    toBuffer( start, buff.get() );
        
    RootType classDesc = RootType::ByName( actualTypeName );
    size_t usedBytes{ bsize };
    void* obj = m_serializerSvc->deserialize( buff.get(), usedBytes, classDesc );
    ATH_MSG_DEBUG( "Obtained object " << obj << " which used " << usedBytes << " bytes from available " << bsize  );
    // for the moment I do not know what do with the raw prt

    if ( obj ) {
      BareDataBucket* dataBucket = new BareDataBucket( obj, clid, classDesc);
      const std::string outputName = m_prefix + key;
      auto proxyPtr = evtStore()->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( dataBucket ), outputName, false, false );
      if ( proxyPtr == nullptr )  {
	ATH_MSG_WARNING( "Recording of object of CLID " << clid << " and name " << outputName << " failed" );
      }
    } else {
      ATH_MSG_WARNING( "Deserialisation of object of CLID " << clid << " and type#name " << name << " failed" );
    }

    start = toNextFragment( start );
  }
  
  return StatusCode::SUCCESS;
}

size_t TriggerEDMDeserialiserAlg::nameLength( TriggerEDMDeserialiserAlg::PayloadIterator start ) const {
  return *( start + NameLengthOffset);
}

std::string TriggerEDMDeserialiserAlg::collectionName( TriggerEDMDeserialiserAlg::PayloadIterator start ) const {
  StringSerializer ss;
  std::vector<std::string> labels;
  ss.deserialize( start + NameOffset, start + NameOffset + nameLength(start), labels );
  return labels[0];
}
size_t TriggerEDMDeserialiserAlg::dataSize( TriggerEDMDeserialiserAlg::PayloadIterator start ) const {
  return *( start + NameOffset + nameLength( start ) );
}

void TriggerEDMDeserialiserAlg::toBuffer( TriggerEDMDeserialiserAlg::PayloadIterator start, char* buffer ) const {
  // move to the beginning of the buffer memory
  PayloadIterator dataStart =  start + NameOffset + nameLength(start) + 1 /*skip size*/;
  // we rely on continous memory layout of std::vector ...
  std::memcpy( buffer, &(*dataStart), dataSize( start ) );
}

