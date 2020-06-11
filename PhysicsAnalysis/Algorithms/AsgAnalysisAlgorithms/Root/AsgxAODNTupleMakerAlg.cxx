// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

// Local include(s):
#include "AsgAnalysisAlgorithms/AsgxAODNTupleMakerAlg.h"

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/normalizedTypeinfoName.h"

// Framework include(s):
#include "SystematicsHandles/Helpers.h"

// ROOT include(s):
#include <TClass.h>
#include <TTree.h>
#include <TBranch.h>
#include <TVirtualCollectionProxy.h>

// System include(s):
#include <regex>
#include <algorithm>
#include <functional>
#include <sstream>

namespace {

#ifdef XAOD_STANDALONE

   /// Get an xAOD container from the event store
   ///
   /// This is the "standalone implementation" of the function.
   ///
   /// @param key The name of the container in the event store
   /// @param evtStore The value of evtStore() from the algorithm
   /// @param allowMissing Set to @c true to print an error message in case
   ///                     of a failure
   /// @param msg Reference to the caller's @c MsgStream object
   /// @return A pointer to the container if successful, @c nullptr if not
   ///
   const SG::AuxVectorBase* getVector( const std::string& key,
                                       asg::SgTEvent& evtStore,
                                       bool allowMissing,
                                       MsgStream& msg ) {
      if( allowMissing &&
          ( ! evtStore.contains< const SG::AuxVectorBase >( key ) ) ) {
         return nullptr;
      }
      const SG::AuxVectorBase* c = nullptr;
      if( ! evtStore.retrieve( c, key ).isSuccess() ) {
         msg << MSG::ERROR << "Couldn't retrieve container with key \"" << key
             << "\"" << endmsg;
         return nullptr;
      }
      return c;
   }

   /// Get a standalone xAOD object from the event store
   ///
   /// This is the "standalone implementation" of the function.
   ///
   /// @param key The name of the container in the event store
   /// @param evtStore The value of evtStore() from the algorithm
   /// @param allowMissing Set to @c true to print an error message in case
   ///                     of a failure
   /// @param msg Reference to the caller's @c MsgStream object
   /// @return A pointer to the container if successful, @c nullptr if not
   ///
   const SG::AuxElement* getElement( const std::string& key,
                                     asg::SgTEvent& evtStore,
                                     bool allowMissing,
                                     MsgStream& msg ) {
      if( allowMissing &&
          ( ! evtStore.contains< const SG::AuxElement >( key ) ) ) {
         return nullptr;
      }
      const SG::AuxElement* e = nullptr;
      if( ! evtStore.retrieve( e, key ).isSuccess() ) {
         msg << MSG::ERROR << "Couldn't retrieve object with key \"" << key
             << "\"" << endmsg;
         return nullptr;
      }
      return e;
   }

#else

   /// Predicate for finding a proxy with a given name
   class ProxyWithName {
   public:
      /// Type of the predicate's argument
      typedef const SG::DataProxy* argument_type;
      /// Constructor with key/name
      ProxyWithName( const std::string& name ) : m_name( name ) {}
      /// Operator evaluating whether this is a good proxy or not
      bool operator()( argument_type proxy ) const {
         return ( proxy->name() == m_name );
      }
   private:
      std::string m_name; ///< Name for the proxy to find
   }; // class ProxyWithName

   /// Get an xAOD container from the event store
   ///
   /// This is the "Athena implementation" of the function.
   ///
   /// @param key The name of the container in the event store
   /// @param evtStore The value of evtStore() from the algorithm
   /// @param allowMissing Set to @c true to print an error message in case
   ///                     of a failure
   /// @param msg Reference to the caller's @c MsgStream object
   /// @return A pointer to the container if successful, @c nullptr if not
   ///
   const SG::AuxVectorBase* getVector( const std::string& key,
                                       IProxyDict& evtStore,
                                       bool allowMissing,
                                       MsgStream& msg ) {

      // Find all proxies with this key:
      auto proxies = evtStore.proxies();
      proxies.erase( std::remove_if( proxies.begin(), proxies.end(),
                                     std::not1( ProxyWithName( key ) ) ),
                     proxies.end() );
      // Now iterate over them:
      for( const SG::DataProxy* proxy : proxies ) {
         // We need a non-const version of it... :-(
         SG::DataProxy* proxy_nc = const_cast< SG::DataProxy* >( proxy );
         // Try to get the right object out of it.
         DataBucketBase* bucket =
            dynamic_cast< DataBucketBase* >( proxy_nc->accessData() );
         if( ! bucket ) {
            // This is a big problem in the job. Return right away.
            msg << MSG::ERROR
                << "Couldn't access data object as a data bucket?!?" << endmsg;
            return nullptr;
         }
         // Get the dictionary for the type:
         TClass* cl = TClass::GetClass( bucket->tinfo() );
         if( ! cl ) {
            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "No dictionary found for: "
                   << bucket->tinfo().name() << endmsg;
            }
            continue;
         }
         // Check whether the object inherits from AuxVectorBase:
         if( ! cl->InheritsFrom( "SG::AuxVectorBase" ) ) {
            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "Object \"" << key << "/" << cl->GetName()
                   << "\" does not inherit from SG::AuxVectorBase" << endmsg;
            }
            continue;
         }
         // If all is well, just assume that the inheritance is direct/simple:
         const SG::AuxVectorBase* result =
            reinterpret_cast< const SG::AuxVectorBase* >( bucket->object() );
         return result;
      }

      // Apparently we failed...
      if( ! allowMissing ) {
         msg << MSG::ERROR << "Couldn't retrieve object \"" << key
             << "\" as SG::AuxVectorBase" << endmsg;
      }
      return nullptr;
   }

   /// Get a standalone xAOD object from the event store
   ///
   /// This is the "Athena implementation" of the function.
   ///
   /// @param key The name of the container in the event store
   /// @param evtStore The value of evtStore() from the algorithm
   /// @param allowMissing Set to @c true to print an error message in case
   ///                     of a failure
   /// @param msg Reference to the caller's @c MsgStream object
   /// @return A pointer to the container if successful, @c nullptr if not
   ///
   const SG::AuxElement* getElement( const std::string& key,
                                     IProxyDict& evtStore,
                                     bool allowMissing,
                                     MsgStream& msg ) {

      // Find all proxies with this key:
      auto proxies = evtStore.proxies();
      proxies.erase( std::remove_if( proxies.begin(), proxies.end(),
                                     std::not1( ProxyWithName( key ) ) ),
                     proxies.end() );
      // Now iterate over them:
      for( const SG::DataProxy* proxy : proxies ) {
         // We need a non-const version of it... :-(
         SG::DataProxy* proxy_nc = const_cast< SG::DataProxy* >( proxy );
         // Try to get the right object out of it.
         DataBucketBase* bucket =
            dynamic_cast< DataBucketBase* >( proxy_nc->accessData() );
         if( ! bucket ) {
            // This is a big problem in the job. Return right away.
            msg << MSG::ERROR
                << "Couldn't access data object as a data bucket?!?" << endmsg;
            return nullptr;
         }
         // Get the dictionary for the type:
         TClass* cl = TClass::GetClass( bucket->tinfo() );
         if( ! cl ) {
            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "No dictionary found for: "
                   << bucket->tinfo().name() << endmsg;
            }
            continue;
         }
         // Check whether the object inherits from AuxVectorBase:
         if( ! cl->InheritsFrom( "SG::AuxElement" ) ) {
            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "Object \"" << key << "/" << cl->GetName()
                   << "\" does not inherit from SG::AuxElement" << endmsg;
            }
            continue;
         }
         // If all is well, just assume that the inheritance is direct/simple:
         return reinterpret_cast< const SG::AuxElement* >( bucket->object() );
      }

      // Apparently we failed...
      if( ! allowMissing ) {
         msg << MSG::ERROR << "Couldn't retrieve object \"" << key
             << "\" as SG::AuxElement" << endmsg;
      }
      return nullptr;
   }
#endif // XAOD_STANDALONE

   /// This function is used internally in the code when creating primitive
   /// branches. I just took the code from xAODRootAccess, which itself too it
   /// from SFrame... :-P
   ///
   /// @param typeidType The type name coming from typeid(...).name()
   /// @param msg The caller's @c MsgStream object
   /// @return The character describing this type for @c TTree::Branch
   ///
   char rootType( char typeidType, MsgStream& msg ) {

      // Do the hard-coded translation:
      switch( typeidType ) {

         case 'c':
            return 'B';
            break;
         case 'h':
            return 'b';
            break;
         case 's':
            return 'S';
            break;
         case 't':
            return 's';
            break;
         case 'i':
            return 'I';
            break;
         case 'j':
            return 'i';
            break;
         case 'f':
            return 'F';
            break;
         case 'd':
            return 'D';
            break;
         case 'x':
            return 'L';
            break;
         case 'y':
         case 'm': // Not sure how platform-independent this one is...
            return 'l';
            break;
         case 'b':
            return 'O';
            break;
         default:
            // If we didn't find this type:
            msg << MSG::ERROR << "Received an unknown type: " << typeidType
                << endmsg;
            return '\0';
            break;
      }
   }

   /// Check if an aux item exists in the aux store
   ///
   /// @param key The name of the container in the event store
   /// @return True if branch exists, false if not
   ///
   bool auxItemExists( const std::string& key ) {
      // Get a pointer to the vector factory.
      const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

      // Try to find the aux item
      return reg.findAuxID( key ) != SG::null_auxid;
   }
} // private namespace

namespace CP {

   AsgxAODNTupleMakerAlg::AsgxAODNTupleMakerAlg( const std::string& name,
                                                 ISvcLocator* svcLoc )
   : EL::AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties.
      declareProperty( "TreeName", m_treeName = "physics",
                       "Name of the tree to write" );
      declareProperty( "Branches", m_branches,
                       "Branches to write to the output tree" );
   }

   StatusCode AsgxAODNTupleMakerAlg::initialize() {

      // Check that at least one branch is configured.
      if( m_branches.empty() ) {
         ATH_MSG_ERROR( "No branches set up for writing" );
         return StatusCode::FAILURE;
      }

      // Set up the systematics list.
      ATH_CHECK( m_systematicsList.initialize() );

      // Reset the initialisation flag:
      m_isInitialized = false;

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::execute() {

      // Initialise the processor objects on the first event.
      if( ! m_isInitialized ) {
         // Initialise the output tree.
         m_tree = tree( m_treeName );
         if( ! m_tree ) {
            ATH_MSG_ERROR( "Could not find output tree \"" << m_treeName
                           << "\"" );
            return StatusCode::FAILURE;
         }
         // Call the setup function.
         ATH_CHECK( setupTree() );
         // The processor objects are now set up.
         m_isInitialized = true;
      }

      // Process the standalone objects:
      for( auto& element_itr : m_elements ) {
         // Retrieve the object:
         static const bool ALLOW_MISSING = false;
         const SG::AuxElement* el = getElement( element_itr.first,
                                                *( evtStore() ),
                                                ALLOW_MISSING, msg() );
         if( ! el ) {
            ATH_MSG_ERROR( "Failed to retrieve object \"" << element_itr.first
                           << "\"" );
            return StatusCode::FAILURE;
         }
         // Process it:
         ATH_CHECK( element_itr.second.process( *el ) );
      }

      // Process the container objects:
      for( auto& container_itr : m_containers ) {
         // Retrieve the container:
         static const bool ALLOW_MISSING = false;
         const SG::AuxVectorBase* vec = getVector( container_itr.first,
                                                   *( evtStore() ),
                                                   ALLOW_MISSING, msg() );
         if( ! vec ) {
            ATH_MSG_ERROR( "Failed to retrieve container \""
                           << container_itr.first << "\"" );
            return StatusCode::FAILURE;
         }
         // Process it.
         ATH_CHECK( container_itr.second.process( *vec ) );
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::finalize() {

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::setupTree() {

      // First process nominal
      CP::SystematicSet nominal{};
      for( const std::string& branchDecl : m_branches ) {
         ATH_CHECK( setupBranch( branchDecl, nominal ) );
      }

      // Consider all systematics but skip the nominal one
      for( const auto& sys : m_systematicsList.systematicsVector() ) {
         // Nominal already processed
         if( sys.empty() ) {
            continue;
         }

         // Iterate over the branch specifications.
         for( const std::string& branchDecl : m_branches ) {
            ATH_CHECK( setupBranch( branchDecl, sys ) );
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::setupBranch( const std::string &branchDecl,
                                                  const CP::SystematicSet &sys ) {

      // The regular expression used to extract the needed info. The logic
      // is supposed to be:
      //
      // (match[1]).(match[2])<any whitespace>-><any whitespace>(match[3])
      //
      // Like:
      //    "Electrons.eta  -> el_eta"
      //
      // , where we would pick up "Electrons", "eta" and "el_eta" as the
      // three words using this regexp.
      static const std::regex
         re( "\\s*([\\w%]+)\\.([\\w%]+)\\s*->\\s*([\\w%]+)" );

      // Interpret this branch declaration.
      std::smatch match;
      if( ! std::regex_match( branchDecl, match, re ) ) {
         ATH_MSG_ERROR( "Expression \"" << branchDecl
                        << "\" doesn't match \"<object>.<variable> ->"
                        " <branch>\"" );
         return StatusCode::FAILURE;
      }

      // Check if we are running nominal
      bool nominal = sys.empty();

      // Event store key for the object under consideration.
      const std::string key = makeSystematicsName( match[ 1 ], sys );
      // Auxiliary variable name for the object under consideration.
      const std::string auxName = makeSystematicsName( match[ 2 ],
                                                         sys );
      // Branch name for the variable.
      const std::string brName = makeSystematicsName( match[ 3 ],
                                                      sys );

      // If the %SYS% pattern was not used in this setup, then stop
      // on non-nominal systematic.
      if( ! nominal &&
          ( key == match[ 1 ] ) && ( auxName == match[ 2 ] ) &&
          ( brName == match[ 3 ] ) ) {
         return StatusCode::SUCCESS;
      }

      // Check that we use the %SYS% pattern reasonably in the names.
      if( ( ( key == match[ 1 ] ) && ( auxName == match[ 2 ] ) &&
            ( brName != match[ 3 ] ) ) ||
            ( ( ( key != match[ 1 ] ) || ( auxName != match[ 2 ] ) ) &&
            ( brName == match[ 3 ] ) ) ) {
         ATH_MSG_ERROR( "The systematic variation pattern is used "
                        "inconsistently in: \"" << branchDecl
                        << "\"" );
         return StatusCode::FAILURE;
      }

      // Flag keeping track whether any branch was set up for this rule.
      bool branchCreated = false;

      // Decide whether the specified key belongs to a container or
      // a standalone object.
      static const bool ALLOW_MISSING = true;
      if( getVector( key, *( evtStore() ), ALLOW_MISSING,
                     msg() ) ) {
         bool created = false;
         ATH_CHECK( m_containers[ key ].addBranch( *m_tree,
                                                   auxName,
                                                   brName,
                                                   ALLOW_MISSING,
                                                   created ) );
         if( created ) {
            ATH_MSG_DEBUG( "Writing branch \"" << brName
                           << "\" from container/variable \"" << key
                           << "." << auxName << "\"" );
            branchCreated = true;
         } else {
            ATH_MSG_DEBUG( "Skipping branch \"" << brName
                           << "\" from container/variable \"" << key
                           << "." << auxName << "\"" );
         }
      } else if( getElement( key, *( evtStore() ),
                              ALLOW_MISSING, msg() ) ) {
         bool created = false;
         ATH_CHECK( m_elements[ key ].addBranch( *m_tree,
                                                   auxName,
                                                   brName,
                                                   ALLOW_MISSING,
                                                   created ) );
         if( created ) {
            ATH_MSG_DEBUG( "Writing branch \"" << brName
                           << "\" from object/variable \"" << key
                           << "." << auxName << "\"" );
            branchCreated = true;
         } else {
            ATH_MSG_DEBUG( "Skipping branch \"" << brName
                        << "\" from object/variable \"" << key
                        << "." << auxName << "\"" );
         }
      } else {
         ATH_MSG_DEBUG( "Container \"" << key
                        << "\" not readable for expression: \""
                        << branchDecl << "\"" );
      }

      // Check if the rule was meaningful or not:
      if( nominal && ! branchCreated && key == match[ 1 ] ) {
         ATH_MSG_ERROR( "No branch was created for rule: \""
                        << branchDecl << "\"" );
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   AsgxAODNTupleMakerAlg::ElementProcessor::ElementProcessor()
   : asg::AsgMessaging( "CP::AsgxAODNTupleMakerAlg::ElementProcessor" ) {

   }

   StatusCode AsgxAODNTupleMakerAlg::ElementProcessor::
   process( const SG::AuxElement& element ) {

      // Process all branches.
      for( BranchProcessor& p : m_branches ) {
         ATH_CHECK( p.process( element, msg() ) );
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::ElementProcessor::
   addBranch( TTree& tree, const std::string& auxName,
              const std::string& branchName,
              bool allowMissing,
              bool &created ) {

      /// Helper class for finding an already existing branch processor.
      class BranchFinder {
      public:
         /// Type of the predicate's argument
         typedef const BranchProcessor& argument_type;
         /// Constructor with key/name
         BranchFinder( const std::string& branchName ) : m_name( branchName ) {}
         /// Operator evaluating whether this is the branch we're looking for
         bool operator()( argument_type bp ) const {
            return ( bp.m_branchName == m_name );
         }
      private:
         std::string m_name; ///< Name of the branch
      }; // class BranchFinder

      // Check if the corresponding aux item exists
      bool validAuxItem = auxItemExists( auxName );
      if( ! validAuxItem ) {
         if( allowMissing ) {
            // Return gracefully.
            ATH_MSG_DEBUG( "Aux item \"" << auxName
                           << "\" not readable for branch \""
                           << branchName << "\"" );
            return StatusCode::SUCCESS;
         } else {
            // Return gracefully.
            ATH_MSG_ERROR( "Aux item \"" << auxName
                           << "\" not readable for branch \""
                           << branchName << "\"" );
            return StatusCode::FAILURE;
         }
      }

      // Check whether this branch is already set up:
      auto itr = std::find_if( m_branches.begin(), m_branches.end(),
                               BranchFinder( branchName ) );
      if( itr != m_branches.end() ) {
         ATH_MSG_WARNING( "Duplicate setup received for branch: " << branchName );
         return StatusCode::SUCCESS;
      }

      created = true;

      // Set up the new branch.
      m_branches.emplace_back();
      ATH_CHECK( m_branches.back().setup( tree, auxName, branchName, msg() ) );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode
   AsgxAODNTupleMakerAlg::ElementProcessor::BranchProcessor::
   setup( TTree& tree, const std::string& auxName,
          const std::string& branchName, MsgStream& msg ) {

      // Remember the branch name.
      m_branchName = branchName;

      // Create the accessor.
      m_acc.reset( new SG::AuxElement::TypelessConstAccessor( auxName ) );

      // Get a pointer to the vector factory.
      const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
      const std::type_info* ti = reg.getType( m_acc->auxid() );
      if( ! ti ) {
         msg << MSG::ERROR
             << "No std::type_info available for auxiliary variable: "
             << auxName << endmsg;
         return StatusCode::FAILURE;
      }
      m_factory = reg.getFactory( *ti );
      if( ! m_factory ) {
         msg << MSG::ERROR << "No factory found for auxiliary variable: "
             << auxName << endmsg;
         return StatusCode::FAILURE;
      }

      // Create the data object.
      m_data = m_factory->create( 1, 1 );

      // Pointer to the branch, to be created.
      TBranch* br = nullptr;

      // Decide whether we're dealing with a "primitive" or an "object" branch.
      if( strlen( ti->name() ) == 1 ) {

         // This is a "primitive" variable...

         // Get the type identifier for it that ROOT will understand.
         const char rType = rootType( ti->name()[ 0 ], msg );
         if( rType == '\0' ) {
            msg << MSG::ERROR << "Type not recognised for variable: "
                << branchName << endmsg;
            return StatusCode::FAILURE;
         }

         // Construct the type description.
         std::ostringstream typeDesc;
         typeDesc << branchName << "/" << rType;

         // Create the primitive branch.
         br = tree.Branch( branchName.c_str(), m_data->toPtr(),
                           typeDesc.str().c_str() );

      } else {

         // This is an "object" variable...

         // Get a proper type name for the variable.
         const std::string typeName = SG::normalizedTypeinfoName( *ti );

         // Access the dictionary for the type.
         TClass* cl = TClass::GetClass( *ti );
         if( ! cl ) {
            cl = TClass::GetClass( typeName.c_str() );
         }
         if( ! cl ) {
            msg << MSG::ERROR << "Couldn't find dictionary for type: "
                << typeName << endmsg;
            return StatusCode::FAILURE;
         }
         if( ! cl->GetStreamerInfo() ) {
            msg << MSG::ERROR << "No streamer info available for type: "
                << cl->GetName() << endmsg;
            return StatusCode::FAILURE;
         }

         // Create the object branch.
         m_dataPtr = m_data->toPtr();
         br = tree.Branch( branchName.c_str(), cl->GetName(), &m_dataPtr );

      }

      // Check that the branch creation succeeded.
      if( ! br ) {
         msg << MSG::ERROR << "Failed to create branch: " << branchName
             << endmsg;
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode
   AsgxAODNTupleMakerAlg::ElementProcessor::BranchProcessor::
   process( const SG::AuxElement& element, MsgStream& msg ) {

      // A security check.
      if( ( ! m_acc ) || ( ! m_factory ) || ( ! m_data ) ) {
         msg << MSG::FATAL << "Internal logic error detected" << endmsg;
         return StatusCode::FAILURE;
      }

      // Get the data out of the xAOD object.
      const void* auxData = ( *m_acc )( element );

      // Copy it into the output variable.
      m_factory->copy( m_data->toPtr(), 0, auxData, 0 );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   AsgxAODNTupleMakerAlg::ContainerProcessor::ContainerProcessor()
   : asg::AsgMessaging( "CP::AsgxAODNTupleMakerAlg::ContainerProcessor" ) {

   }

   StatusCode AsgxAODNTupleMakerAlg::ContainerProcessor::
   process( const SG::AuxVectorBase& container ) {

      // Get the collection proxy for the type if it's not available yet.
      if( ! m_collProxy ) {

         // Try to get the dictionary for the type.
         TClass* cl = TClass::GetClass( typeid( container ) );
         if( ! cl ) {
            ATH_MSG_ERROR( "No dictionary found for container" );
            return StatusCode::FAILURE;
         }

         // Get the collection proxy from the dictionary.
         m_collProxy = cl->GetCollectionProxy();
         if( ! m_collProxy ) {
            ATH_MSG_ERROR( "No collection proxy provided by type: "
                           << cl->GetName() );
            return StatusCode::FAILURE;
         }

         // Get the offset that one needs to use to get from the element
         // pointers to SG::AuxElement pointers.
         static const TClass* auxElementClass =
            TClass::GetClass( typeid( SG::AuxElement ) );
         m_auxElementOffset =
            m_collProxy->GetValueClass()->GetBaseClassOffset( auxElementClass );
         if( m_auxElementOffset < 0 ) {
            ATH_MSG_ERROR( "Vector element type \""
                           << m_collProxy->GetValueClass()->GetName()
                           << "\" doesn't seem to inherit from \""
                           << auxElementClass->GetName() << "\"" );
            return StatusCode::FAILURE;
         }
      }

      // Set up the iteration over the elements of the container. In a really
      // low level / ugly way...
      void* cPtr =
         const_cast< void* >( static_cast< const void* >( &container ) );
      TVirtualCollectionProxy::TPushPop helper( m_collProxy, cPtr );
      const UInt_t cSize = m_collProxy->Size();

      // Tell all branch processors to resize their variables.
      for( BranchProcessor& p : m_branches ) {
         ATH_CHECK( p.resize( cSize, msg() ) );
      }

      // Now iterate over the container.
      for( UInt_t i = 0; i < cSize; ++i ) {

         // Get the element.
         char* elPtr = static_cast< char* >( m_collProxy->At( i ) );
         if( ! elPtr ) {
            ATH_MSG_ERROR( "Failed to get element " << i << " from container" );
            return StatusCode::FAILURE;
         }
         const SG::AuxElement* element =
            reinterpret_cast< const SG::AuxElement* >( elPtr +
                                                       m_auxElementOffset );

         // Execute all branch processors on this element.
         for( BranchProcessor& p : m_branches ) {
            ATH_CHECK( p.process( *element, i, msg() ) );
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::ContainerProcessor::
   addBranch( TTree& tree, const std::string& auxName,
              const std::string& branchName,
              bool allowMissing,
              bool &created ) {

      /// Helper class for finding an already existing branch processor.
      class BranchFinder {
      public:
         /// Type of the predicate's argument
         typedef const BranchProcessor& argument_type;
         /// Constructor with key/name
         BranchFinder( const std::string& branchName ) : m_name( branchName ) {}
         /// Operator evaluating whether this is the branch we're looking for
         bool operator()( argument_type bp ) const {
            return ( bp.m_branchName == m_name );
         }
      private:
         std::string m_name; ///< Name of the branch
      }; // class BranchFinder

      // Check if the corresponding aux item exists
      bool validAuxItem = auxItemExists( auxName );
      if( ! validAuxItem ) {
         if( allowMissing ) {
            // Return gracefully.
            ATH_MSG_DEBUG( "Aux item \"" << auxName
                           << "\" not readable for branch \""
                           << branchName << "\"" );
            return StatusCode::SUCCESS;
         } else {
            // Return gracefully.
            ATH_MSG_ERROR( "Aux item \"" << auxName
                           << "\" not readable for branch \""
                           << branchName << "\"" );
            return StatusCode::FAILURE;
         }
      }

      // Check whether this branch is already set up:
      auto itr = std::find_if( m_branches.begin(), m_branches.end(),
                               BranchFinder( branchName ) );
      if( itr != m_branches.end() ) {
         ATH_MSG_WARNING( "Duplicate setup received for branch: " << branchName );
         return StatusCode::SUCCESS;
      }

      created = true;

      // Set up the new branch.
      m_branches.emplace_back();
      ATH_CHECK( m_branches.back().setup( tree, auxName, branchName, msg() ) );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::ContainerProcessor::BranchProcessor::
   setup( TTree& tree, const std::string& auxName,
          const std::string& branchName, MsgStream& msg ) {

      // Remember the branch name.
      m_branchName = branchName;

      // Create the accessor.
      m_acc.reset( new SG::AuxElement::TypelessConstAccessor( auxName ) );

      // Get a pointer to the vector factory.
      const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
      const std::type_info* ti    = reg.getType( m_acc->auxid() );
      const std::type_info* vecTi = reg.getVecType( m_acc->auxid() );
      if( ( ! ti ) || ( ! vecTi ) ) {
         msg << MSG::ERROR
             << "No std::type_info available for auxiliary variable: "
             << auxName << endmsg;
         return StatusCode::FAILURE;
      }
      m_factory = reg.getFactory( *ti );
      if( ! m_factory ) {
         msg << MSG::ERROR << "No factory found for auxiliary variable: "
             << auxName << endmsg;
         return StatusCode::FAILURE;
      }

      // Create the data object.
      m_data = m_factory->create( 0, 0 );

      // Get a proper type name for the variable.
      const std::string typeName = SG::normalizedTypeinfoName( *vecTi );

      // Access the dictionary for the type.
      TClass* cl = TClass::GetClass( *vecTi );
      if( ! cl ) {
         cl = TClass::GetClass( typeName.c_str() );
      }
      if( ! cl ) {
         msg << MSG::ERROR << "Couldn't find dictionary for type: "
             << typeName << endmsg;
         return StatusCode::FAILURE;
      }
      if( ! cl->GetStreamerInfo() ) {
         msg << MSG::ERROR << "No streamer info available for type: "
             << cl->GetName() << endmsg;
         return StatusCode::FAILURE;
      }

      // Create the branch.
      m_dataPtr = m_data->toVector();
      TBranch* br = tree.Branch( branchName.c_str(), cl->GetName(),
                                 &m_dataPtr );
      if( ! br ) {
         msg << MSG::ERROR << "Failed to create branch: " << branchName
             << endmsg;
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::ContainerProcessor::BranchProcessor::
   resize( size_t size, MsgStream& msg ) {

      // A security check.
      if( ! m_data ) {
         msg << MSG::FATAL << "Internal logic error detected" << endmsg;
         return StatusCode::FAILURE;
      }

      // Do the deed.
      m_data->resize( 0 );
      m_data->resize( size );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode AsgxAODNTupleMakerAlg::ContainerProcessor::BranchProcessor::
   process( const SG::AuxElement& element, size_t index, MsgStream& msg ) {

      // A security check.
      if( ( ! m_acc ) || ( ! m_factory ) || ( ! m_data ) ) {
         msg << MSG::FATAL << "Internal logic error detected" << endmsg;
         return StatusCode::FAILURE;
      }

      // Get the data out of the xAOD object.
      const void* auxData = ( *m_acc )( element );

      // Copy it into the output variable.
      m_factory->copy( m_data->toPtr(), index, auxData, 0 );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace CP
