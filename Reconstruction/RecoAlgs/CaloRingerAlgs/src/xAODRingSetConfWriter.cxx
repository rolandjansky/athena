/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// STL include(s)
#include <algorithm>
#include <sstream>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Tool interface includes:
#include "CaloRingerTools/ICaloRingsBuilder.h"

// EDM include(s):
#include "xAODCore/AuxInfoBase.h"
#include "xAODCaloRings/RingSetConfAuxContainer.h"

// Local include(s):
#include "xAODRingSetConfWriter.h"

namespace Ringer {


// =================================================================================
xAODRingSetConfWriter::xAODRingSetConfWriter( const std::string& name,
                               ::ISvcLocator* svcLoc ) : 
  ::AthAlgorithm( name, svcLoc ),
  m_metaStore( "MetaDataStore", name ),
  m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name )
{

  declareProperty( "MetaDataStore", m_metaStore, 
      "The metadata Storage ServiceHandle. If empty, it will use "
      "MetaDataStore service with same name as this algorithm.");

  declareProperty( "CaloRingsBuilderTools", m_crBuilderTools ,
      "The CaloRingsBuilder tools handles array.");

  declareProperty( "RingSetConfContainerNames", m_rsMetaNames, 
      "The metadata RingSet Configuration container keys to write." );

}

// =================================================================================
StatusCode xAODRingSetConfWriter::initialize() {

  // Greet the user:
  ATH_MSG_INFO( "Initialising " << name() << 
      " - Package version: " << PACKAGE_VERSION );

  // Make sure that configuration param are equivalent in size:
  if ( m_crBuilderTools.size() != m_rsMetaNames.size() )
  {

    ATH_MSG_ERROR("Configuration error: every CaloRingsBuilder should "
        "have its own RingSet/CaloRings names "
        "set via the RingSetConfMetaNames "
        "python configurable.");

    ATH_MSG_ERROR("The retrieven m_RingSet names are: " << m_rsMetaNames );
    ATH_MSG_ERROR("The retrieven m_crBuilderTools names are: " 
        << [this]()
        {
          std::vector<std::string> names;
          for ( const auto& tool : m_crBuilderTools )
          {
            names.push_back( tool.name() );
          }
          return names;
        }()
      );

    return StatusCode::FAILURE;
  }

  // Display the configuration:
  ATH_MSG_DEBUG( "RingSetConfMetaNames = " << m_rsMetaNames );
  if ( msg().level() <= MSG::DEBUG ){ // Display 
    std::vector<std::string> toolNames;
    toolNames.reserve( m_crBuilderTools.size() );
    for ( const auto &tool : m_crBuilderTools ){
      toolNames.push_back( tool.name() );
    }
    ATH_MSG_DEBUG( "CaloRingsBuilderTools = " << toolNames );
  }

  ATH_MSG_VERBOSE( "inputMetaStore = " << m_inputMetaStore->dump() );

  // Retrieve the necessary service(s):
  CHECK( m_metaStore.retrieve() );
  CHECK( m_inputMetaStore.retrieve() );

  // Now work to set xAOD RingSet/CaloRings configuration metadata available on
  // the output meta store:
  CHECK( retrieveCaloRingsBuilders() );
  CHECK( allocateContainers() );
  CHECK( fillConfigurations() );

  // NOTE: This must be called after fillConfigurations, otherwise it will
  // attempt to fill those configurations retrieven from the builders.
  CHECK( copyInputMetaStore() );

  // Print-out configurations:
  ATH_MSG_DEBUG("There are available a total of " << m_rsConfContVec.size() << " RingSetConfContainer(s).");
  for ( const auto* c : m_rsConfContVec ) {
    if ( nullptr != c ) {
      if ( msg().level() <= MSG::VERBOSE ) {
        for ( const auto& r : *c ){
          std::ostringstream str;
          r->print( str );
          ATH_MSG_VERBOSE( str.str() );
        }
      }
    } else {
      ATH_MSG_WARNING("Container " << c << "is empty!");
    }
  }

  ATH_MSG_DEBUG("Obtained configuration succesfully.");

  ATH_MSG_VERBOSE( "outputMetaStore = " << m_metaStore->dump() );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode xAODRingSetConfWriter::copyInputMetaStore()
{
  ATH_MSG_DEBUG("Checking if it is needed to copy RingSetConf "
      "MetaData on input file.");

  CHECK( searchAndCopyCLID< xAOD::RingSetConfAuxContainer >( 
        ClassID_traits< xAOD::RingSetConfContainer >() ) );

  return StatusCode::SUCCESS;
}

// =================================================================================
template< class auxT, class T >
StatusCode xAODRingSetConfWriter::searchAndCopyCLID( 
    const ClassID_traits< T >& classID )
{
  std::vector< std::string > ringConfKeys;

  // Retrieve our container keys:
  m_inputMetaStore->keys( classID.ID() , ringConfKeys); 
  
  ATH_MSG_DEBUG( "Available keys with type(" << classID.typeName() << ") are: " 
      << ringConfKeys );

  // Now loop retrieving them and recording on the outputContainer:
  for ( const auto &key : ringConfKeys ) 
  {
    if (key[0] == ';' && key[3] == ';') continue; // ignore versioned keys
    ATH_MSG_VERBOSE( "Attempting to copy " << key );

    // Check if this meta isn't already available in output, if so, do not copy it:
    if ( std::any_of( m_rsMetaNames.begin(), m_rsMetaNames.end(),  
        [&key](std::string &builderKey){
          return (builderKey == key) || ((builderKey + "Aux.") == key);
        } ) )
    {
      ATH_MSG_VERBOSE( "This key is already going to be built "
          "(outputMetaNames = " << m_rsMetaNames << ")"
          ", it will NOT be copied!" );
      continue;
    }

    // Retrieve and record its copy in output store
    CHECK( (copyKeyToStore<auxT, T>( key )) ); 
    ATH_MSG_INFO( "Successfully copied key " << key );
  }

  return StatusCode::SUCCESS;

}

// =================================================================================
template < class auxT, class T >
StatusCode xAODRingSetConfWriter::copyKeyToStore( const std::string &key )
{
  typedef typename T::value_type value_type;
  typedef typename T::base_value_type base_value_type;
  // Retrieve container
  const T* cont(nullptr);
  CHECK( m_inputMetaStore->retrieve( cont, key ) );

  // Create new containers:
  auxT* contAuxCopy = new auxT;
  T* contCopy = new T;
  contCopy->setStore( contAuxCopy );

  // Copy them:
  contCopy->reserve( cont->size() );
  contAuxCopy->reserve( cont->size() );
  ATH_MSG_DEBUG("Copying object with key: " << key);
  for ( const base_value_type* obj : *cont ) {
    ATH_MSG_VERBOSE("Original object:");
    // Print-out object:
    if( msgLevel() <= MSG::VERBOSE ) {
      std::ostringstream str;
      obj->print( str );
      ATH_MSG_VERBOSE( str.str() );
    }
    // Copy object
    value_type objCopy = new (base_value_type)( *obj );
    // Print-out object:
    ATH_MSG_VERBOSE("Copied object:");
    if( msgLevel() <= MSG::VERBOSE ) {
      std::ostringstream str;
      objCopy->print( str );
      ATH_MSG_VERBOSE( str.str() );
    }
    // Add to container
    contCopy->push_back( objCopy );
  }
  
  // And record:
  CHECK( m_metaStore->record( contAuxCopy, key + "Aux.") );
  CHECK( m_metaStore->record( contCopy, key) );

  return StatusCode::SUCCESS;
}


// =================================================================================
StatusCode xAODRingSetConfWriter::retrieveCaloRingsBuilders()
{
  ATH_MSG_INFO("Retrieving " << m_crBuilderTools.size() <<
    " reader tools for " << name() );

  ATH_CHECK(m_crBuilderTools.retrieve());

  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode xAODRingSetConfWriter::allocateContainers()
{

  for (size_t counter = 0; counter < m_rsMetaNames.size(); ++counter)
  {
    // Create an empty RingSetConf container:
    xAOD::RingSetConfAuxContainer* rsAux = new xAOD::RingSetConfAuxContainer();
    xAOD::RingSetConfContainer* rsCont = new xAOD::RingSetConfContainer();
    rsCont->setStore( rsAux );
    m_rsConfContVec.push_back( rsCont );

    // Record the RingSet configuration metadata into output container:
    CHECK( m_metaStore->record( rsAux, m_rsMetaNames[counter] + "Aux." ) );
    CHECK( m_metaStore->record( rsCont, m_rsMetaNames[counter] ) );
  }

  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode xAODRingSetConfWriter::fillConfigurations()
{

  for (size_t counter = 0; counter < m_rsConfContVec.size(); ++counter)
  {
    auto& crBuilder = m_crBuilderTools[counter];
    auto& rsCont = m_rsConfContVec[counter];

    // Create the xAOD configuration object (it will populate
    // RingSetConfContainer for us):
    xAOD::RingSetConf::fillRingSetConfContainer( 
        crBuilder->rawConf(), 
        rsCont);

    // Inform what we did:
    ATH_MSG_DEBUG( "Got configuration for " << crBuilder->name() << "." );

  }

  return StatusCode::SUCCESS;

}


// =================================================================================
StatusCode xAODRingSetConfWriter::execute() 
{
  // Our execute don't do anything... we don't have any assyncronous change of
  // configuration, so we don't need to keep track of what happens here.
  // Return gracefully:
  return StatusCode::SUCCESS;
}

// =================================================================================
StatusCode xAODRingSetConfWriter::finalize() 
{
  return StatusCode::SUCCESS;
}

} // namespace Ringer
