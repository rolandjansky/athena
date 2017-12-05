/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBuilder.cxx 667020 2015-05-13 20:40:26Z stelzer $

// STL includes:
#include <iomanip>
#include <vector>

// Gaudi/Athena includes:
#include "AthenaKernel/errorcheck.h"

#include "EventInfo/EventID.h"
#include "DataModel/DataVector.h"

// eformat include:
#include "eformat/SourceIdentifier.h"

// TrigT1 includes:
#include "TrigT1Interfaces/TrigT1CTPDefs.h"





// Local includes:
#include "RoIBuilder.h"
#include "RoIBDefs.h"

namespace ROIB {

   //--------------
   // Constructor
   //--------------
   RoIBuilder::RoIBuilder( const std::string& name, ISvcLocator* pSvcLocator )
      : AthAlgorithm( name, pSvcLocator ) {

      // Property setting general behaviour:

   }

   //---------------------------------
   // initialize()
   //---------------------------------
   StatusCode RoIBuilder::initialize() {

      ATH_MSG_INFO( "========================================" );
      ATH_MSG_INFO( "Initialisation for RoIBuilder algorithm." );
      ATH_MSG_INFO( " Version: " << PACKAGE_VERSION );
      ATH_MSG_INFO( "========================================" );

      if ( m_doCalo ) { 
	CHECK( m_caloEMTauLocation.empty() );
	CHECK( m_caloJetEnergyLocation.empty() );
      }
      if ( m_doMuon ) 
	CHECK( m_muctpiSLinkLocation.key().empty() );

      // Print system info
      if( ! m_doCalo ) {
	ATH_MSG_WARNING( "Inputs from LVL1 Calo systems switched off" );
      }
      if( ! m_doMuon ) {
	ATH_MSG_WARNING("Inputs from LVL1 Muon systems switched off" );
      }
      CHECK( m_ctpSLinkLocation.initialize() );
      CHECK( m_muctpiSLinkLocation.initialize() );
      CHECK( m_caloEMTauLocation.initialize() );
      CHECK( m_caloJetEnergyLocation.initialize() );
      CHECK( m_roibRDOLocation.initialize() );

      return StatusCode::SUCCESS;
   }

   //---------------------------------
   // finalize()
   //---------------------------------
   StatusCode RoIBuilder::finalize() {
      ATH_MSG_INFO( "Finalizing " << name()
                    << " - package version " << PACKAGE_VERSION );
      return StatusCode::SUCCESS;
   }

   //----------------------------------------------
   // execute() method called once per event
   //----------------------------------------------
   StatusCode RoIBuilder::execute( ) {

      // Exec message
      ATH_MSG_DEBUG( "============================" );
      ATH_MSG_DEBUG( "Execution of RoIB algorithm." );
      ATH_MSG_DEBUG( "============================" );

      //
      // Get the official event ID:
      //

      auto eventInfoHandle = SG::makeHandle( m_eventInfoKey );
      const EventInfo* thisEvent = eventInfoHandle.cptr();
      // we will not reach execution if the EventInfo object is not present, not worth checking
      const int evtNum = thisEvent->event_ID()->event_number();
      ATH_MSG_VERBOSE( "Event number is: " << evtNum );


      /////////////////////////////////////////////////////////////////////////////
      //                                                                         //
      //                              CTP RoI                                    //
      //                                                                         //
      /////////////////////////////////////////////////////////////////////////////

      // create the header
      Header ctp_rdo_header( eformat::helper::SourceIdentifier( eformat::TDAQ_CTP,
                                                                0 ).code() );

      // create zero data trailer
      Trailer ctp_rdo_trailer( 0 );

      // create data element
      std::vector< unsigned int > ctp_rdo_data;

      // Try to retrieve CTP result from StoreGate:
      bool ctp_simulation_error = false;

      const LVL1CTP::CTPSLink* ctp_slink = SG::makeHandle( m_ctpSLinkLocation ).cptr();
      
      // test for consistency
      if ( ctp_slink->getCTPToRoIBWords().empty() ) {
	ctp_simulation_error = true;
	REPORT_MESSAGE( MSG::WARNING )
	  << "CTP size is zero. No header, trailer, data element";
	//} else if( ctp_slink->getDataElements().size() != LVL1CTP::CTPSLink::wordsPerCTPSLink ) {
      } else if( ctp_slink->getDataElements().size() != ctp_slink->getNumWordsPerCTPSLink() ) {
	ctp_simulation_error = true;
	REPORT_MESSAGE( MSG::WARNING )
	  << "Found CTP size inconsistency: " 
	  << ctp_slink->getDataElements().size() << "/"
	  //<< LVL1CTP::CTPSLink::wordsPerCTPSLink
	  << ctp_slink->getNumWordsPerCTPSLink()
	  << " (found/expected)";
	
	// get the data elements
	if( msgLvl( MSG::DEBUG ) ) {
	  const std::vector< unsigned int > ctp_rdo_data_inc = ctp_slink->getDataElements();
	  for( size_t i(0); i < ctp_rdo_data_inc.size(); ++i ) {
	    ATH_MSG_DEBUG( "broken CTP RoI = " << std::setw( 2 ) << i << ' ' 
			   << MSG::hex << std::setfill('0') << std::setw( 8 )
			   << ctp_rdo_data_inc[i] 
			   << MSG::dec << std::setfill(' ') );
	  }
	}
      } else {
	ATH_MSG_VERBOSE( "Retrieved CTP result from TES with key: "
			 << m_ctpSLinkLocation );
      }
   

      if( ctp_simulation_error ) {

         REPORT_MESSAGE( MSG::WARNING )
            << "Creating empty CTP RDO with error code!";      
         ctp_rdo_trailer.setSimulationError();

      } else {

         // prepare header
         ctp_rdo_header = ctp_slink->getHeader();

         // get the data elements
         ctp_rdo_data = ctp_slink->getDataElements();
         if( msgLvl( MSG::DEBUG ) ) {
            for (size_t i(0); i < ctp_rdo_data.size(); ++i) {
               ATH_MSG_DEBUG( "CTP RoI = " << MSG::dec << std::setw( 2 ) << i 
                              << " 0x" << MSG::hex << std::setfill('0') << std::setw( 8 )
                              << ctp_rdo_data[i]
                              << MSG::dec << std::setfill(' ') );
            }
         }

         // perpare trailer
         ctp_rdo_trailer = ctp_slink->getTrailer();
      }

      // build result
      CTPResult ctp_rdo_result( ctp_slink->getCTPVersionNumber(), ctp_rdo_header, ctp_rdo_trailer, ctp_rdo_data );
      ATH_MSG_VERBOSE( "Dump CTPResult object:\n" + ctp_rdo_result.dump() );

      //
      // Check whether the event was accepted or not:
      //
      bool accept = ctp_slink->getAccept();
      ATH_MSG_DEBUG( "L1 Accept = " << accept );

      /////////////////////////////////////////////////////////////////////////////
      //                                                                         //
      //                              egamma RoI                                 //
      //                                                                         //
      /////////////////////////////////////////////////////////////////////////////

      std::vector< EMTauResult > emtau_rdo_result_vector;


      for( unsigned int slink = 0; slink < numEMTauSlinks; ++slink ) {

         eformat::helper::SourceIdentifier
            emtau_source_id( eformat::TDAQ_CALO_CLUSTER_PROC_ROI, slink );
         Header emtau_rdo_header( emtau_source_id.code(), evtNum );
         std::vector< EMTauRoI > emtau_rdo_data;

         bool emtau_simulation_error = false;
         const DataVector< LVL1CTP::SlinkWord >* emtau_slink = 0;

         // get slink from storegate
         if( m_doCalo ) {
	   auto handle = SG::makeHandle( m_caloEMTauLocation[slink] );
	   emtau_slink  = handle.cptr();	   

	   unsigned int icnt = 0;
	   DataVector< LVL1CTP::SlinkWord >::const_iterator itr =
	     emtau_slink->begin();
	   DataVector< LVL1CTP::SlinkWord >::const_iterator end =
	     emtau_slink->end();
	   for( ; itr != end; ++itr ) {
	     
	     ++icnt;
	     if( ( icnt > ( wordsPerHeader + 1 ) ) &&
		 ( icnt <= ( emtau_slink->size() - wordsPerTrailer - 1 ) ) ) {
	       
	       EMTauRoI emtau_roi( ( *itr )->word() );
	       emtau_rdo_data.push_back( emtau_roi );
	       ATH_MSG_DEBUG( "EmTau RoI  = " << MSG::hex << std::setw( 8 )
			      << emtau_roi.roIWord() );
	       
	     }
	   }
         
         } else {
            emtau_simulation_error = true;
	    ATH_MSG_VERBOSE( "Retrieved EMTau Slink from TES with key: " <<  m_caloEMTauLocation[slink] );
         }



         Trailer emtau_rdo_trailer( 0, 0 );
         if( ! emtau_simulation_error ) {
            emtau_rdo_trailer.setNumDataWords( emtau_rdo_data.size() );
         } else {
            emtau_rdo_trailer.setSimulationError();
         }

         EMTauResult emtau_rdo_result( emtau_rdo_header, emtau_rdo_trailer,
                                       emtau_rdo_data );

         emtau_rdo_result_vector.push_back( emtau_rdo_result );
      }

      /////////////////////////////////////////////////////////////////////////////
      //                                                                         //
      //                            jet/energy RoI                               //
      //                                                                         //
      /////////////////////////////////////////////////////////////////////////////

      std::vector< JetEnergyResult > jetenergy_rdo_result_vector;

      for( unsigned int slink = 0; slink < numJetEnergySlinks; ++slink ) {

         eformat::helper::SourceIdentifier
            jetenergy_source_id( eformat::TDAQ_CALO_JET_PROC_ROI, slink );
         Header jetenergy_rdo_header( jetenergy_source_id.code(), evtNum );
         std::vector< JetEnergyRoI > jetenergy_rdo_data;

         bool jetenergy_simulation_error = false;
         const DataVector< LVL1CTP::SlinkWord >* jetenergy_slink = 0;

         // get slink from storegate
         if( m_doCalo ) {
	   auto handle = SG::makeHandle( m_caloJetEnergyLocation[slink] );
	   jetenergy_slink = handle.cptr();
	   
	   ATH_MSG_VERBOSE( "Retrieved JetEnergy Slink from TES with key: "
			    << m_caloJetEnergyLocation[slink] );

	   unsigned int icnt = 0;
	   DataVector< LVL1CTP::SlinkWord >::const_iterator itr =
	     jetenergy_slink->begin();
	   DataVector< LVL1CTP::SlinkWord >::const_iterator end =
	     jetenergy_slink->end();
	   for( ; itr != end; ++itr ) {
	     
	     ++icnt;
	     if( ( icnt > ( wordsPerHeader + 1 ) ) &&
		 ( icnt <= ( jetenergy_slink->size() - wordsPerTrailer - 1 ) ) ) {
	       
	       JetEnergyRoI jetenergy_roi( ( *itr )->word() );
	       jetenergy_rdo_data.push_back( jetenergy_roi );
	       ATH_MSG_DEBUG( "Jet/Energy RoI    = " << MSG::hex << std::setw( 8 )
			      << jetenergy_roi.roIWord() );
	     }
	   }
         } else {
            jetenergy_simulation_error = true;
         }



         // Now wrap up the jetenergy triggers:
         Trailer jetenergy_rdo_trailer( 0, 0 );
         if( !jetenergy_simulation_error ) {
            jetenergy_rdo_trailer.setNumDataWords( jetenergy_rdo_data.size() );
         } else {
            jetenergy_rdo_trailer.setSimulationError();
         }

         JetEnergyResult jetenergy_rdo_result( jetenergy_rdo_header,
                                               jetenergy_rdo_trailer,
                                               jetenergy_rdo_data );

         jetenergy_rdo_result_vector.push_back( jetenergy_rdo_result );
      }

      /////////////////////////////////////////////////////////////////////////////
      //                                                                         //
      //                               muon RoI                                  //
      //                                                                         //
      /////////////////////////////////////////////////////////////////////////////

      eformat::helper::SourceIdentifier
         muon_source_id( eformat::TDAQ_MUON_CTP_INTERFACE, 0 );
      Header muctpi_rdo_header( muon_source_id.code(), evtNum );
      std::vector< MuCTPIRoI > muctpi_rdo_data;

      bool muctpi_simulation_error = false;
      const L1MUINT::MuCTPIToRoIBSLink* muctpi_slink = 0;

      // get slink from storegate
      if( m_doMuon ) {
	auto handle = SG::makeHandle( m_muctpiSLinkLocation );
	muctpi_slink = handle.cptr();
	ATH_MSG_VERBOSE( "Retrieved MuCTPI result from TES with key: "
			 << m_muctpiSLinkLocation );

	unsigned int icnt = 0;
	std::vector< unsigned int >::const_iterator itr =
	  muctpi_slink->getMuCTPIToRoIBWords().begin();
	std::vector< unsigned int >::const_iterator end =
	  muctpi_slink->getMuCTPIToRoIBWords().end();
	for( ; itr != end; ++itr ) {
	  
	  ++icnt;
	  if( ( icnt > ( wordsPerHeader + 1 ) ) &&
	      ( icnt <= ( muctpi_slink->getMuCTPIToRoIBWords().size() -
			  wordsPerTrailer ) ) ) {
	    
	    MuCTPIRoI muctpi_roi( *itr );
	    muctpi_rdo_data.push_back( muctpi_roi );
	    ATH_MSG_DEBUG( "MuCTPI RoI = " << MSG::hex << std::setw( 8 )
			   << muctpi_roi.roIWord() );
	  }
	}
      } else {
         muctpi_simulation_error = true;
      }



      Trailer muctpi_rdo_trailer( 0, 0 );
      if( ! muctpi_simulation_error ) {
         muctpi_rdo_trailer.setNumDataWords( muctpi_rdo_data.size() );
      } else {
         muctpi_rdo_trailer.setSimulationError();
      }

      MuCTPIResult muctpi_rdo_result( muctpi_rdo_header, muctpi_rdo_trailer,
                                      muctpi_rdo_data );

      //
      // Finally create RoIB RDO object:
      //
      std::unique_ptr<RoIBResult> roib_rdo_result = std::make_unique< RoIBResult>( muctpi_rdo_result, ctp_rdo_result,
										   jetenergy_rdo_result_vector,
										   emtau_rdo_result_vector );
      if( msgLvl( MSG::DEBUG ) ) {
         ATH_MSG_DEBUG( "RoIB Results:" );
         roib_rdo_result->muCTPIResult().dumpData( msg( MSG::DEBUG ) );
         // roib_rdo_result->eMTauResult();
         // roib_rdo_result->jetEnergyResult();
         ATH_MSG_DEBUG( roib_rdo_result->cTPResult().dump() );
      }

      //
      // Put RoIB RDO object into SG:
      //
      auto roibHandle = SG::makeHandle( m_roibRDOLocation );
      roibHandle.record( std::move( roib_rdo_result ) );
      // no owerwite possible now

      //
      // Return happily:
      //
      return StatusCode::SUCCESS;
   }

} // namespace ROIB
