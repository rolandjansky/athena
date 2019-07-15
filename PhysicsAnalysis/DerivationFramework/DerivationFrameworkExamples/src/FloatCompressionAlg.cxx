/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <map>
#include <string>

// Framework include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"

// Local include(s):
#include "DerivationFrameworkExamples/FloatCompressionAlg.h"

namespace DerivationFramework {

   FloatCompressionAlg::FloatCompressionAlg( const std::string& name,
                                             ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKeys", m_keys );
      declareProperty( "Vars", m_vars );
   }

   StatusCode FloatCompressionAlg::initialize() {
     
      // Tell the user what's happening:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "SGKeys = " << m_keys );
      ATH_MSG_DEBUG( "Vars = " << m_vars );

      // Initialise the float compressor objects.
      static const unsigned int NMANTISSA = 23;
      m_floatCompressors.reserve( NMANTISSA );
      for( unsigned int i = 0; i < NMANTISSA; ++i ) {
         m_floatCompressors.emplace_back( i );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

  StatusCode FloatCompressionAlg::execute() {

      // Collect all the configured container(s):
      std::vector< std::pair< const SG::IConstAuxStore*, std::string > > stores;
      if( m_keys.size() ) {
         for( const std::string& key : m_keys ) {
            const SG::IConstAuxStore* store = 0;
            ATH_CHECK( evtStore()->retrieve( store, key ) );
            stores.push_back( std::make_pair( store, key ) );
         }
      } 
      ATH_MSG_DEBUG( "Number of IConstAuxStore objects retrieved: "
                     << stores.size() );

      // Collect all the variables(s):
      std::vector< std::pair< const SG::IConstAuxStore*, std::map<std::string, std::vector<std::pair<std::string,int>>> > > stores_vars;
      if( m_vars.size() ) {
	std::map<std::string, std::vector<std::pair<std::string,int>>> mapOfvars;
	
	for( const std::string& mykey : m_vars ) {
	  // Split container and variable+precision
	  std::size_t found = mykey.find(".");
	  std::string key = mykey.substr(0,found+1);
	  std::string var_prec = mykey.substr(found+1, std::string::npos);	   
	  // Split variable and precision
	  found = var_prec.find(":");
	  std::string var = var_prec.substr(0,found);
	  int prec = std::stoi(var_prec.substr(found+1, std::string::npos));
	  // Now store it in a map
	  std::vector<std::pair<std::string,int>> myVars;
	  if (mapOfvars.count(key)) {
	    myVars = mapOfvars[key];
	  }
	  myVars.push_back(std::make_pair( var, prec ) );
	  mapOfvars[key] = myVars;

	}
	// Add the StoreGate info to the variable map
	for( auto& mypair : mapOfvars ) {
	  const SG::IConstAuxStore* store = 0;
	  ATH_CHECK( evtStore()->retrieve( store, mypair.first ) );
	  std::map<std::string, std::vector<std::pair<std::string,int>>> mymapOfvars;
	  mymapOfvars[mypair.first] = mypair.second;
	  stores_vars.push_back( std::make_pair( store, mymapOfvars ));
	}
      }

      // Apply float compression in all the configured AUX containers
      for( const auto& storeKey : stores ) {
         ATH_MSG_VERBOSE( "Apply float compression of AUX containers in store: "
                          << storeKey.second );
         ATH_CHECK( reset( *( storeKey.first ), storeKey.second ) );
      }
	       
      // Apply float compression in the configured AUX variables
      for( const auto& storeKey : stores_vars ) {
         ATH_MSG_VERBOSE( "Apply float compression of AUX variables in store: "
                          << storeKey.second );
         ATH_CHECK( resetVars( *( storeKey.first ), storeKey.second ) );
      }
      
      // Return gracefully:
      return StatusCode::SUCCESS;
  }

  StatusCode FloatCompressionAlg::reset( const SG::IConstAuxStore& store,
					 const std::string& key ) {

      // If the container is empty, return right away:
      if( ! store.size() ) {
         return StatusCode::SUCCESS;
      }

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
      
	ATH_MSG_DEBUG( "key=" << key << ", name=" << reg.getName( auxid ) << ", type=" << tname << ", size=" << reg.getEltSize( auxid ) );

	std::string name = reg.getName( auxid );
	const size_t eltSize = reg.getEltSize( auxid );
	void* ptr = const_cast< void* >( store.getData( auxid ) );
	const size_t sz_i = store.size();

   static const unsigned int DEFAULT_NMANTISSA = 7;

	for( size_t i = 0; i < sz_i; ++i ) {
	  
	  void* eltPtr = reinterpret_cast< char* >( ptr ) + i * eltSize;

	  
	  if (m_typeCache[ auxid ].isFloat) {
	    if (eltPtr) {
	      float val = *(float *) eltPtr;
	      val = m_floatCompressors[ DEFAULT_NMANTISSA ].reduceFloatPrecision( val );
	      *(float *) eltPtr = val;
	    }
	  } else if  (m_typeCache[ auxid ].isFloatVec) {

	    std::vector<float> &vals =  *( reinterpret_cast< std::vector<float>* >( eltPtr ) );
	    
	    const size_t sz_j = vals.size();

	    for( size_t j = 0; j < sz_j; ++j ) {
          vals[j] = m_floatCompressors[ DEFAULT_NMANTISSA ].reduceFloatPrecision( vals[j] );
	    }	    

	    // vals.erase( vals.begin()+2, vals.end() );
	    
	  }
	  
	}

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
               REPORT_MESSAGE( MSG::DEBUG )
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

  StatusCode FloatCompressionAlg::resetVars( const SG::IConstAuxStore& store,
					     const std::map<std::string, std::vector<std::pair<std::string,int>>> key ) {
    
      // If the container is empty, return right away:
      if( ! store.size() ) {
         return StatusCode::SUCCESS;
      }
      
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

	ATH_MSG_DEBUG( "key=" << key << ", name=" << reg.getName( auxid ) << ", type=" << tname << ", size=" << reg.getEltSize( auxid ) );      
	
	std::string name = reg.getName( auxid );
	const size_t eltSize = reg.getEltSize( auxid );
	void* ptr = const_cast< void* >( store.getData( auxid ) );
	const size_t sz_i = store.size();
	
	for( size_t i = 0; i < sz_i; ++i ) {
	  
	  void* eltPtr = reinterpret_cast< char* >( ptr ) + i * eltSize;
	  if (m_typeCache[ auxid ].isFloat) {
	    if (eltPtr) {

	      float val = *(float *) eltPtr;
	      for( auto& mykey : key ) {
		for ( auto &myvar : mykey.second ) {
		  std::string var = myvar.first;
		  int prec = myvar.second;
		  if (var == reg.getName( auxid )) {
		    val = m_floatCompressors[ prec ].reduceFloatPrecision( val );
		  }
		}
	      }

	      *(float *) eltPtr = val;
	    }
	  } else if  (m_typeCache[ auxid ].isFloatVec) {

	    for( auto& mykey : key ) {
	      for ( auto &myvar : mykey.second ) {
		std::string var = myvar.first;
		int prec = myvar.second;
		if (var == reg.getName( auxid )) {

		  std::vector<float> &vals =  *( reinterpret_cast< std::vector<float>* >( eltPtr ) );
		  const size_t sz_j = vals.size();

		  for( size_t j = 0; j < sz_j; ++j ) {
		    vals[j] = m_floatCompressors[ prec ].reduceFloatPrecision( vals[j] );
		  }	    
		}
	      }
	    }

	    // vals.erase( vals.begin()+2, vals.end() );
	    
	  }
	  
	}

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
               REPORT_MESSAGE( MSG::DEBUG )
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
