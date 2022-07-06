/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//author: @asonay
//email: anil.sonay@cern.ch

#include "L1TopoPhase1ByteStreamTool.h"

//L1TopoRDO
#include "L1TopoRDO/Error.h"

//xAOD
#include "xAODTrigL1Calo/L1TopoRawData.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;


L1TopoPhase1ByteStreamTool::L1TopoPhase1ByteStreamTool(const std::string& type,
						       const std::string& name,
						       const IInterface* parent)
  : base_class(type, name, parent) {}


StatusCode L1TopoPhase1ByteStreamTool::initialize() {

  ConversionMode topoRawMode = getConversionMode(m_topoRawReadKey, m_topoRawWriteKey, msg());
  ATH_CHECK(topoRawMode!=ConversionMode::Undefined);
  ATH_CHECK(m_topoRawWriteKey.initialize(topoRawMode==ConversionMode::Decoding));
  ATH_CHECK(m_topoRawReadKey.initialize(topoRawMode==ConversionMode::Encoding));
  ATH_MSG_DEBUG((topoRawMode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " topoRaw ROB IDs: "
		<< std::hex << std::showbase << m_robIds.value() << std::dec);

   
  return StatusCode::SUCCESS;

}

// BS->xAOD conversion
StatusCode L1TopoPhase1ByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {

  // prepare container
  auto l1topoContainer = std::make_unique<xAOD::L1TopoRawDataContainer> ();
  auto l1topoAuxContainer = std::make_unique<xAOD::L1TopoRawDataAuxContainer> ();
  l1topoContainer->setStore(l1topoAuxContainer.get());
  
  // Iterate over ROBFragments to decode
  for (const ROBF* rob : vrobf) {
    ATH_CHECK( convert(rob, l1topoContainer) );
  }

  SG::WriteHandle<xAOD::L1TopoRawDataContainer> l1topo_cont(m_topoRawWriteKey, ctx);
  ATH_CHECK(l1topo_cont.record(std::move(l1topoContainer),std::move(l1topoAuxContainer)));
  ATH_MSG_DEBUG("Recorded L1TopoRawDataContainer with key " << l1topo_cont.key());

  return StatusCode::SUCCESS;
}


// xAOD->BS conversion
StatusCode L1TopoPhase1ByteStreamTool::convertToBS(std::vector<WROBF*>& , const EventContext& ) {
  return StatusCode::FAILURE;
}

StatusCode L1TopoPhase1ByteStreamTool::convert(const ROBF* rob, const std::unique_ptr<xAOD::L1TopoRawDataContainer> &container) const {
  ATH_MSG_DEBUG("executing convert() from ROBFragment to xAOD::L1TopoRawData");

  uint32_t rodId = rob->rob_source_id();

  ATH_MSG_DEBUG("ROD sub-detector ID: 0x" << MSG::hex << rodId << MSG::dec);

  // First use ROB & ROD error check methods
  bool error_rob(false);
  bool error_rod(false);
  try {
    if (rob->check_rob()) {
      ATH_MSG_VERBOSE("ROB fragment checked ok");
    }
  } catch (std::exception const& ex) {
    ATH_MSG_WARNING("ROB fragment not valid: " << ex.what());
    error_rob = true;
  }
  try {
    if (rob->check_rod()) {
      ATH_MSG_VERBOSE("ROD fragment checked ok");
    }
  } catch (std::exception const& ex) {
    ATH_MSG_WARNING("ROD fragment not valid: " << ex.what());
    error_rod = true;
  }

  // Print some header info
  ATH_MSG_VERBOSE(
		  MSG::hex << " \n"
		  << "  rod_version           0x" << rob->rod_version() << " \n"
		  << "  rod_run_no            0x" << MSG::dec
		  << rob->rod_run_no() << " \n"
		  << "  rod_lvl1_id           0x" << MSG::hex
		  << rob->rod_lvl1_id() << " \n"
		  << "  rod_bc_id             0x" << rob->rod_bc_id() << " \n"
		  << "  rod_lvl1_trigger_type 0x" << rob->rod_lvl1_trigger_type()
		  << " \n"
		  << "  nchildren             0x" << rob->nchildren() << " \n"
		  << MSG::dec);

  // print and check status words
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_status = rob->rod_status();
  const uint32_t nstatus = rob->rod_nstatus();
  ATH_MSG_VERBOSE("Number of status words: " << nstatus);
  std::vector<uint32_t> vStatusWords;
  vStatusWords.reserve(nstatus);
  ATH_MSG_VERBOSE("Dumping ROD status words:");
  for (uint32_t i = 0; i < nstatus; ++i, ++it_status) {
    vStatusWords.push_back(static_cast<uint32_t>(*it_status));
    ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill('0') << std::setw(8)
		    << *it_status << MSG::dec);
  }
  // for definition of first status word see:
  // - bits 00-15: https://edms.cern.ch/document/445840/5.0a section 5.8
  // - bits 16-31: bits 16-31:
  // https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ROBINFragmentErrors
  // Can do something more specific eventually.
  bool error_status(false);
  if (vStatusWords.size() == 0) {
    ATH_MSG_WARNING("ROD has no status word");
  }
  if (vStatusWords.size() > 0 && vStatusWords.at(0) != 0) {
    ATH_MSG_WARNING("Non-zero first status word, payload may not be valid");
    error_status = true;
  }

  // print and interpret data words and save them for RDO
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data = rob->rod_data();
  const uint32_t ndata = rob->rod_ndata();
  ATH_MSG_VERBOSE("Number of data words: " << ndata);

  ATH_MSG_VERBOSE("Dumping L1Topo data words:");
  std::vector<uint32_t> vDataWords;
  vDataWords.reserve(ndata);
  for (uint32_t i = 0; i < ndata; ++i, ++it_data) {
    vDataWords.push_back(static_cast<uint32_t>(*it_data));
    ATH_MSG_VERBOSE("     0x" << MSG::hex << std::setfill('0') << std::setw(8)
		    << *it_data << MSG::dec);
  }
  
  //Set Error
  uint32_t error = 0;
  if (error_status) {
    error |= (1 << static_cast<unsigned int>(L1Topo::Error::SLINK_STATUS_ERROR));
  }
  if (error_rob) {
    error |= (1 << static_cast<unsigned int>(L1Topo::Error::ROB_ERROR));
  }
  if (error_rod) {
    error |= (1 << static_cast<unsigned int>(L1Topo::Error::ROD_ERROR));
  }
  
  // Fill xAOD
  container->push_back(std::make_unique<xAOD::L1TopoRawData>());
  container->back()->initialize(vDataWords,vStatusWords,error,rodId);

  return StatusCode::SUCCESS;

}
