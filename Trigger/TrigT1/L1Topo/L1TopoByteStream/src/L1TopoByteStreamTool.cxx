/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <exception>
#include <bitset>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Trigger include(s):
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/Helpers.h"

// Local include(s):
#include "L1TopoByteStreamTool.h"

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IL1TopoByteStreamTool( "L1TopoByteStreamTool", 1, 0 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& L1TopoByteStreamTool::interfaceID() {
  return IID_IL1TopoByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
L1TopoByteStreamTool::L1TopoByteStreamTool( const std::string& type, const std::string& name,
                                      const IInterface* parent )
  : AthAlgTool( type, name, parent ), m_srcIdMap( 0 ) {
  
  declareInterface< L1TopoByteStreamTool >( this );
  declareProperty("ROBSourceIDs", m_sourceIDs, "ROB fragment source identifiers");
}

/**
 * The destructor doesn't need to do anything.
 */
L1TopoByteStreamTool::~L1TopoByteStreamTool() {

}

/**
 * The function creates a L1TopoSrcIdMap object that is used in the conversion
 * and initialises the base class.
 */
StatusCode L1TopoByteStreamTool::initialize() {

  m_srcIdMap = new L1TopoSrcIdMap();
  return AlgTool::initialize();

}

/**
 * The function deletes the L1TopoSrcIdMap object and finalises the base class.
 */
StatusCode L1TopoByteStreamTool::finalize() {

  delete m_srcIdMap;
  return AlgTool::finalize();

}

/**
 * Return list of L1Topo *DAQ* source IDs to use for conversion.
 * Will generate list according to algorithm herein, if not already provided via properties
 * Used by L1TopoByteStreamCnv and L1TopoByteStreamTool
 * Note that the *ROI* source IDs are not set here; instead see RoIBResultByteStreamTool and ByeStreamAddressProviderSvc.
 */
const std::vector<uint32_t>& L1TopoByteStreamTool::sourceIDs(){
  if (m_sourceIDs.empty()) {
    const int daqOrRoi=0; // DAQ
    const int slink=1; // convention has it that DAQ output is on link 1 of each module
    for (int module: {0,1}){
      const uint32_t id = m_srcIdMap->getRodID (slink, module, daqOrRoi);
      m_sourceIDs.push_back(id);
    }

  }
  return m_sourceIDs;
}

/**
 * Conversion from RDO to eformat::ROBFragment.
 * This is called from the L1TopoByteStreamCnv::createRep method.
 */
StatusCode L1TopoByteStreamTool::convert( const L1TopoRDO* result, RawEventWrite* re ) {

  ATH_MSG_DEBUG("executing convert() from RDO to ROBFragment");

  // Clear Event Assembler
  m_fea.clear();

  // Reset lumi-block number to 1
  m_fea.setDetEvtType( 1 );
  // Set L1Apos to center of readout window
  //uint16_t minorVersion = ( result->getNumberOfBunches() - 1u ) / 2u;
  //minorVersion &= ctpVersion.getL1APositionMask();
  //minorVersion <<= ctpVersion.getL1APositionShift();
  uint16_t minorVersion=0;
  m_fea.setRodMinorVersion( minorVersion );  

  FullEventAssembler< L1TopoSrcIdMap >::RODDATA* theROD;

  // Source ID of L1Topo
  // NB needs to change to using list of modules - as it is, it will just produce one with an unconventional ID of 0x91000000.
  const uint32_t rodId = m_srcIdMap->getRodID();
  ATH_MSG_DEBUG( " ROD ID:" << MSG::hex << rodId );
  const std::vector< uint32_t > rodIds = sourceIDs();
  ATH_MSG_DEBUG( " ROD IDs:" << MSG::hex << rodIds << " NOT YET IMPLEMENTED");


  // get the ROD data container to be filled
  theROD = m_fea.getRodData( rodId );

  ATH_MSG_VERBOSE( " Dumping L1Topo data words:" );
  // fill Data Words
  const std::vector< uint32_t >& vDataWords = result->getDataWords();
  std::vector< uint32_t >::const_iterator it = vDataWords.begin();
  std::vector< uint32_t >::const_iterator end = vDataWords.end();
  for( ; it != end; ++it ) {
    theROD->push_back( *it );
    ATH_MSG_VERBOSE ( "     0x" << MSG::hex << std::setw( 8 )  << ( *it ) );
  }

  // Now fill full event
  ATH_MSG_DEBUG( "Now filling the event with the L1Topo fragment");
  m_fea.fill( re, msg() );

  return StatusCode::SUCCESS;
}


/**
 * Conversion from eformat::ROBFragment to RDO.
 * This is called from the L1TopoByteStreamCnv::createObj method.
 */
// change to VROBFRAG and use IROBDataProviderSvc::VROBFRAG::const_iterator to loop over them
StatusCode L1TopoByteStreamTool::convert( const ROBF* rob, L1TopoRDO*& result ) {

  ATH_MSG_DEBUG( "executing convert() from ROBFragment to RDO" );
  
  const std::vector<uint32_t> l1TopoRodIds = sourceIDs();
  uint32_t rodId = rob->rod_source_id();

  ATH_MSG_DEBUG( "expected ROD sub-detector ID: " << MSG::hex << l1TopoRodIds 
		 << " ID found: " << MSG::hex << rodId << MSG::dec );  

  bool idMatch=false;
  for (auto id: l1TopoRodIds){
    if (rodId == id){
      idMatch=true;
    }
  }
  if ( idMatch ) {

    // Need to check S-link trailer for errors - this would be propagated to status word, see below.
    // First use ROB & ROD error check methods
    bool error_rob(false);
    bool error_rod(false);
    try{
      if (rob->check_rob()){
	ATH_MSG_VERBOSE( "ROB fragment checked ok" );
      }
    }
    catch (std::exception const & ex){
      ATH_MSG_WARNING( "ROB fragment not valid: " << ex.what() );
      error_rob=true;
    }
    try{
      if (rob->check_rod()){
	ATH_MSG_VERBOSE( "ROD fragment checked ok");
      }
    }
    catch (std::exception const & ex){
      ATH_MSG_WARNING( "ROD fragment not valid: " << ex.what() );
      error_rod=true;
    }
    
    // Print some header info
    ATH_MSG_VERBOSE( MSG::hex << " \n" 
		     << "  rod_version           0x" << rob->rod_version() << " \n" 
		     << "  rod_run_no            0x" << MSG::dec << rob->rod_run_no() << " \n" 
		     << "  rod_lvl1_id           0x" << MSG::hex << rob->rod_lvl1_id() << " \n" 
		     << "  rod_bc_id             0x" << rob->rod_bc_id() << " \n" 
		     << "  rod_lvl1_trigger_type 0x" << rob->rod_lvl1_trigger_type() << " \n" 
		     << "  nchildren             0x" << rob->nchildren() << " \n" 
		     << MSG::dec );

    // print and check status words
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_status = rob->rod_status();
    const uint32_t nstatus = rob->rod_nstatus();
    ATH_MSG_VERBOSE( "Number of status words: " << nstatus );
    std::vector<uint32_t> vStatusWords;
    ATH_MSG_VERBOSE( "Dumping ROD status words:" );
    for ( uint32_t i = 0; i < nstatus; ++i, ++it_status ) {
      vStatusWords.push_back( static_cast<uint32_t>( *it_status ) );
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setfill ('0') << std::setw( 8 )  << *it_status << MSG::dec );
    }
    // for definition of first status word see:
    // - bits 00-15: https://edms.cern.ch/document/445840/4.0c section 5.8 
    // - bits 16-31: bits 16-31: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ROBINFragmentErrors
    // Can do something more specific eventually.
    bool error_status(false);
    if (vStatusWords[0]!=0){
      ATH_MSG_WARNING( "non-zero status word, payload may not be valid" );
      error_status=true;
    }

    // print and interpret data words and save them for RDO
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data = rob->rod_data();
    const uint32_t ndata = rob->rod_ndata();
    ATH_MSG_VERBOSE( "Number of data words: " << ndata);

    ATH_MSG_VERBOSE( "Dumping L1Topo data words:");
    std::vector<uint32_t> vDataWords;
    for ( uint32_t i = 0; i < ndata; ++i, ++it_data ) {
      vDataWords.push_back( static_cast<uint32_t>( *it_data ) );  
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setfill ('0') << std::setw( 8 )  << *it_data << MSG::dec);	
    }

    // create L1Topo RDO
    result = new L1TopoRDO();
    result->setDataWords(vDataWords);
    result->setStatusWords(vStatusWords);
    result->setError(error_status<<2 | error_rob<<1 | error_rod);
    result->setSourceID(rodId);

    return StatusCode::SUCCESS;

  }
  
  ATH_MSG_ERROR( "Wrong ROD ID found in the L1Topo ROB fragment!" );
  return StatusCode::FAILURE;
}
