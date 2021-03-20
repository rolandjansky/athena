/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterCreator.cxx 596346 2014-05-10 13:47:20Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// Local include(s):
#include "ClusterCreator.h"

namespace {

   /// Helper operator for printing vectors
   template< class T >
   std::ostream& operator<< ( std::ostream& out,
                              const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i < ( vec.size() - 1 ) ) {
            out << ", ";
         }
      }
      out << "]";

      return out;
   }

} // private namespace

ClusterCreator::ClusterCreator( const std::string& name,
                                ISvcLocator* svcLoc )
   : AthAlgorithm( name, svcLoc ),
     m_keySet( false ),
     m_cnvTool( "xAODMaker::CaloClusterCnvTool/CaloClusterCnvTool", this ) {

   declareProperty( "AODContainerNames", m_aodContainerNames );
   declareProperty( "xAODContainerNames", m_xaodContainerNames );
}

StatusCode ClusterCreator::initialize() {

   ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

   // Retrieve the needed tool(s):
   CHECK( m_cnvTool.retrieve() );

   if( m_xaodContainerNames.empty() ) {
      // Assume same key in AOD and xAOD if to explicitly set
      m_xaodContainerNames = m_aodContainerNames;
   }

   // A sanity check:
   if( m_aodContainerNames.size() != m_xaodContainerNames.size() ) {
      REPORT_MESSAGE( MSG::FATAL )
         << "Job properties 'AODContainerNames' and 'xAODContainerNames' "
         << "have unequal lenght! (" 
         << m_aodContainerNames.size() << " vs "
         << m_xaodContainerNames.size() << ")";
      return StatusCode::FAILURE;
   }

   // Tell the user what's going to happen:
   if( ( m_aodContainerNames.empty() ) &&
       ( m_xaodContainerNames.empty() ) ) {
      ATH_MSG_INFO( "Will convert all CaloClusterContainers in the event");
      m_keySet = false;
   }
   else {
      m_keySet = true;
      const size_t nKeys = m_aodContainerNames.size();
      for (size_t i=0;i<nKeys;++i) {
         ATH_MSG_INFO( "Will convert CaloClusterContainer#"
                       << m_aodContainerNames[ i ]
                       << " to xAOD::CaloClusterContainer#"
                       << m_xaodContainerNames[ i ] );
      }
   }

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode ClusterCreator::execute() {

   //
   // If no StoreGate keys were set, convert everything:
   //
   if( ! m_keySet ) {
      // Get the keys from StoreGate:
      evtStore()->keys< CaloClusterContainer >( m_aodContainerNames,
                                                false, false );

      // Use same container key for input and output:
      m_xaodContainerNames = m_aodContainerNames;

      ATH_MSG_DEBUG( "Found the following CaloClusterContainers: "
                     << m_aodContainerNames );  
   }

   //
   // Loop over container keys:
   //
   const size_t nKeys = m_aodContainerNames.size();
   for( size_t iKey = 0; iKey < nKeys; ++iKey ) {

      const std::string& aodName  = m_aodContainerNames[ iKey ];
      const std::string& xaodName = m_xaodContainerNames[ iKey ];

      // Check if anything needs to be done:
      if( ! evtStore()->contains< CaloClusterContainer >( aodName ) ) {
         ATH_MSG_DEBUG( "AOD cluster container " << aodName
                        << " not found. Ignored." );
         continue;
      }
      if( evtStore()->contains< xAOD::CaloClusterContainer >( xaodName ) ) {
         ATH_MSG_DEBUG( "xAOD::CaloCluster with key " << xaodName
                        << " exists already. No conversion done" );
         continue;
      }

      // Retrieve the AOD clusters:
      const CaloClusterContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, aodName ) );
      ATH_MSG_DEBUG( "Retrieved clusters with key: " << aodName );

      // Create new Cell-Link container:
      const std::string ccclc_name = xaodName + "_links";
      CaloClusterCellLinkContainer* ccclc =
         new CaloClusterCellLinkContainer();
      CHECK( evtStore()->record( ccclc, ccclc_name ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::CaloClusterContainer* xaod = new xAOD::CaloClusterContainer();
      CHECK( evtStore()->record( xaod, xaodName ) );
      
      xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer(); 
      CHECK( evtStore()->record( aux, xaodName + "Aux." ) );
      
      xaod->setStore( aux );
      ATH_MSG_DEBUG( "Recorded xAOD clusters with key: " << xaodName );

 
      // Create the xAOD objects:
      CaloClusterContainer::const_iterator itr = aod->begin();
      CaloClusterContainer::const_iterator end = aod->end();
      for(unsigned i = 0 ; itr != end; ++itr, ++i ) {

         ATH_MSG_VERBOSE( "Copying cluster #" << i );

         // Create the xAOD object:
         xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
         xaod->push_back( cluster );

         // Leave the conversion up to the tool:
         CHECK( m_cnvTool->convert( *itr, cluster, ccclc ) );

         ATH_MSG_VERBOSE( "Number of dynamic auxiliary variables: "
                          << aux->getDynamicAuxIDs().size() );

      } // end loop over clusters

   } // end loop over cluster containers

   // Return gracefully:
   return StatusCode::SUCCESS;
}
