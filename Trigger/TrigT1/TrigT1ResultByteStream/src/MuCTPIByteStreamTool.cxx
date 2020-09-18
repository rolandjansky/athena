/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1ResultByteStream/MuCTPIByteStreamTool.h"
#include "TrigT1Result/MuCTPI_RDO.h"


// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IMuCTPIByteStreamTool( "MuCTPIByteStreamTool", 1, 1 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID & MuCTPIByteStreamTool::interfaceID() {
  return IID_IMuCTPIByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
MuCTPIByteStreamTool::MuCTPIByteStreamTool( const std::string& type, const std::string& name,
                                            const IInterface* parent )
    : AthAlgTool( type, name, parent ), m_srcIdMap( 0 ) {

  declareInterface<MuCTPIByteStreamTool>( this );
}

/**
 * The destructor doesn't do anything.
 */
MuCTPIByteStreamTool::~MuCTPIByteStreamTool() {

}

/**
 * The function creates a MuCTPISrcIdMap object that is used in the conversion
 * and initialises the base class.
 */
StatusCode MuCTPIByteStreamTool::initialize() {

  m_srcIdMap = new MuCTPISrcIdMap();
  return AlgTool::initialize();

}

/**
 * The function deletes the MuCTPISrcIdMap object and finalises the base class.
 */
StatusCode MuCTPIByteStreamTool::finalize() {

  delete m_srcIdMap;
  return AlgTool::finalize();

}

/**
 * Conversion from RDO to eformat::ROBFragment.
 * This is called from the MuCTPIByteStreamCnv::createRep method.
 */
StatusCode MuCTPIByteStreamTool::convert( const MuCTPI_RDO* result, RawEventWrite* re ) {

  ATH_MSG_DEBUG("executing convert() from RDO to ROBFragment");

  // Clear Event Assembler
  m_fea.clear();

  // MIROD
  FullEventAssembler< MuCTPISrcIdMap >::RODDATA* theROD;

  // Source ID of MIROD
  const uint32_t rodId = m_srcIdMap->getRodID();

  // get the ROD data container to be filled
  theROD = m_fea.getRodData( rodId );

  ATH_MSG_VERBOSE(" Dumping MuCTPI words:");

  // fill Candidate Multiplicity
  const std::vector< uint32_t >& multiWord = result->getAllCandidateMultiplicities();
  std::vector< uint32_t >::const_iterator it   = multiWord.begin();
  std::vector< uint32_t >::const_iterator it_e = multiWord.end();
  for( ; it != it_e; ++it ) {
    theROD->push_back( *it );
    ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill( '0' )
        << std::setw( 8 )  << ( *it ) << " (candidate multiplicity)");
  }
  
  // fill Data Words
  const std::vector< uint32_t >& dataWord = result->dataWord();
  it   = dataWord.begin();
  it_e = dataWord.end();
  for( ; it != it_e; ++it ) {
    theROD->push_back( *it );
    ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill( '0' )
        << std::setw( 8 )  << ( *it ) << " (candidate word)");
  }

  // Now fill full event
  ATH_MSG_DEBUG("Now filling the event with the MuCTPI fragment");
  m_fea.fill( re, msg() );

  return StatusCode::SUCCESS;
}

/**
 * Conversion from eformat::ROBFragment to RDO.
 * This is called from the MuCTPIByteStreamCnv::createObj method.
 */
StatusCode MuCTPIByteStreamTool::convert( const ROBF* rob, MuCTPI_RDO*& result ) {

  ATH_MSG_DEBUG("executing convert() from ROBFragment to RDO");

  // Source ID of MIROD
  const uint32_t miRodId = m_srcIdMap->getRodID();

  // check ROD source ID
  const uint32_t rodId = rob->rod_source_id();

  // check BC ID
  const uint32_t bcId = rob->rod_bc_id();

  ATH_MSG_DEBUG(" expected ROD sub-detector ID: " << std::hex << miRodId 
      << " ID found: " << std::hex << rodId << std::dec);  

  if( rodId == miRodId || rodId == 0x7501 ) {

    ATH_MSG_VERBOSE(" ROD Header BCID " << bcId << ", dumping MuCTPI words:");
    if( rodId == 0x7501 ) {
      ATH_MSG_DEBUG(" Deprecated ROD source id found: " 
          << std::hex << rodId << std::dec);
    }

    // For generality let's declare the data pointer like this. Altough it's
    // unlikely to ever change from being a pointer to uint32_t-s.
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data;
    rob->rod_data( it_data );
    const uint32_t ndata = rob->rod_ndata();
    ATH_MSG_VERBOSE(" number of data words: " << ndata);

    // candidate multiplicity
    std::vector< uint32_t > candidateMultiplicity;
    // data words
    std::vector< uint32_t > dataWord;
    for( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      if( *it_data >> MuCTPI_RDO::MULT_WORD_FLAG_SHIFT ) {
        candidateMultiplicity.push_back( static_cast< uint32_t >( *it_data ) );
        ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
            << ( *it_data ) << " (candidate multiplicity)");
      } else {
        dataWord.push_back( static_cast< uint32_t >( *it_data ) );
        ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setw( 8 ) << std::setfill( '0' )
            << ( *it_data ) << " (candidate word)");
      }
    }

    // create MuCTPI RDO
    result = new MuCTPI_RDO( std::move(candidateMultiplicity), std::move(dataWord) );
    return StatusCode::SUCCESS;

  }

  ATH_MSG_ERROR("Wrong ROD ID found in the MuCTPI ROB fragment!");
  return StatusCode::FAILURE;
}
