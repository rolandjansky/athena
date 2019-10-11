/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv.cxx 773852 2016-09-19 14:08:20Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// Infrastucture include(s):

// EDM include(s):
#include "xAODCaloEvent/versions/CaloClusterAuxContainer_v1.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Local include(s):
#include "xAODCaloClusterAuxContainerCnv.h"
#include "xAODCaloClusterAuxContainerCnv_v1.h"
#include "AthContainers/tools/copyThinned.h"

xAODCaloClusterAuxContainerCnv::
xAODCaloClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloClusterAuxContainerCnvBase( svcLoc )
#ifndef XAOD_ANALYSIS
   , m_compressor( "xAODClusterCompressor","xAODCaloClusterAuxContainerCnv" )
#endif
   , m_doCompression( false )
{

#ifndef XAOD_ANALYSIS
   if( m_compressor.retrieve().isSuccess() ) {
      m_doCompression = true;
      ATH_MSG_INFO("Retrieved compression tool");
   } else {
      m_doCompression = false;
      ATH_MSG_WARNING( "Failed to retrieve compression tool. "
                       "Will store uncompressed cluster" );
   }
#endif

}

xAOD::CaloClusterAuxContainer*
xAODCaloClusterAuxContainerCnv::
createPersistentWithKey( xAOD::CaloClusterAuxContainer* trans,
                         const std::string& key) {

  std::unique_ptr<xAOD::CaloClusterAuxContainer> result
    { xAODCaloClusterAuxContainerCnvBase::createPersistentWithKey (trans, key) };

#ifndef XAOD_ANALYSIS
   // Compress it if possible:
   if( m_doCompression ) {
      xAOD::CaloClusterContainer helper;
      for( size_t i = 0; i < result->size(); ++i ) {
         helper.push_back( new xAOD::CaloCluster() );
      }
      helper.setStore( result.get() );
      m_compressor->compress( &helper );
   }
#endif

   // Return the object to be written:
   return result.release();
}

