/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBTester.cxx 500598 2012-05-14 15:39:58Z krasznaa $

// STL includes:
#include <iomanip>
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// TrigT1 include(s):
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Result/RoIBResult.h"

// Local include(s):
#include "RoIBTester.h"

namespace ROIB {

   //--------------
   // Constructor
   //--------------
   RoIBTester::RoIBTester( const std::string& name, ISvcLocator* pSvcLocator )
      : AthAlgorithm( name, pSvcLocator ) {

      // Properties for data object locations in StoreGate
      declareProperty( "RoIBRDOLocation",
                       m_roibRDOLocation = ROIB::DEFAULT_RoIBRDOLocation,
                       "StoreGate location of RoIB RDO" );
   }

   //---------------------------------
   // initialize()
   //---------------------------------
   StatusCode RoIBTester::initialize() {

      ATH_MSG_INFO( "Initializing " << name() << " algorithm " 
                    << " - package version " << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   //---------------------------------
   // finalize()
   //---------------------------------
   StatusCode RoIBTester::finalize() {

      ATH_MSG_INFO( "Finalizing " << name()
                    << " - package version " << PACKAGE_VERSION );

      return StatusCode::SUCCESS;
   }

   //----------------------------------------------
   // execute() method called once per event
   //----------------------------------------------
   StatusCode RoIBTester::execute() {

      // Return right away if we're not going to print anything anyway:
      if( ! msgLvl( MSG::DEBUG ) ) {
         return StatusCode::SUCCESS;
      }

      // Exec message
      ATH_MSG_DEBUG( "=================================" );
      ATH_MSG_DEBUG( "Execution of RoIB test algorithm." );
      ATH_MSG_DEBUG( "=================================" );

      // Get RoIB RDO object back from SG:
      const RoIBResult* roib_rdo = 0;
      CHECK( evtStore()->retrieve( roib_rdo, m_roibRDOLocation ) );

      //
      // Test RoIB RDO object from store:
      //
      ATH_MSG_DEBUG( "<---- new CTP slink ---->" );
      ATH_MSG_DEBUG( "Header marker : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().headerMarker() );
      ATH_MSG_DEBUG( "Header size   : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().headerSize() );
      ATH_MSG_DEBUG( "Format version: 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().formatVersion() );
      ATH_MSG_DEBUG( "Source ID     : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().sourceID() );
      ATH_MSG_DEBUG( "L1 ID         : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().L1ID() );
      ATH_MSG_DEBUG( "BCID          : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().BCID() );
      ATH_MSG_DEBUG( "Trigger type  : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().triggerType() );
      ATH_MSG_DEBUG( "Event type    : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().header().eventType() );

      std::vector< CTPRoI >::const_iterator ctp_itr =
         roib_rdo->cTPResult().roIVec().begin();
      std::vector< CTPRoI >::const_iterator ctp_end =
         roib_rdo->cTPResult().roIVec().end();
      for( ; ctp_itr != ctp_end; ++ctp_itr ) {
         ATH_MSG_DEBUG( "  --> RoI = 0x" << std::ios::hex << ctp_itr->roIWord() );
      }

      ATH_MSG_DEBUG( "Error status  : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().trailer().errorStatus() );
      ATH_MSG_DEBUG( "Status info   : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().trailer().statusInfo() );
      ATH_MSG_DEBUG( "St. wrd. num. : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().trailer().numStatusWords() );
      ATH_MSG_DEBUG( "Da. wrd. num. : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().trailer().numDataWords() );
      ATH_MSG_DEBUG( "Status pos.   : 0x" << std::ios::hex
                     << roib_rdo->cTPResult().trailer().statusPosition() );

      std::vector< EMTauResult >::const_iterator emtau_itr =
         roib_rdo->eMTauResult().begin();
      std::vector< EMTauResult >::const_iterator emtau_end =
         roib_rdo->eMTauResult().end();
      for( ; emtau_itr != emtau_end; ++emtau_itr ) {

         ATH_MSG_DEBUG( "<---- new egamma slink ---->" );
         ATH_MSG_DEBUG( "Header marker : 0x" << std::ios::hex
                        << emtau_itr->header().headerMarker() );
         ATH_MSG_DEBUG( "Header size   : 0x" << std::ios::hex
                        << emtau_itr->header().headerSize() );
         ATH_MSG_DEBUG( "Format version: 0x" << std::ios::hex
                        << emtau_itr->header().formatVersion() );
         ATH_MSG_DEBUG( "Source ID     : 0x" << std::ios::hex
                        << emtau_itr->header().sourceID() );
         ATH_MSG_DEBUG( "L1 ID         : 0x" << std::ios::hex
                        << emtau_itr->header().L1ID() );
         ATH_MSG_DEBUG( "BCID          : 0x" << std::ios::hex
                        << emtau_itr->header().BCID() );
         ATH_MSG_DEBUG( "Trigger type  : 0x" << std::ios::hex
                        << emtau_itr->header().triggerType() );
         ATH_MSG_DEBUG( "Event type    : 0x" << std::ios::hex
                        << emtau_itr->header().eventType() );

         std::vector< EMTauRoI >::const_iterator roi_itr =
            emtau_itr->roIVec().begin();
         std::vector< EMTauRoI >::const_iterator roi_end =
            emtau_itr->roIVec().end();
         for( ; roi_itr != roi_end; ++roi_itr ) {
            ATH_MSG_DEBUG( "  --> RoI = 0x" << std::ios::hex << roi_itr->roIWord() );
         }

         ATH_MSG_DEBUG( "Error status  : 0x" << std::ios::hex
                        << emtau_itr->trailer().errorStatus() );
         ATH_MSG_DEBUG( "Status info   : 0x" << std::ios::hex
                        << emtau_itr->trailer().statusInfo() );
         ATH_MSG_DEBUG( "St. wrd. num. : 0x" << std::ios::hex
                        << emtau_itr->trailer().numStatusWords() );
         ATH_MSG_DEBUG( "Da. wrd. num. : 0x" << std::ios::hex
                        << emtau_itr->trailer().numDataWords() );
         ATH_MSG_DEBUG( "Status pos.   : 0x" << std::ios::hex
                        << emtau_itr->trailer().statusPosition() );
      }

      std::vector< JetEnergyResult >::const_iterator jete_itr =
         roib_rdo->jetEnergyResult().begin();
      std::vector< JetEnergyResult >::const_iterator jete_end =
         roib_rdo->jetEnergyResult().end();
      for( ; jete_itr != jete_end; ++jete_itr ) {

         ATH_MSG_DEBUG( "<---- new jetenergy slink ---->" );
         ATH_MSG_DEBUG( "Header marker : 0x" << std::ios::hex
                        << jete_itr->header().headerMarker() );
         ATH_MSG_DEBUG( "Header size   : 0x" << std::ios::hex
                        << jete_itr->header().headerSize() );
         ATH_MSG_DEBUG( "Format version: 0x" << std::ios::hex
                        << jete_itr->header().formatVersion() );
         ATH_MSG_DEBUG( "Source ID     : 0x" << std::ios::hex
                        << jete_itr->header().sourceID() );
         ATH_MSG_DEBUG( "L1 ID         : 0x" << std::ios::hex
                        << jete_itr->header().L1ID() );
         ATH_MSG_DEBUG( "BCID          : 0x" << std::ios::hex
                        << jete_itr->header().BCID() );
         ATH_MSG_DEBUG( "Trigger type  : 0x" << std::ios::hex
                        << jete_itr->header().triggerType() );
         ATH_MSG_DEBUG( "Event type    : 0x" << std::ios::hex
                        << jete_itr->header().eventType() );

         std::vector< JetEnergyRoI >::const_iterator roi_itr =
            jete_itr->roIVec().begin();
         std::vector< JetEnergyRoI >::const_iterator roi_end =
            jete_itr->roIVec().end();
         for( ; roi_itr != roi_end; ++roi_itr ) {
            ATH_MSG_DEBUG( "  -> RoI = 0x" << std::ios::hex << roi_itr->roIWord() );
         }

         ATH_MSG_DEBUG( "Error status  : 0x" << std::ios::hex
                        << jete_itr->trailer().errorStatus() );
         ATH_MSG_DEBUG( "Status info   : 0x" << std::ios::hex
                        << jete_itr->trailer().statusInfo() );
         ATH_MSG_DEBUG( "St. wrd. num. : 0x" << std::ios::hex
                        << jete_itr->trailer().numStatusWords() );
         ATH_MSG_DEBUG( "Da. wrd. num. : 0x" << std::ios::hex
                        << jete_itr->trailer().numDataWords() );
         ATH_MSG_DEBUG( "Status pos.   : 0x" << std::ios::hex
                        << jete_itr->trailer().statusPosition() );
      }

      ATH_MSG_DEBUG( "<---- new MuCTPI slink ---->" );
      ATH_MSG_DEBUG( "Header marker : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().headerMarker() );
      ATH_MSG_DEBUG( "Header size   : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().headerSize() );
      ATH_MSG_DEBUG( "Format version: 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().formatVersion() );
      ATH_MSG_DEBUG( "Source ID     : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().sourceID() );
      ATH_MSG_DEBUG( "L1 ID         : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().L1ID() );
      ATH_MSG_DEBUG( "BCID          : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().BCID() );
      ATH_MSG_DEBUG( "Trigger type  : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().triggerType() );
      ATH_MSG_DEBUG( "Event type    : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().header().eventType() );

      std::vector< MuCTPIRoI >::const_iterator muctpi_itr =
         roib_rdo->muCTPIResult().roIVec().begin();
      std::vector< MuCTPIRoI >::const_iterator muctpi_end =
         roib_rdo->muCTPIResult().roIVec().end();
      for( ; muctpi_itr != muctpi_end; ++muctpi_itr ) {
         ATH_MSG_DEBUG( "  --> RoI = 0x" << std::ios::hex << muctpi_itr->roIWord() );
      }

      ATH_MSG_DEBUG( "Error status  : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().trailer().errorStatus() );
      ATH_MSG_DEBUG( "Status info   : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().trailer().statusInfo() );
      ATH_MSG_DEBUG( "St. wrd. num. : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().trailer().numStatusWords() );
      ATH_MSG_DEBUG( "Da. wrd. num. : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().trailer().numDataWords() );
      ATH_MSG_DEBUG( "Status pos.   : 0x" << std::ios::hex
                     << roib_rdo->muCTPIResult().trailer().statusPosition() );

      // Return happily!
      return StatusCode::SUCCESS;
   }

} // namespace ROIB
