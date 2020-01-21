//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// EDM include(s).
#include "xAODMetaData/FileMetaData.h"
#include "xAODEventInfo/EventInfo.h"

// Local include(s).
#include "EventLoopTest/UnitTestAlg4.h"

namespace EL {

   UnitTestAlg4::UnitTestAlg4( const std::string& name, ISvcLocator* svcLoc )
      : AnaAlgorithm( name, svcLoc ) {

   }

   ::StatusCode UnitTestAlg4::initialize() {

      // Greet the user.
      ATH_MSG_INFO( "Initialising algorithm..." );

      // Make sure that @c beginInputFile() gets called.
      ATH_CHECK( requestBeginInputFile() );

      // Read the file metadata.
      ATH_CHECK( readFileMetaData() );

      // Make sure that event data can *not* be accessed yet.
      const xAOD::EventInfo* ei = nullptr;
      if( evtStore()->retrieve( ei, "EventInfo" ).isSuccess() == true ) {
         ATH_MSG_FATAL( "Event information should not be available yet!" );
         return ::StatusCode::FAILURE;
      }

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

   ::StatusCode UnitTestAlg4::execute() {

      // Check that event data access is working by now.
      const xAOD::EventInfo* ei = nullptr;
      ATH_CHECK( evtStore()->retrieve( ei, "EventInfo" ) );

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

   ::StatusCode UnitTestAlg4::finalize() {

      // Check whether @c beginInputFile() was called during the job.
      if( m_callCount == 0 ) {
         ATH_MSG_FATAL( "The job did not call beginInputFile()" );
         return ::StatusCode::FAILURE;
      }

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

   ::StatusCode UnitTestAlg4::beginInputFile() {

      // Increment the internal counter.
      m_callCount++;

      // Read the file metadata.
      ATH_CHECK( readFileMetaData() );

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

   ::StatusCode UnitTestAlg4::readFileMetaData() {

      // Access the file metadata object.
      ATH_MSG_INFO( "Reading the file metadata..." );
      const xAOD::FileMetaData* fmd = nullptr;
      ATH_CHECK( inputMetaStore()->retrieve( fmd, "FileMetaData" ) );

      // Get some information out of the object.
      std::string amiTag, dataType, simFlavour;
      fmd->value( xAOD::FileMetaData::amiTag, amiTag );
      fmd->value( xAOD::FileMetaData::dataType, dataType );
      fmd->value( xAOD::FileMetaData::simFlavour, simFlavour );
      ATH_MSG_INFO( "  amiTag     = " << amiTag );
      ATH_MSG_INFO( "  dataType   = " << dataType );
      ATH_MSG_INFO( "  simFlavour = " << simFlavour );

      // Return gracefully.
      return ::StatusCode::SUCCESS;
   }

} // namespace EL
