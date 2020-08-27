/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

// Local include(s):
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_RDO.h"

/**
 * The constructor only has to give an initial value to MuCTPI_MultiplicityWord_Decoder::m_multiplicityWord stored in the object
 *
 * @param multword  The 32-bit MuCTPI multiplicity word that you want to decode
 */
MuCTPI_MultiplicityWord_Decoder::MuCTPI_MultiplicityWord_Decoder( const uint32_t multword, const TriggerMode_t mode )
  : m_multiplicityWord( multword ), m_triggerMode(mode) {

}

/**
 * The constructor only has to give an initial value to MuCTPI_MultiplicityWord_Decoder::m_multiplicityWord stored in the object
 *
 * @param multword  The 32-bit MuCTPI multiplicity word that you want to decode
 */
MuCTPI_MultiplicityWord_Decoder::MuCTPI_MultiplicityWord_Decoder( const uint32_t multword, const bool inclusiveTriggerMode)
  : m_multiplicityWord( multword ), m_triggerMode(exclusive) {
  if (inclusiveTriggerMode) m_triggerMode = inclusive;
}


/**
 * @return total number of muon candidates
 */
uint16_t MuCTPI_MultiplicityWord_Decoder::getNCandidates() const {

  uint16_t nCand = 0;
  if (m_triggerMode == exclusive){
    for( uint32_t i = 0; i < MuCTPI_RDO::MULT_THRESH_NUM; ++i ) {
      nCand += this->getMultiplicity( i );
    }
  } else {
    nCand = this->getMultiplicity( 0 );
  }
  return nCand;
}

/**
 * <strong>Be careful, that candidateNumber is defined from 0 to 5, not from 1 to 6!</strong>
 *
 * @param candidateNumber the index of the threshold for which to get the number of candidates
 * @return                3-bit candidate multiplicity for the specified threshold
 */
uint16_t MuCTPI_MultiplicityWord_Decoder::getMultiplicity( const uint16_t candidateNumber ) const {

  if( candidateNumber >= MuCTPI_RDO::MULT_THRESH_NUM ) {
    // throw exception? issue warning?

    throw GaudiException( "MuCTPI_MultiplicityWord_Decoder::getMultiplicity> Threshold out of range",
			  "MuCTPI_MultiplicityWord_Decoder::getMultiplicity", StatusCode::FAILURE );
    return 99;
  }

  return ((m_multiplicityWord >> (candidateNumber * MuCTPI_RDO::MULT_BITS)) &
	  MuCTPI_RDO::MULT_VAL);
}

/**
 * @return 3-bit BCID fragment in the multiplicity word
 */
uint16_t MuCTPI_MultiplicityWord_Decoder::getBCID() const {

  return ( ( m_multiplicityWord >> ((MuCTPI_RDO::MULT_BCID_POS-1) * MuCTPI_RDO::MULT_BITS)) &
             MuCTPI_RDO::MULT_VAL );
}

/**
 * This function is not responsible for actually printing the information, it just creates
 * a message stream to print the information to with MuCTPI_MultiplicityWord_Decoder::dumpData(MsgStream&) const.
 */
void MuCTPI_MultiplicityWord_Decoder::dumpData() const {

  IMessageSvc* msgSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );
  if( sc.isFailure() ) {
    return;
  }
  MsgStream log( msgSvc, "MuCTPI_MultiplicityWord_Decoder" );
  dumpData( log );

  return;
}

/**
 * Function printing the decodable information of the MuCTPI multiplicity word in an easy-to-read fashion.
 *
 * @param log  A message stream to print the information to
 */
void MuCTPI_MultiplicityWord_Decoder::dumpData( MsgStream& log ) const {

  log << MSG::DEBUG << "=================================================" << endmsg;
  log << MSG::DEBUG << "Candidate Multiplicity word 0x" << MSG::hex << m_multiplicityWord << MSG::dec << endmsg;
  for( uint16_t i = 0; i < MuCTPI_RDO::MULT_THRESH_NUM; ++i ) {
    log << MSG::DEBUG << "Pt threshold " << ( i + 1 ) << ", multiplicity " << getMultiplicity( i ) << endmsg;
  }
  log << MSG::DEBUG << "Number of candidates: " << getNCandidates() << endmsg;
  log << MSG::DEBUG << "MICTP BCID: 0x" << MSG::hex
      << getBCID() << MSG::dec << " ( " << getBCID() << " )" << endmsg;
  log << MSG::DEBUG << "=================================================" << endmsg;

  return;
}
