/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <map>

// Framework include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthLinks/ElementLinkBase.h"

// Local include(s):
#include "DerivationFrameworkExamples/FloatCompressionAlg.h"

#include "xAODCore/tools/FloatCompressor.h"

namespace DerivationFramework {

   FloatCompressionAlg::FloatCompressionAlg( const std::string& name,
                                             ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKeys", m_keys );
   }

   StatusCode FloatCompressionAlg::initialize() {
     
      // Tell the user what's happening:
     ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
     ATH_MSG_DEBUG( "SGKeys = " << m_keys );

     m_floatCompressor = std::make_unique<xAOD::FloatCompressor>( 7 );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

  StatusCode FloatCompressionAlg::execute() {


        std::cout << " Kerim In FloatCompressionAlg::execute " << std::endl;
	
	const SG::IConstAuxStore* store = nullptr;
	
	ATH_CHECK( evtStore()->retrieve( store, "AnalysisElectrons_NOSYSAux." ) );

	const SG::auxid_set_t& auxids = store->getAuxIDs();
	SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
	for( auto auxid : auxids ) {
	  std::string name = reg.getName( auxid );
	  
	  const std::string tname =
	      SG::normalizedTypeinfoName( *( reg.getType( auxid ) ) );

	  std::cout << " kerim auxid " << auxid << " name " << name << " tname " << tname << std::endl;
	}



      // Collect all the container(s):
      std::vector< std::pair< const SG::IConstAuxStore*, std::string > > stores;
      if( m_keys.size() ) {
         for( const std::string& key : m_keys ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, key ) );
            stores.push_back( std::make_pair( store, key ) );
         }
      } else {
         SG::ConstIterator< SG::IConstAuxStore > begin, end;
         ATH_CHECK( evtStore()->retrieve( begin, end ) );
         for( auto itr = begin; itr != end; ++itr ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, itr.key() ) );
            stores.push_back( std::make_pair( store, itr.key() ) );
         }
      }
      ATH_MSG_DEBUG( "Number of IConstAuxStore objects retrieved: "
                     << stores.size() );

      // Reset the ElementLinks in all of them:
      for( const auto& storeKey : stores ) {
         ATH_MSG_VERBOSE( "Reseting element links in store: "
                          << storeKey.second );
         ATH_CHECK( reset( *( storeKey.first ), storeKey.second ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

  StatusCode FloatCompressionAlg::reset( const SG::IConstAuxStore& store,
					 const std::string& key ) {
    
      // If the container is empty, return right away:
      //if( ! store.size() ) {
      //   return StatusCode::SUCCESS;
      //}
      
      // Get all the IDs stored in this object:
      const SG::auxid_set_t& auxids = store.getAuxIDs();
      
      // The auxiliary type registry:
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

      // Loop over them:
      for( SG::auxid_t auxid : auxids ) {
	
	const std::string tname =
	  SG::normalizedTypeinfoName( *( reg.getType( auxid ) ) );


	// Check/cache its type:
	if( m_typeCache.size() <= auxid ) {
	  m_typeCache.resize( auxid + 1 );
	}

	
	if( ! m_typeCache[ auxid ].isSet ) {
	    const std::string tname =
	      SG::normalizedTypeinfoName( *( reg.getType( auxid ) ) );
	    
	    std::cout << "Kerim " << tname << std::endl;

            static const std::string pat1 = "float";
            static const std::string pat2 = "std::vector<float>";
            if( tname.substr( 0, pat1.size() ) == pat1 ) {
	      m_typeCache[ auxid ].isFloat = true;
            } else if( tname.substr( 0, pat2.size() ) == pat2 ) {
	      m_typeCache[ auxid ].isFloatVec = true;
            }
            m_typeCache[ auxid ].isSet = true;
            ATH_MSG_VERBOSE( "Type for \"" << tname << "\": isFloat = "
                             << m_typeCache[ auxid ].isFloat << ", isFloatVec = "
                             << m_typeCache[ auxid ].isFloatVec );
         }
      
      
         // If it's not a float type, then don't bother:
         //if( ! ( m_typeCache[ auxid ].isFloat || m_typeCache[ auxid ].isFloatVec ) )
	
	// if( ! ( m_typeCache[ auxid ].isFloat || m_typeCache[ auxid ].isFloatVec) )
	// continue;
	
      
	std::cout << "Kerim key " << key << " name " << reg.getName( auxid ) << " type " << tname << " size " << reg.getEltSize( auxid ) << std::endl;

	std::string name = reg.getName( auxid );

	if ( !(name=="NumTrkPt500" || name=="SumPtTrkPt500" || name=="NumTrkPt1000" || name=="TrackWidthPt1000") )
	  continue;
   
	const size_t eltSize = reg.getEltSize( auxid );

	void* ptr = const_cast< void* >( store.getData( auxid ) );

	const size_t sz_i = store.size();
	
	

	for( size_t i = 0; i < sz_i; ++i ) {
	  
	  void* eltPtr = reinterpret_cast< char* >( ptr ) + i * eltSize;

	  
	  if (m_typeCache[ auxid ].isFloat) {
	    if (eltPtr) {

	      float val = *(float *) eltPtr;
	    
	      // val = m_floatCompressor->reduceFloatPrecision( val );
	    
	      // *(float *) eltPtr = val;
	    }
	  } else if  (m_typeCache[ auxid ].isFloatVec) {

	    std::vector<float> &vals =  *( reinterpret_cast< std::vector<float>* >( eltPtr ) );
	    
	    const size_t sz_j = vals.size();

	    for( size_t j = 0; j < sz_j; ++j )
	      ;
	    // vals[j] = m_floatCompressor->reduceFloatPrecision( vals[j] );

	    // vals.erase( vals.begin()+2, vals.end() );
	    
	  }
	  
	}
	
	continue;
	       	    

         // If the pointer is null, then something dodgy happened with this
         // (dynamic) variable.
         if( ! ptr ) {
            // Check if this is a static variable. If it is, it's not an error
            // to get a null pointer for it. Since such a case can only happen
            // when a new static variable was introduced into the EDM, and we're
            // reading an old input file that doesn't have this variable in it
            // yet. Which is an okay scenario.
            const SG::IAuxStoreIO* storeIO =
               dynamic_cast< const SG::IAuxStoreIO* >( &store );
            if( ( ! storeIO ) || ( storeIO->getDynamicAuxIDs().find( auxid ) !=
                                   storeIO->getDynamicAuxIDs().end() ) ) {
               REPORT_MESSAGE( MSG::ERROR )
                  << "Invalid pointer received for variable: " << key
                  << reg.getName( auxid );
            } else {
	        ATH_MSG_DEBUG( "Static variable " << key << reg.getName( auxid )
                              << " is empty" );
            }
            continue;
         }

      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   FloatCompressionAlg::AuxIDType::AuxIDType()
      : isSet( false ), isFloat( false ), isFloatVec( false ) {

   }

} // namespace xAODMaker
