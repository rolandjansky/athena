/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "TrigT1ResultByteStream/RoIBResultByteStreamTool.h"

// Trigger includes:
#include "CTPfragment/CTPdataformat.h"
#include "CTPfragment/CTPfragment.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/L1TopoRDO.h"
#include "TrigT1Result/L1TopoResult.h"
#include "TrigT1Result/RoIBResult.h"

// Gaudi includes:
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

// TDAQ includes:
#include "eformat/SourceIdentifier.h"

// System includes
#include <exception>
#include <sstream>

using DataType = OFFLINE_FRAGMENTS_NAMESPACE::PointerType;

/// Unique interface ID of the tool that identifies it to the framweork
static const InterfaceID IID_IRoIBResultByteStreamTool( "RoIBResultByteStreamTool", 2, 0 );

/**
 * This function is needed by the framework to know what kind of tool
 * this is.
 */
const InterfaceID& RoIBResultByteStreamTool::interfaceID() {
  return IID_IRoIBResultByteStreamTool;
}

/**
 * The constructor takes care of correctly constructing the base class and
 * declaring the tool's interface to the framework.
 */
RoIBResultByteStreamTool::RoIBResultByteStreamTool( const std::string& type, const std::string& name,
                                                    const IInterface* parent )
  : AthAlgTool( type, name, parent ) {

  declareInterface< RoIBResultByteStreamTool >( this );

}

/**
 * The destructor doesn't do anything.
 */
RoIBResultByteStreamTool::~RoIBResultByteStreamTool() {

}

/**
 * @brief Initialise the tool
 *
 * Fill the vector of configured ROB IDs and print the module IDs to debug message stream
 */
StatusCode RoIBResultByteStreamTool::initialize() {
  std::vector<eformat::helper::SourceIdentifier> configuredROBSIDs;
  std::ostringstream str;
  ATH_MSG_DEBUG("Configured module IDs for:");

  // CTP
  configuredROBSIDs.emplace_back(eformat::TDAQ_CTP, m_ctpModuleID);
  ATH_MSG_DEBUG("   CTP                                  = 0x" << MSG::hex << m_ctpModuleID.value() << MSG::dec);

  // MUCTPI
  configuredROBSIDs.emplace_back(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID);
  ATH_MSG_DEBUG("   muCTPi                               = 0x" << MSG::hex << m_muCTPIModuleID.value() << MSG::dec);

  // Jet/Energy
  str.str("");
  for (const uint16_t module_id : m_jetModuleID) {
    configuredROBSIDs.emplace_back(eformat::TDAQ_CALO_JET_PROC_ROI, module_id);
    str << "0x" << std::hex << module_id << std::dec << " ";
  }
  ATH_MSG_DEBUG("   Calorimeter Jet/Energy Processor RoI = " << str.str());

  // EM/Tau
  str.str("");
  for (const uint16_t module_id : m_emModuleID) {
    configuredROBSIDs.emplace_back(eformat::TDAQ_CALO_CLUSTER_PROC_ROI, module_id);
    str << "0x" << std::hex << module_id << std::dec << " ";
  }
  ATH_MSG_DEBUG("   Calorimeter Cluster Processor RoI    = " << str.str());

  // L1Topo
  str.str("");
  for (const uint16_t module_id : m_l1TopoModuleID) {
    configuredROBSIDs.emplace_back(eformat::TDAQ_CALO_TOPO_PROC, module_id);
    str << "0x" << std::hex << module_id << std::dec << " ";
  }
  ATH_MSG_DEBUG("   L1Topo                               = " << str.str());

  // Fill the ROB ID vector
  for (const auto& sid : configuredROBSIDs) {
    m_configuredROBIds.push_back( sid.code() );
  }

  return StatusCode::SUCCESS;
}

/**
 * The function finalises the base class.
 */
StatusCode RoIBResultByteStreamTool::finalize() {

  return AlgTool::finalize();
}

/**
 * Conversion from RDO to eformat::ROBFragment.
 * This is called from the RoIBResultByteStreamCnv::createRep method.
 */
StatusCode RoIBResultByteStreamTool::convert( ROIB::RoIBResult* result, RawEventWrite* re ) {

  // Clear the event assembler:
  m_fea.clear();
  // Reset lumi-block number to 1
  m_fea.setDetEvtType( 1 );

  ATH_MSG_DEBUG("executing convert() from RDO to ROBFragments");

  // I use only this one pointer now to fill the RODs:
  FullEventAssembler< L1SrcIdMap >::RODDATA* theROD;

  /** CTP ROD */
  ATH_MSG_VERBOSE("   Dumping CTP RoI words:");
  eformat::helper::SourceIdentifier helpCTPID( eformat::TDAQ_CTP, m_ctpModuleID );
  uint32_t rodIdCTP = helpCTPID.code();
  theROD = m_fea.getRodData( rodIdCTP );
  const std::vector< ROIB::CTPRoI >& CTPRoIVec = result->cTPResult().roIVec();
  for( std::vector< ROIB::CTPRoI >::const_iterator j = CTPRoIVec.begin(); j != CTPRoIVec.end(); ++j ) {
    ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
    theROD->push_back( j->roIWord() );
  }

  /** Muon ROD */
  ATH_MSG_VERBOSE( "   Dumping MuCTPI RoI words:" );
  eformat::helper::SourceIdentifier helpMuonID( eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID );
  uint32_t rodIdMuon = helpMuonID.code();
  theROD = m_fea.getRodData( rodIdMuon );
  const std::vector< ROIB::MuCTPIRoI >& MuCTPIRoIVec = result->muCTPIResult().roIVec();
  for( std::vector< ROIB::MuCTPIRoI >::const_iterator j = MuCTPIRoIVec.begin();
       j != MuCTPIRoIVec.end(); ++j ) {
    ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
    theROD->push_back( j->roIWord() );
  }

  /** Jet/Energy ROD */
  const std::vector< ROIB::JetEnergyResult >& jetEnergyResultVector = result->jetEnergyResult();
  for( unsigned int slink = 0; slink < jetEnergyResultVector.size(); ++slink ) {
    ATH_MSG_VERBOSE( "   Dumping Jet/Energy " << slink << " RoI words:" );
    eformat::helper::SourceIdentifier helpJetID( eformat::TDAQ_CALO_JET_PROC_ROI, m_jetModuleID[slink] );
    theROD = m_fea.getRodData( helpJetID.code() );
    const std::vector< ROIB::JetEnergyRoI >& JetEnergyRoIVec = jetEnergyResultVector[ slink ].roIVec();
    for( std::vector< ROIB::JetEnergyRoI >::const_iterator j = JetEnergyRoIVec.begin();
         j != JetEnergyRoIVec.end(); ++j ) {
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
      theROD->push_back( j->roIWord() );
    }
  }

  /* EMTau RODs */
  const std::vector< ROIB::EMTauResult >& eMTauRDOResultVector = result->eMTauResult();
  for( unsigned int slink = 0; slink < eMTauRDOResultVector.size(); ++slink ) {
    ATH_MSG_VERBOSE( "   Dumping EM/Tau " << slink << " RoI words:" );
    eformat::helper::SourceIdentifier helpEMTauID( eformat::TDAQ_CALO_CLUSTER_PROC_ROI, m_emModuleID[slink]);
    theROD = m_fea.getRodData( helpEMTauID.code() );
    const std::vector< ROIB::EMTauRoI >& EMTauRoIVec = eMTauRDOResultVector[ slink ].roIVec();
    for( std::vector<ROIB::EMTauRoI>::const_iterator j = EMTauRoIVec.begin();
         j != EMTauRoIVec.end(); ++j ) {
      ATH_MSG_VERBOSE( "     0x" << MSG::hex << std::setw( 8 ) << j->roIWord() );
      theROD->push_back( j->roIWord() );
    }
  }

  /** L1Topo ROD */
  ATH_MSG_VERBOSE( "   Dumping L1Topo RoI words" );
  const std::vector< ROIB::L1TopoResult >& l1TopoResultVector = result->l1TopoResult();
  ATH_MSG_VERBOSE( "   Dumping L1Topo RoI words for " << l1TopoResultVector.size() << " RODs:" );
  unsigned int slink(0);
  for (auto & l1tr : l1TopoResultVector){
    uint32_t sourceID = l1tr.rdo().getSourceID(); // needs to be a 16-bit module ID
    ATH_MSG_VERBOSE( "  for L1Topo source ID from RDO "  << L1Topo::formatHex8(sourceID) );
    if (sourceID == 0 && slink < m_l1TopoModuleID.size()){
      sourceID = eformat::helper::SourceIdentifier( eformat::TDAQ_CALO_TOPO_PROC, m_l1TopoModuleID.value().at(slink) ).code();
      ATH_MSG_DEBUG("  (source ID in L1TopoRDO was zero so using Property for slink " << slink << ": " << L1Topo::formatHex8(sourceID) << ")");
    }
    else if (sourceID == 0){
      sourceID=eformat::helper::SourceIdentifier( eformat::TDAQ_CALO_TOPO_PROC, 0 ).code();
      ATH_MSG_WARNING("  (source ID in L1TopoRDO was zero, no properties available for slink counter " << slink << ", so as a fall back, constructed module 0 with source ID " << L1Topo::formatHex8(sourceID) << ")");
    }
    theROD = m_fea.getRodData( sourceID );
    for ( auto & word : l1tr.rdo().getDataWords()){
      ATH_MSG_VERBOSE("     " << MSG::hex << std::setw(8) << std::showbase << word << std::noshowbase << std::dec);
      theROD->push_back(word);
    }
    ++slink;
  }
  /** now fill them into the RawEvent re */
  ATH_MSG_DEBUG("Now filling the event with the RoI Builder fragments");
  MsgStream log( msgSvc(), name() );
  m_fea.fill( re, log );

  return StatusCode::SUCCESS;
}

/**
 * @brief Conversion from eformat::ROBFragments to RDO
 *
 * This is called from the decoder algorithm RoIBResultByteStreamDecoderAlg
 */
StatusCode RoIBResultByteStreamTool::convert(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vrobf,
                                             ROIB::RoIBResult& resultToFill) const {
  ATH_MSG_DEBUG("executing convert() from ROBFragments to RDO");

  // Create all RDOs
  ROIB::CTPResult cTPResult;
  ROIB::MuCTPIResult muCTPIResult;
  std::vector< ROIB::JetEnergyResult > jetEnergyResult(2);
  std::vector< ROIB::EMTauResult > eMTauResult(4);
  std::vector< ROIB::L1TopoResult > l1TopoResult;

  // Create flags indicating whether or not ROB fragment was found
  bool cTPFound = false;
  bool muCTPIFound = false;
  bool jetEnergyFound[2] = {false, false};
  bool eMTauFound[4] = {false, false, false, false};
  bool l1TopoFound = false;

  // Loop over ROB fragments
  for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* p_robf : vrobf) {
    const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment robf = *p_robf;
    eformat::helper::SourceIdentifier rodSID(robf.rod_source_id());
    eformat::SubDetector rodSubdetId = rodSID.subdetector_id();
    uint16_t rodModuleId = rodSID.module_id();

    // -------------------------------------------------------------------------
    // CTP
    // -------------------------------------------------------------------------
    if (rodSubdetId == eformat::TDAQ_CTP) {
      // Check if the current ROD module ID matches the configured CTP module ID.
      // Accept also ctpModuleId=0 to catch the early data with the old CTP firmware, which assigns 0x770000 to both
      // DAQ and LVL2 ROD fragment - for this data we readout always multiple bunches for the DAQ ROD, therefore
      // rodFragSize=46 should identify the LVL2 ROD fragment
      if (rodModuleId != m_ctpModuleID && rodModuleId != 0) continue;
      if (rodModuleId == 0 && robf.rod_fragment_size_word() != 46) continue;

      // Flag as found
      cTPFound = true;
      ATH_MSG_DEBUG("   Found CTP ROB with source ID " << MSG::hex << rodSID.code() << MSG::dec);

      // Extract the data
      DataStatus status;
      ROIB::Header header = roibHeader(robf, status);
      std::vector<ROIB::CTPRoI> content = roibContent<ROIB::CTPRoI>(robf);
      ROIB::Trailer trailer = roibTrailer(status, content.size());

      // Extract the CTP version number
      const uint32_t* rod;
      robf.rod_start(rod);
      unsigned int ctpVersionNumber = ((rod[CTPdataformat::Helper::FormatVersionPos] >> CTPdataformat::CTPFormatVersionShift) & CTPdataformat::CTPFormatVersionMask);

      // Create CTPResult object
      cTPResult = ROIB::CTPResult(ctpVersionNumber, header, trailer, content);
    }

    // -------------------------------------------------------------------------
    // MUCTPI
    // -------------------------------------------------------------------------
    if (rodSubdetId == eformat::TDAQ_MUON_CTP_INTERFACE) {
      // Check if the current ROD module ID matches the configured MuCTPI module ID
      if (rodModuleId != m_muCTPIModuleID) continue;

      // Flag as found
      muCTPIFound = true;
      ATH_MSG_DEBUG("   Found MuCTPI ROB with source ID " << MSG::hex << rodSID.code() << MSG::dec);

      // Extract the data
      DataStatus status;
      ROIB::Header header = roibHeader(robf, status);
      std::vector<ROIB::MuCTPIRoI> content = roibContent<ROIB::MuCTPIRoI>(robf);
      ROIB::Trailer trailer = roibTrailer(status, content.size());

      // Create MuCTPIResult object
      muCTPIResult = ROIB::MuCTPIResult(header, trailer, content);
    }

    // -------------------------------------------------------------------------
    // Jet/Energy
    // -------------------------------------------------------------------------
    if (rodSubdetId == eformat::TDAQ_CALO_JET_PROC_ROI) {
      // Check if the current ROD module ID matches the configured Jet/Energy module IDs
      auto it = std::find(m_jetModuleID.begin(), m_jetModuleID.end(), rodModuleId);
      if (it == m_jetModuleID.end()) continue;
      size_t index = static_cast<size_t>(std::distance(m_jetModuleID.begin(), it));

      // Flag as found
      jetEnergyFound[index] = true;
      ATH_MSG_DEBUG("   Found Jet/Energy ROB with source ID " << MSG::hex << rodSID.code() << MSG::dec);

      // Extract the data
      DataStatus status;
      ROIB::Header header = roibHeader(robf, status);
      std::vector<ROIB::JetEnergyRoI> content = roibContent<ROIB::JetEnergyRoI>(robf);
      ROIB::Trailer trailer = roibTrailer(status, content.size());

      // Create JetEnergyResult object
      jetEnergyResult[index] = ROIB::JetEnergyResult(header, trailer, content);
    }

    // -------------------------------------------------------------------------
    // EM/Tau
    // -------------------------------------------------------------------------
    if (rodSubdetId == eformat::TDAQ_CALO_CLUSTER_PROC_ROI) {
      // Check if the current ROD module ID matches the configured EM/Tau module IDs
      auto it = std::find(m_emModuleID.begin(), m_emModuleID.end(), rodModuleId);
      if (it == m_emModuleID.end()) continue;
      size_t index = static_cast<size_t>(std::distance(m_emModuleID.begin(), it));

      // Flag as found
      eMTauFound[index] = true;
      ATH_MSG_DEBUG("   Found EM/Tau ROB with source ID " << MSG::hex << rodSID.code() << MSG::dec);

      // Extract the data
      DataStatus status;
      ROIB::Header header = roibHeader(robf, status);
      std::vector<ROIB::EMTauRoI> content = roibContent<ROIB::EMTauRoI>(robf);
      ROIB::Trailer trailer = roibTrailer(status, content.size());

      // Create EMTauResult object
      eMTauResult[index] = ROIB::EMTauResult(header, trailer, content);
    }

    // -------------------------------------------------------------------------
    // L1Topo
    // -------------------------------------------------------------------------
    if (rodSubdetId == eformat::TDAQ_CALO_TOPO_PROC) {
      // Check if the current ROD module ID matches the configured L1Topo module IDs
      auto it = std::find(m_l1TopoModuleID.begin(), m_l1TopoModuleID.end(), rodModuleId);
      if (it == m_l1TopoModuleID.end()) continue;

      // Flag as found
      l1TopoFound = true;
      ATH_MSG_DEBUG("   Found L1Topo ROB with source ID " << MSG::hex << rodSID.code() << MSG::dec);

      // Extract the data
      DataStatus status;
      ROIB::Header header = roibHeader(robf, status);
      L1TopoRDO content = l1topoContent(robf);
      ROIB::Trailer trailer = roibTrailer(status, content.getDataWords().size());

      // Set status words
      content.setStatusWords({status.status_word, status.status_info});

      // Flag errors in RDO
      if (status.status_word != 0) content.setError(L1Topo::Error::SLINK_STATUS_ERROR);
      if (status.rob_error) content.setError(L1Topo::Error::ROB_ERROR);
      if (status.rod_error) content.setError(L1Topo::Error::ROD_ERROR);

      // Create L1TopoResult object
      l1TopoResult.push_back(ROIB::L1TopoResult(header, trailer, content));
    }

  } // End of loop over all ROB fragments

  ATH_MSG_DEBUG("Building RoIBResult with the following inputs:");
  ATH_MSG_DEBUG("  CTP             - " << (cTPFound ? "found" : "not found"));
  ATH_MSG_DEBUG("  MUCTPI          - " << (muCTPIFound ? "found" : "not found"));
  ATH_MSG_DEBUG("  Jet/Energy[0/1] - " << (jetEnergyFound[0] ? "found" : "not found") << "/"
                                       << (jetEnergyFound[1] ? "found" : "not found"));
  ATH_MSG_DEBUG("  EM/Tau[0/1/2/3] - " << (eMTauFound[0] ? "found" : "not found") << "/"
                                       << (eMTauFound[1] ? "found" : "not found") << "/"
                                       << (eMTauFound[2] ? "found" : "not found") << "/"
                                       << (eMTauFound[3] ? "found" : "not found"));
  ATH_MSG_DEBUG("  L1Topo          - " << (l1TopoFound ? "found" : "not found"));

  resultToFill = ROIB::RoIBResult(muCTPIResult, cTPResult, jetEnergyResult, eMTauResult);
  if (l1TopoFound) resultToFill.l1TopoResult(l1TopoResult);

  return StatusCode::SUCCESS;
}

ROIB::Header RoIBResultByteStreamTool::roibHeader(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob,
                                                  DataStatus& dataStatus) const {
  // Get format version and event number of fragment
  uint32_t formatVersion = rob.rod_version();
  uint32_t evtNum = rob.rod_lvl1_id();
  uint32_t robFragSize = rob.fragment_size_word();
  uint32_t rodFragSize = rob.rod_fragment_size_word();
  uint32_t robId = rob.source_id();
  uint32_t rodId = rob.rod_source_id();

  // Check for errors
  dataStatus.rob_error = false;
  dataStatus.rod_error = false;
  dataStatus.status_word = 0;
  dataStatus.status_info = 0;

  try {
    if (rob.check_rob()) ATH_MSG_VERBOSE("ROB fragment checked ok");
  }
  catch (std::exception const & ex) {
    ATH_MSG_WARNING("ROB fragment not valid: " << ex.what());
    dataStatus.rob_error = true;
  }

  try {
    if (rob.check_rod()) ATH_MSG_VERBOSE("ROD fragment checked ok");
  }
  catch (std::exception const & ex) {
    ATH_MSG_WARNING("ROD fragment not valid: " << ex.what());
    dataStatus.rod_error = true;
  }

  // Process the status words
  DataType status;
  rob.rod_status(status);
  uint32_t nstatus = rob.rod_nstatus();
  ATH_MSG_VERBOSE("Number of status words: " << nstatus);
  if (msgLvl(MSG::VERBOSE)) {
    for (uint32_t i=0; i<nstatus; ++i, ++status) {
      ATH_MSG_VERBOSE("   Status word: 0x" << MSG::hex << std::setw(8) << std::setfill('0') << *status);
    }
  }
  rob.rod_status(status);

  if(nstatus > 0) dataStatus.status_word = static_cast<uint32_t>(*status);
  if(nstatus > 1) {
      ++status;
      dataStatus.status_info = static_cast<uint32_t>(*status);
  }

  ATH_MSG_DEBUG("ROB ID 0x" << MSG::hex << robId <<  " ROD ID 0x" << rodId << MSG::dec << " ROB fragment size "
                << robFragSize << " ROD fragment size " << rodFragSize);

  return ROIB::Header(rodId, evtNum, formatVersion);
}

ROIB::Trailer RoIBResultByteStreamTool::roibTrailer(const DataStatus& dataStatus, uint32_t dataSize) const {
  std::vector<uint32_t> words;
  words.reserve(5);
  words.push_back(dataStatus.status_word); // error status
  words.push_back(dataStatus.status_info); // status info
  words.push_back(2);                      // number of status words
  words.push_back(dataSize);               // number of data words
  words.push_back(1);                      // status block position
  return ROIB::Trailer(words);
}

template<typename RoIType> std::vector<RoIType> RoIBResultByteStreamTool::roibContent(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob) const {
  uint32_t ndata = rob.rod_ndata();
  DataType data;
  rob.rod_data(data);
  std::vector<RoIType> content;
  content.reserve(ndata);
  ATH_MSG_VERBOSE("   Dumping RoI Words:");
  for (uint32_t i=0; i<ndata; ++i, ++data) {
    ATH_MSG_VERBOSE("       0x" << MSG::hex << std::setfill('0') << std::setw(8)
                    << static_cast<uint32_t>(*data) << MSG::dec);
    RoIType thisRoI(static_cast<uint32_t>(*data));
    content.push_back(thisRoI);
  }
  return content;
}

L1TopoRDO RoIBResultByteStreamTool::l1topoContent(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& rob) const {
  uint32_t ndata = rob.rod_ndata();
  DataType data;
  rob.rod_data(data);
  L1TopoRDO content;
  ATH_MSG_VERBOSE( "   Dumping RoI Words:" );
  std::vector<uint32_t> vDataWords;
  vDataWords.reserve(ndata);
  for (uint32_t i=0; i<ndata; ++i, ++data) {
    ATH_MSG_VERBOSE("       0x" << MSG::hex << std::setfill('0') << std::setw(8)
                    << static_cast<uint32_t>(*data) << MSG::dec);
    vDataWords.push_back(static_cast<uint32_t>(*data));
  }
  content.setDataWords(vDataWords);
  content.setSourceID(rob.rod_source_id());
  return content;
}
