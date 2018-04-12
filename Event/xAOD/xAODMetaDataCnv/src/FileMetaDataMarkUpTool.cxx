/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataMarkUpTool.cxx 685407 2015-07-24 16:15:06Z cranshaw $

// System include(s):
#include <algorithm>
#include <sstream>

// Athena metadata EDM:
#include "EventInfo/EventStreamInfo.h"

// Local include(s):
#include "FileMetaDataMarkUpTool.h"

namespace xAODMaker {

/// Helper macro to execute the setter functions correctly
#define CHECK_BOOL( EXP )                                             \
      do {                                                            \
         try {                                                        \
            if( ! EXP ) {                                             \
               ATH_MSG_ERROR( "Failed to execute: " << #EXP );        \
               return StatusCode::FAILURE;                            \
            }                                                         \
         } catch( const std::exception& ex ) {                        \
            ATH_MSG_INFO( "Couldn't extract attribute with command: " \
                          << #EXP );                                  \
         }                                                            \
      } while( 0 )

   FileMetaDataMarkUpTool::FileMetaDataMarkUpTool( const std::string& name )
      : asg::AsgMetadataTool( name ) {

      declareProperty( "Key", m_outputKey = "" );
   }

   StatusCode FileMetaDataMarkUpTool::initialize() {
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::postInitialize() {
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::preExecute() {
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::postExecute() {
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::preFinalize() {
      ATH_MSG_DEBUG("in preFinalize()");
      std::vector<std::string> fmd_keys;
      outputMetaStore()->keys<xAOD::FileMetaData>( fmd_keys );
      if (fmd_keys.size() != 1) {
         return(StatusCode::SUCCESS);
      }
      if (!outputMetaStore()->transientContains<xAOD::FileMetaData>( *(fmd_keys.begin()) )) {
         return(StatusCode::SUCCESS);
      }
      DataHandle<xAOD::FileMetaData> fmd;
      StatusCode sc = outputMetaStore()->retrieve(fmd, *(fmd_keys.begin()) );
      if (!sc.isSuccess()) {
         return(StatusCode::SUCCESS);
      }
      const DataHandle<EventStreamInfo> esi;
      sc = outputMetaStore()->retrieve(esi, m_outputKey);
      if (sc.isSuccess()) {
         ATH_MSG_DEBUG( "Setting xAOD::FileMetaData from output EventStreamInfo object " << m_outputKey );
         if (esi->getProcessingTags().size()==1) {
            CHECK_BOOL( fmd->setValue( xAOD::FileMetaData::dataType,
                                       *(esi->getProcessingTags().begin()) ) );
         } else {
            CHECK_BOOL( fmd->setValue( xAOD::FileMetaData::dataType, m_outputKey ) );
         }
         if (esi->getEventTypes().size()==1) {
            float id = esi->getEventTypes().begin()->mc_channel_number();
            CHECK_BOOL( fmd->setValue( xAOD::FileMetaData::mcProcID, id) );
         }
      }
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::finalize() {
      return(StatusCode::SUCCESS);
   }

   StatusCode FileMetaDataMarkUpTool::start() {
      return StatusCode::SUCCESS;
   }

   StatusCode FileMetaDataMarkUpTool::beginInputFile() {
      return StatusCode::SUCCESS;
   }

   StatusCode FileMetaDataMarkUpTool::metaDataStop() {
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
