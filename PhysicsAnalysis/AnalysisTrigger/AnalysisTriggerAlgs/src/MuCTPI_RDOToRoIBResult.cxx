/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuCTPI_RDOToRoIBResult.cxx 275499 2010-01-27 18:25:00Z krasznaa $

// STL include(s):
#include <iomanip>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPIResult.h"
#include "TrigT1Result/MuCTPIRoI.h"

// Local include(s):
#include "MuCTPI_RDOToRoIBResult.h"

MuCTPI_RDOToRoIBResult::MuCTPI_RDOToRoIBResult( const std::string& name,
                                                ISvcLocator* pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ) {

   declareProperty( "MuCTPIInputKey", m_muctpiInputKey = "MUCTPI_RDO" );
   declareProperty( "RoIBInputKey", m_roibInputKey = "RoIBResult" );
   declareProperty( "RoIBOutputKey", m_roibOutputKey = "CorrectRoIBResult" );
}

StatusCode MuCTPI_RDOToRoIBResult::execute() {

   ATH_MSG_DEBUG( "Executing the MuCTPI RoI fixer algorithm" );

   //
   // Retrieve the (hopefully) correct MuCTPI_RDO object:
   //
   const MuCTPI_RDO* muctpi_rdo = 0;
   CHECK( evtStore()->retrieve( muctpi_rdo, m_muctpiInputKey ) );
   ATH_MSG_VERBOSE( "Retrieved the MuCTPI_RDO object with key: "
                    << m_muctpiInputKey );

   //
   // Retrieve the (muon-wise) wrong RoIBResult object:
   //
   const ROIB::RoIBResult* roibresult = 0;
   CHECK( evtStore()->retrieve( roibresult, m_roibInputKey ) );
   ATH_MSG_VERBOSE( "Retrieved the old RoIBResult object with key:"
                    << m_roibInputKey );

   // Create the correct RoI vector from the RDO data:
   ATH_MSG_VERBOSE( "Now creating the correct RoIs..." );
   std::vector< ROIB::MuCTPIRoI > roi_vector;

   uint32_t bcid = multiplicityBCID( muctpi_rdo->candidateMultiplicity() );

   //
   // Loop over all "data words" and select the ones that should've been
   // sent ot LVL2:
   //
   std::vector< uint32_t >::const_iterator dword_itr = muctpi_rdo->dataWord().begin();
   std::vector< uint32_t >::const_iterator dword_end = muctpi_rdo->dataWord().end();
   for( ; dword_itr != dword_end; ++dword_itr ) {

      // Select which candidates should've gone to LVL2:
      if( roiBCID( *dword_itr ) != bcid ) continue;
      if( ! roiAccepted( *dword_itr ) ) continue;

      // Save the candidate:
      roi_vector.push_back( ROIB::MuCTPIRoI( toRoIWord( *dword_itr ) ) );
      ATH_MSG_VERBOSE( "  - Processed data word: 0x" << std::hex << std::setw( 8 )
                       << std::setfill( '0' ) << *dword_itr );
   }

   //
   // Create the new MuCTPI result:
   //
   ROIB::MuCTPIResult newResult( ROIB::Header(roibresult->muCTPIResult().header()),
                                 ROIB::Trailer(roibresult->muCTPIResult().trailer()),
                                 std::move(roi_vector) );

   //
   // Create and save the new RoIBResult object:
   //
   ROIB::RoIBResult* new_roibresult = new ROIB::RoIBResult( std::move(newResult),
                                                            ROIB::CTPResult(roibresult->cTPResult()),
                                                            std::vector< ROIB::JetEnergyResult >(roibresult->jetEnergyResult()),
                                                            std::vector< ROIB::EMTauResult >(roibresult->eMTauResult()) );
   CHECK( evtStore()->record( new_roibresult, m_roibOutputKey ) );
   ATH_MSG_VERBOSE( "Saved the fixed RoIBResult object with key: "
                    << m_roibOutputKey );

   return StatusCode::SUCCESS;
}

/**
 * @param multi_word Multiplicity word from the MuCTPI_RDO object
 * @returns The BCID stored in the multiplicity word
 */
uint32_t MuCTPI_RDOToRoIBResult::multiplicityBCID( uint32_t multi_word ) {

   return ( ( multi_word >> 18 ) & 0x7 );
}

/**
 * @param data_word Data word from the MuCTPI_RDO object
 * @returns The RoI word created from the data word
 */
uint32_t MuCTPI_RDOToRoIBResult::toRoIWord( uint32_t data_word ) {

   return ( ( ( data_word & 0x8000000 ) >> 4 ) | ( ( data_word & 0x3fe0000 ) >> 3 ) |
            ( data_word & 0x3fff ) );
}

/**
 * @param data_word Data word from the MuCTPI_RDO object
 * @returns the BCID stored in the data word
 */
uint32_t MuCTPI_RDOToRoIBResult::roiBCID( uint32_t data_word ) {

   return ( ( data_word >> 14 ) & 0x7 );
}

/**
 * @param data_word Data word from the MuCTPI_RDO object
 * @returns <code>true</code> if the candidate was destined to go to LVL2,
 *          <code>false</code> otherwise
 */
bool MuCTPI_RDOToRoIBResult::roiAccepted( uint32_t data_word ) {

   return ( ( data_word >> 26 ) & 0x1 );
}
