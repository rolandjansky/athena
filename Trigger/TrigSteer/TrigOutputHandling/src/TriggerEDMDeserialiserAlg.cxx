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

StatusCode TriggerEDMDeserialiserAlg::execute_r(const EventContext& context) const
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
  const Payload* dataptr = nullptr;
  // TODO: revise if there are use cases where result may be not available in some events
  ATH_CHECK( resultHandle->getSerialisedData( m_moduleID, dataptr ) );
  PayloadIterator start = dataptr->begin();
  
  while ( start != dataptr->end() )  {
    const CLID clid{ collectionCLID( start ) };
    const std::string name{ collectionName( start ) };
    const size_t bsize{ dataSize( start ) };
    std::string transientType;
    ATH_CHECK( m_clidSvc->getTypeNameOfID( clid, transientType ) );
    ATH_MSG_DEBUG( "fragment: clid, name, size " << clid << " " << transientType<< "#" << name << " " << bsize );
    resize( bsize );
    toBuffer( start, buff.get() );

    
    RootType classDesc = RootType::ByName( transientType+"_v1" ); // TODO remove this dirty hack, needsdiscussion how to find the real type
    size_t usedBytes{ bsize };
    void* obj = m_serializerSvc->deserialize( buff.get(), usedBytes, classDesc );
    ATH_MSG_DEBUG( "Obtained object " << obj << " which used " << usedBytes << " from available " << bsize  );
    // for the moment I do not know what do with the raw prt

    if ( obj ) {
      BareDataBucket* dataBucket = new BareDataBucket( obj, clid, classDesc);
      const std::string outputName = m_prefix + name;
      auto proxyPtr = evtStore()->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( dataBucket ), outputName, false, false );
      if ( proxyPtr == nullptr )  {
	ATH_MSG_WARNING( "Recording of object of CLID " << clid << " and name " << name << " failed" );
      }
    } else {
      ATH_MSG_WARNING( "Deserialisation of object of CLID " << clid << " and name " << name << " failed" );
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

