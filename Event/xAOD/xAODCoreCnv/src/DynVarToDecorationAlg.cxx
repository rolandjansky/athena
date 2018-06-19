// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#define private public
#include "AthContainers/AuxStoreInternal.h"
#undef private
#include "xAODCore/AuxInfoBase.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "DynVarToDecorationAlg.h"

namespace {

   /// Operator for printing the configured variable names nicely
   MsgStream& operator<< ( MsgStream& out,
                           const std::vector< std::string >& vec ) {
      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << "\"" << vec[ i ] << "\"";
         if( i + 1 < vec.size() ) {
            out << ", ";
         }
      }
      out << "]";
      return out;
   }

} // private namespace

namespace xAODMaker {

   DynVarToDecorationAlg::DynVarToDecorationAlg( const std::string& name,
                                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties:
      declareProperty( "ContainerName", m_containerName,
                       "Name of the container to remove auxiliary variables "
                       "from" );
      declareProperty( "AuxVariableNames", m_auxVariableNames,
                       "The list of auxiliary variables to remove from the "
                       "container" );
   }

   StatusCode DynVarToDecorationAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Removing the following variables from \""
                    << m_containerName << "\":" );
      ATH_MSG_INFO( "   " << m_auxVariableNames );

      // Check that some configuration was provided:
      if( ( m_containerName == "" ) || ( ! m_auxVariableNames.size() ) ) {
         ATH_MSG_FATAL( "Invalid configuration provided" );
         return StatusCode::FAILURE;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode DynVarToDecorationAlg::execute() {

      // Unfortunately we can't just retrieve an SG::IAuxStoreHolder pointer
      // from StoreGate, so let's use more concrete types instead:
      if( evtStore()->contains< xAOD::AuxContainerBase >( m_containerName ) ) {
         ATH_MSG_DEBUG( "Updating variables in \"xAOD::AuxContainerBase/"
                        << m_containerName << "\"" );
         const xAOD::AuxContainerBase* c = nullptr;
         ATH_CHECK( evtStore()->retrieve( c, m_containerName ) );
         ATH_CHECK( updateVars( c->getStore() ) );
      } else if( evtStore()->contains< xAOD::AuxInfoBase >( m_containerName ) ) {
         ATH_MSG_DEBUG( "Updating variables in \"xAOD::AuxInfoBase/"
                        << m_containerName << "\"" );
         const xAOD::AuxInfoBase* c = nullptr;
         ATH_CHECK( evtStore()->retrieve( c, m_containerName ) );
         ATH_CHECK( updateVars( c->getStore() ) );
      } else {
         ATH_MSG_FATAL( "Unknown type of container found for \""
                        << m_containerName << "\"" );
         return StatusCode::FAILURE;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode DynVarToDecorationAlg::updateVars( SG::IAuxStore* store ) {

      // Check that the store is what we expect:
      if( ! store ) {
         ATH_MSG_ERROR( "No dynamic auxiliary store found in container \""
                        << m_containerName << "\"" );
         return StatusCode::FAILURE;
      }
      SG::AuxStoreInternal* storeInternal =
            dynamic_cast< SG::AuxStoreInternal* >( store );
      if( ! storeInternal ) {
         ATH_MSG_ERROR( "The dynamic auxiliary store of \"" << m_containerName
                        << "\" does not inherit from SG::AuxStoreInternal" );
         return StatusCode::FAILURE;
      }

      // If the auxiliary IDs were not deduced yet, do that now. We have to
      // delay this as much as possible, since the container has to be read in
      // before the type registry would know about these variable names.
      if( ! m_auxIDs.size() ) {
         for( const std::string& name : m_auxVariableNames ) {
            SG::auxid_t auxid =
                  SG::AuxTypeRegistry::instance().findAuxID( name );
            if( auxid == SG::null_auxid ) {
               ATH_MSG_FATAL( "Could not find an auxiliary ID for variable: \""
                              << name << "\"" );
               return StatusCode::FAILURE;
            }
            ATH_MSG_VERBOSE( "Using auxid " << auxid << " for variable \""
                             << name << "\"" );
            m_auxIDs.insert( auxid );
         }
      }

      // Now that we have the pointer to it, update it:
      for( SG::auxid_t auxid : m_auxIDs ) {
         // Don't be too bothered by the variable not being in the container...
         if( storeInternal->getAuxIDs().find( auxid ) ==
             storeInternal->getAuxIDs().end() ) {
            ATH_MSG_DEBUG( "Auxiliary ID " << auxid
                           << " not held by container \"" << m_containerName
                           << "\"" );
            continue;
         }
         // Force the store to handle the variable as a decoration:
         ATH_MSG_VERBOSE( "Updating variable (auxid): " << auxid );
         SG::AuxStoreInternal::guard_t guard( storeInternal->m_mutex );
         storeInternal->m_auxids.erase( auxid );
         if( ( storeInternal->m_isDecoration.size() <= auxid ) ||
             ( storeInternal->m_vecs.size() <= auxid ) ) {
            storeInternal->m_isDecoration.resize( auxid + 1 );
            storeInternal->m_vecs.resize( auxid + 1 );
         }
         storeInternal->m_isDecoration[ auxid ] = true;
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
