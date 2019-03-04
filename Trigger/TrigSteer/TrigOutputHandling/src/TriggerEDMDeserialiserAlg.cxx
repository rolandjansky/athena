/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstring>
#include "AthenaKernel/StorableConversions.h"
#include "AthenaKernel/DataBucketBase.h"
#include "SGTools/DataProxy.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "BareDataBucket.h"
#include "TBufferFile.h"
#include "TVirtualCollectionProxy.h"
#include "TClass.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainersInterfaces/AuxTypes.h"
#include "xAODCore/AuxContainerBase.h"
#include "AthContainersRoot/getDynamicAuxID.h"
#include "RootUtils/Type.h"

#include "TriggerEDMDeserialiserAlg.h"


class TriggerEDMDeserialiserAlg::WritableAuxStore : public SG::AuxStoreInternal {
public:
  WritableAuxStore() {}
  using SG::AuxStoreInternal::addVector;
};


namespace  {
  /**
   * @brief Find the type of an element of a vector.
   * @param[in] tname The name of the type to analyze.
   * @param[out] elementTypeName The name of the type of an element of the vector.
   *
   * Returns the @c type_info for an element of the vector.
   */
  const std::type_info* getElementType ( const std::string& tname,
					 std::string& elementTypeName ) {
    TClass* cls = TClass::GetClass( tname.c_str() );
    if ( !cls ) return nullptr;
    TVirtualCollectionProxy* prox = cls->GetCollectionProxy();
    if ( !prox ) return nullptr;
    if ( prox->GetValueClass() ) {
      elementTypeName = prox->GetValueClass()->GetName();
      return prox->GetValueClass()->GetTypeInfo();
    }
    RootUtils::Type type ( prox->GetType() );
    elementTypeName = type.getTypeName();
    return type.getTypeInfo();
  }

  
  std::string stripStdVec (const std::string& s_in) {
    std::string s = s_in;
    std::string::size_type pos;
    while ((pos = s.find ("std::vector<")) != std::string::npos) {
      s.erase (pos, 5);
    }
    return s;
  }
  
}


TriggerEDMDeserialiserAlg::TriggerEDMDeserialiserAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {}

TriggerEDMDeserialiserAlg::~TriggerEDMDeserialiserAlg() {}

StatusCode TriggerEDMDeserialiserAlg::initialize() {
  ATH_CHECK( m_resultKey.initialize() );
  ATH_CHECK( m_clidSvc.retrieve() );
  ATH_CHECK( m_serializerSvc.retrieve() );
  ATH_CHECK( m_tpTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMDeserialiserAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMDeserialiserAlg::execute(const EventContext& context) const {    

  auto resultHandle = SG::makeHandle( m_resultKey, context );
  if ( resultHandle.isValid() )
    ATH_MSG_DEBUG("Obtained HLTResultMT " << m_resultKey.key() );
  
  const Payload* dataptr = nullptr;
  // TODO: check if there are use cases where result may be not available in some events and this is not an issue at all
  if ( resultHandle->getSerialisedData( m_moduleID, dataptr ).isFailure() ) {
    ATH_MSG_WARNING("No payload available with moduleId " << m_moduleID << " in this event");
    return StatusCode::SUCCESS;
  }
  ATH_CHECK( deserialise( dataptr ) );
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMDeserialiserAlg::deserialise(   const Payload* dataptr  ) const {

  size_t  buffSize = m_initialSerialisationBufferSize;
  std::unique_ptr<char[]> buff( new char[buffSize] );

  // returns a char* buffer that is at minimum as bg as specified in the argument
  auto resize = [&]( size_t neededSize )  {
		  if ( neededSize > buffSize ) {
		    buffSize = neededSize;
		    buff.reset( new char[buffSize] );
		  }
		};  


  WritableAuxStore* currentAuxStore = nullptr; // set when decoding Aux
  int fragmentCount = 0;
  PayloadIterator start = dataptr->begin();
  while ( start != dataptr->end() )  {
    fragmentCount++;
    const CLID clid{ collectionCLID( start ) };
    const std::vector<std::string> descr{ collectionDescription( start ) };
    ATH_CHECK( descr.size() == 3 );
    std::string transientTypeName{ descr[0] };
    std::string persistentTypeName{ descr[1] };
    const std::string key{ descr[2] };
    const size_t bsize{ dataSize( start ) };

    ATH_MSG_DEBUG( "" );
    ATH_MSG_DEBUG( "fragment: " << fragmentCount << " type: "<< transientTypeName << " persistent type: " <<  persistentTypeName << " key: " << key << " size: " << bsize );
    resize( bsize );
    toBuffer( start, buff.get() );

    start = toNextFragment( start ); // point the start to the next chunk, irrespectively of what happens in deserialisation below
        
    RootType classDesc = RootType::ByName( persistentTypeName );
    size_t usedBytes{ bsize };
    void* obj = m_serializerSvc->deserialize( buff.get(), usedBytes, classDesc );

    ATH_MSG_DEBUG( "Deserialised object of ptr: " << obj << " which used: " << usedBytes << " bytes from available: " << bsize );
    if ( obj == nullptr ) {
      ATH_MSG_ERROR( "Deserialisation of object of CLID " << clid << " and transientTypeName " << transientTypeName << " # " << key << " failed" );
      ATH_CHECK(false);
    }
    const bool isxAODInterfaceContainer = transientTypeName.find("xAOD")   != std::string::npos and transientTypeName.find("Aux") == std::string::npos;
    const bool isxAODAuxContainer       = transientTypeName.find("xAOD")   != std::string::npos and transientTypeName.find("Aux") != std::string::npos;
    const bool isxAODDecoration	        = transientTypeName.find("vector") != std::string::npos;
    const bool isTPContainer	        = persistentTypeName.find("_p")	   != std::string::npos;
    
    ATH_CHECK( checkSanity( transientTypeName, isxAODInterfaceContainer, isxAODAuxContainer, isxAODDecoration, isTPContainer ) );
    
    if ( isTPContainer ) {
      std::string decodedTransientName;
      void * converted = m_tpTool->convertPT( persistentTypeName, obj, decodedTransientName );
      ATH_CHECK( converted != nullptr );
      ATH_CHECK( decodedTransientName == transientTypeName );      
      classDesc.Destruct( obj );
      obj = converted;      
    }

    if ( isxAODInterfaceContainer or isxAODAuxContainer or isTPContainer ) {
      BareDataBucket* dataBucket = new BareDataBucket( obj, clid, classDesc);
      const std::string outputName = m_prefix + key;
      auto proxyPtr = evtStore()->recordObject( SG::DataObjectSharedPtr<BareDataBucket>( dataBucket ), outputName, false, false );
      if ( proxyPtr == nullptr )  {
	ATH_MSG_WARNING( "Recording of object of CLID " << clid << " and name " << outputName << " failed" );
      }
      
      if ( isxAODAuxContainer )  {
	ATH_CHECK( key.back() == '.' );
	ATH_CHECK( std::count( key.begin(), key.end(), '.')  == 1 );
	xAOD::AuxContainerBase* auxHolder = reinterpret_cast<xAOD::AuxContainerBase*>(dataBucket->object());
	ATH_CHECK( auxHolder != nullptr );	
	currentAuxStore = new WritableAuxStore();
	auxHolder->setStore( currentAuxStore );
      }  else {
	currentAuxStore = nullptr;
      }

    } else if ( isxAODDecoration ) {      
      ATH_CHECK( deserialiseDynAux( transientTypeName, persistentTypeName, key, obj, currentAuxStore) );
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode TriggerEDMDeserialiserAlg::deserialiseDynAux( const std::string& transientTypeName, const std::string& persistentTypeName, const std::string& decorationName,
							 void* obj,   WritableAuxStore* currentAuxStore ) const {
  const bool isPacked = persistentTypeName.find("SG::PackedContainer") != std::string::npos;      

  SG::AuxTypeRegistry& registry = SG::AuxTypeRegistry::instance();     
  SG::auxid_t id = registry.findAuxID ( decorationName );
  if (id != SG::null_auxid ) {
    if ( stripStdVec( registry.getVecTypeName(id) ) != stripStdVec(transientTypeName) ) {
      ATH_MSG_INFO( "Schema evolution required for decoration " << decorationName << " from " << transientTypeName << " to "  <<  registry.getVecTypeName( id ) << " not handled yet"); 
      return StatusCode::SUCCESS;
    }
  } else {
      std::string elementTypeName;
      const std::type_info* elt_tinfo = getElementType( transientTypeName, elementTypeName );
      ATH_CHECK( elt_tinfo != nullptr );
      ATH_MSG_DEBUG( "Dynamic decoration: " << decorationName << " of type " << transientTypeName << " will create a dynamic ID, stored type" << elementTypeName );    
      id = SG::getDynamicAuxID ( *elt_tinfo, decorationName, elementTypeName, transientTypeName, false );
  }        
  ATH_MSG_DEBUG( "Unstreaming decoration " << decorationName << " of type " << transientTypeName  << " aux ID " << id << " class " << persistentTypeName << " packed " << isPacked  );  
  std::unique_ptr<SG::IAuxTypeVector> vec( registry.makeVectorFromData (id, obj, isPacked, true) );
  ATH_CHECK( vec.get() != nullptr );
  ATH_CHECK( currentAuxStore != nullptr );
  currentAuxStore->addVector(id, std::move(vec), false);
    
  return StatusCode::SUCCESS;  
}

StatusCode TriggerEDMDeserialiserAlg::checkSanity( const std::string& transientTypeName, bool isxAODInterfaceContainer, bool isxAODAuxContainer, bool isDecoration, bool isTPContainer ) const {
  ATH_MSG_DEBUG( "Recognised type " << transientTypeName <<" as: " 
		 << (isxAODInterfaceContainer ?" xAOD Interface Container":"" ) 
		 << (isxAODAuxContainer ?" xAOD Aux Container ":"" ) 
		 << ( isDecoration ? " xAOD Decoration" : "") 
		 << ( isTPContainer ? " T/P Contianer " : "") );
  
  const std::vector<bool> typeOfContainer( { isxAODInterfaceContainer, isxAODAuxContainer, isDecoration, isTPContainer } );			     
  const size_t count = std::count( typeOfContainer.begin(), typeOfContainer.end(), true );
  if ( count == 0 ) {
    ATH_MSG_ERROR( "Could not recognise the kind of container " << transientTypeName );
    return StatusCode::FAILURE;
  } else if (count > 1 ) {
    ATH_MSG_ERROR( "Ambigous kind serialised content deduced from the transient type name " << transientTypeName );
    ATH_MSG_ERROR( "Recognised type as: " 
		   << (isxAODInterfaceContainer ?" xAOD Interface Context":"" ) 
		   << (isxAODAuxContainer ?" xAOD Aux Container ":"" ) 
		   << ( isDecoration ? " xAOD Decoration" : "") 
		   << ( isTPContainer ? " T/P Contianer " : "") );
    return StatusCode::FAILURE;    
  }
  return StatusCode::SUCCESS;
}


size_t TriggerEDMDeserialiserAlg::nameLength( TriggerEDMDeserialiserAlg::PayloadIterator start ) const {
  return *( start + NameLengthOffset);
}

std::vector<std::string> TriggerEDMDeserialiserAlg::collectionDescription( TriggerEDMDeserialiserAlg::PayloadIterator start ) const {
  StringSerializer ss;
  std::vector<std::string> labels;
  ss.deserialize( start + NameOffset, start + NameOffset + nameLength(start), labels );
  return labels;
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
