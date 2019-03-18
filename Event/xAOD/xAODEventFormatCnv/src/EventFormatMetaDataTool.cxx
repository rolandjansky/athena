/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatMetaDataTool.cxx 651874 2015-03-05 14:16:19Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventFormatMetaDataTool.h"

namespace xAODMaker {

   /// Name of the incident sent around for metadata writing
   //static const char* METADATASTOP = "MetaDataStop";

   EventFormatMetaDataTool::EventFormatMetaDataTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : GenericMetadataToolNoAux <xAOD::EventFormat>(type,name,parent)
   {
   }

   StatusCode EventFormatMetaDataTool::initialize() {

      // Return gracefully:
      return StatusCode::SUCCESS;
   }


   StatusCode EventFormatMetaDataTool::updateContainer(
                                         xAOD::EventFormat* output,
                                   const xAOD::EventFormat* input ) {

      // Merge the new object into the output one:
      for( auto itr = input->begin(); itr != input->end(); ++itr ) {
         if( ! output->exists( itr->second.hash() ) ) {
            output->add( itr->second );
         }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
