/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1MuctpiTestReadResults.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <iomanip>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// TrigT1 include(s):
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"

// Local include(s):
#include "L1MuctpiTestReadResults.h"

namespace LVL1MUCTPI {

  //--------------
  // Constructor
  //--------------
   L1MuctpiTestReadResults::L1MuctpiTestReadResults( const std::string& name,
                                                     ISvcLocator* pSvcLocator )
      : AthAlgorithm( name, pSvcLocator ) {

   }


   //----------------------------------------------
   // execute() method called once per event
   //----------------------------------------------
   StatusCode L1MuctpiTestReadResults::execute() {

      ATH_MSG_DEBUG( "============================================" );
      ATH_MSG_DEBUG( "Executing L1MuctpiTestReadResults algorithm." );
      ATH_MSG_DEBUG( "============================================" );

      // Get the CTP object from storegate and print
      const LVL1::MuCTPICTP* dHandleCTPresult = 0;
      CHECK( evtStore()->retrieve( dHandleCTPresult ) );
      ATH_MSG_DEBUG( "got CTP result Ptr from SG" );

      ATH_MSG_INFO( "The CTP word is: " << std::ios::hex
                    << dHandleCTPresult->muCTPIWord() );

      // Get the RoIB object from storegate and print
      const L1MUINT::MuCTPIToRoIBSLink* dHandleRoIBresult = 0;
      CHECK( evtStore()->retrieve( dHandleRoIBresult ) );
      ATH_MSG_DEBUG( "got RoIB result Ptr from SG" );

      std::vector< unsigned int >::const_iterator roibRes_itr =
         dHandleRoIBresult->getMuCTPIToRoIBWords().begin();
      std::vector< unsigned int >::const_iterator roibRes_end =
         dHandleRoIBresult->getMuCTPIToRoIBWords().end();
      for( ; roibRes_itr != roibRes_end; ++roibRes_itr ) {

         ATH_MSG_INFO( "The RoIB result: " << std::ios::hex
                       << ( *roibRes_itr ) );

      }

      return StatusCode::SUCCESS;
   }

} // namespace LVL1MUCTPI
