//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "EventFormatPrinterAlg.h"

// EDM include(s).
#include "xAODEventFormat/EventFormat.h"

#include "StoreGate/ReadHandle.h"

// Declare the type to Gaudi.
DECLARE_COMPONENT( xAODMakerTest::EventFormatPrinterAlg )

namespace xAODMakerTest {

   StatusCode EventFormatPrinterAlg::initialize() {

      // Initialise the data handle(s).
      ATH_CHECK( m_metaDataStore.retrieve() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode EventFormatPrinterAlg::execute() {
      // Read and print the event format object.
      const xAOD::EventFormat * ef =
          m_metaDataStore->tryConstRetrieve< xAOD::EventFormat >(m_eventFormatKey);
      if (ef) ATH_MSG_INFO( "Event format:\n" << *ef );
      else return StatusCode::FAILURE;

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODMakerTest
