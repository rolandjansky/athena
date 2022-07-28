/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "L1TopoOnlineMonitor.h"

// Trigger includes
#include "xAODTrigger/L1TopoSimResults.h"
#include "TrigT1Result/CTP_Decoder.h"

// System includes
#include <utility>


// Local helper constants and functions
namespace {
  /// Return a vector of indices corresponding to bits set to 1 in the bitset
  template<size_t N>
  std::vector<size_t> bitsetIndices(const std::bitset<N>& bits) {
    std::vector<size_t> indices;
    indices.reserve(bits.count());
    for (size_t i=0; i<bits.size(); ++i) {
      if (bits[i]) indices.push_back(i);
    }
    return indices;
  }
}

// =============================================================================
// Standard constructor
// =============================================================================
L1TopoOnlineMonitor::L1TopoOnlineMonitor(const std::string& name, ISvcLocator* svcLoc)
  : AthMonitorAlgorithm(name, svcLoc),
    m_ctpIds(0)
{}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TopoOnlineMonitor::initialize() {

  ATH_CHECK(m_l1topoKey.initialize());
  ATH_CHECK(m_ctpRdoKey.initialize(m_doHwMonCTP));
  ATH_CHECK(m_l1topoRawDataKey.initialize(m_doHwMon));
  ATH_CHECK(m_monTool.retrieve(DisableTool{m_monTool.name().empty()}));

  return AthMonitorAlgorithm::initialize();
}

StatusCode L1TopoOnlineMonitor::start() {

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( m_detStore->retrieve(l1menu) ); 

   m_ctpIds = getCtpIds(*l1menu);

   return StatusCode::SUCCESS;
}


StatusCode L1TopoOnlineMonitor::fillHistograms( const EventContext& ctx ) const {
  
  DecisionBits decisionBits{};
  enum class MonFunction : uint8_t {doSimMon=0, doHwMonCTP, doHwMon, doComp};
  std::vector<uint8_t> failedMonFunctions;

  
  if (m_doHwMon) {
    StatusCode sc = doHwMon(decisionBits,ctx);
    ATH_MSG_DEBUG("Executed doHWMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doHwMon));
    }    
  }
  
  if (m_doSimMon) {
    StatusCode sc = doSimMon(decisionBits,ctx);
    ATH_MSG_DEBUG("Executed doSimMon: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doSimMon));
    }    
  }

  if (m_doHwMonCTP) {
    StatusCode sc = doHwMonCTP(decisionBits,ctx);
    ATH_MSG_DEBUG("Executed doHWMonCTP: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doHwMonCTP));
    }    
  }

  if (m_doComp) {
    StatusCode sc = doComp(decisionBits);
    ATH_MSG_DEBUG("Executed doComp: " << (sc.isFailure() ? "failed" : "ok"));
    if (sc.isFailure()) {
      failedMonFunctions.push_back(static_cast<uint8_t>(MonFunction::doComp));
    }    
  }
  
  auto monFailedMonFunctions = Monitored::Collection("MonitoringFailures", failedMonFunctions);
  Monitored::Group(m_monTool, monFailedMonFunctions);

  return StatusCode::SUCCESS;
}


StatusCode L1TopoOnlineMonitor::doSimMon( DecisionBits& decisionBits, const EventContext& ctx ) const {
  
  
  SG::ReadHandle<xAOD::L1TopoSimResultsContainer> cont(m_l1topoKey, ctx);
  if(!cont.isValid()){
    ATH_MSG_FATAL("Could not retrieve L1Topo EDM Container from the Simulation.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("----got L1Topo container: " << cont.key());

  std::bitset<s_nTopoCTPOutputs>& triggerBitsSim = DecisionBits::createBits(decisionBits.triggerBitsSim);
  for(const auto l1topo_dec : * cont){
    ATH_MSG_DEBUG( "Reading L1Topo EDM:: Connection ID: " << l1topo_dec->connectionId() << " Clock: " << l1topo_dec->clock() << " Bit-length: " << l1topo_dec->bitWidth() << " Word: " << l1topo_dec->topoWord() << " Word64: " << l1topo_dec->topoWord64() );
    
    std::vector<unsigned> topoword;

    if (l1topo_dec->bitWidth() == 32) {
      for(unsigned int i=0; i<32; ++i) {
	uint32_t mask = 0x1; mask <<= i;
	if ((l1topo_dec->topoWord() & mask) !=0)
	  {
	    topoword.push_back(32*l1topo_dec->clock()+i);
	    uint32_t pos = 32*l1topo_dec->clock()*(l1topo_dec->connectionId()-1)+i;
	    triggerBitsSim[pos] = (!decisionBits.triggerBits.has_value() && m_ctpIds[pos]>=512) ? false : true;
	  }
      }
      std::string name = "CableElec_";
      name += std::to_string(l1topo_dec->connectionId());
      auto monTopoDec = Monitored::Collection(name, topoword);
      Monitored::Group(m_monTool,monTopoDec);
    }
    else if (l1topo_dec->bitWidth() == 64) {
      for(unsigned int i=0; i<64; ++i) {
	uint64_t mask = 0x11; mask <<= i;
	if ((l1topo_dec->topoWord64() & mask) !=0)
	  {topoword.push_back(64*l1topo_dec->clock()+i);}
      }
      std::string name = "CableOpti_";
      name += std::to_string(l1topo_dec->connectionId());
      auto monTopoDec = Monitored::Collection(name, topoword);
      Monitored::Group(m_monTool,monTopoDec);
    }
    else {
      ATH_MSG_DEBUG( "Unknown Bit-length: " << l1topo_dec->bitWidth() );
      return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode L1TopoOnlineMonitor::doHwMonCTP( DecisionBits& decisionBits, const EventContext& ctx ) const {
  
  // Retrieve CTP DAQ data for comparison
  SG::ReadHandle<CTP_RDO> ctpRdo{m_ctpRdoKey, ctx};
  if (!ctpRdo.isValid()) {
    ATH_MSG_DEBUG("Failed to retrieve CTP_RDO object (converted from CTP DAQ ROB) with key \""
                  << m_ctpRdoKey.key() << "\". Skipping CTP hardware comparison");
    return StatusCode::FAILURE;
  }

  // CTP RDO contains 17 TBP words for a number of BCs, so use CTP_Decoder to access accepted BC
  CTP_Decoder ctp;
  ctp.setRDO(ctpRdo.cptr());
  const uint32_t l1aPos = ctpRdo->getL1AcceptBunchPosition();
  if (l1aPos >= ctp.getBunchCrossings().size()) {
    ATH_MSG_DEBUG("CTP_RDO gave invalid l1aPos. Skipping CTP hardware comparison");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("CTP l1aPos, size: " << l1aPos << ", " << ctp.getBunchCrossings().size());
  const CTP_BC& ctpL1a = ctp.getBunchCrossings().at(l1aPos);

  // Fill decision bits from CTP RDO
  std::bitset<s_nTopoCTPOutputs>& triggerBitsCtp = DecisionBits::createBits(decisionBits.triggerBitsCtp);
  static constexpr size_t ctpTBPSize{512};
  const std::bitset<ctpTBPSize>& tbp = ctpL1a.getTBP();
  ATH_MSG_VERBOSE("CTP TBP bits: " << tbp.to_string());

  for (size_t i=0; i<s_nTopoCTPOutputs; ++i) {
    if (m_ctpIds[i] < 512)
      {triggerBitsCtp[i] = tbp.test(m_ctpIds[i]);}
    else
      {triggerBitsCtp[i] = false;}
  }

  std::vector<size_t> triggerBitIndicesCtp = bitsetIndices(triggerBitsCtp);
  auto monTopoCtp = Monitored::Collection("TopoCTP", triggerBitIndicesCtp);
  Monitored::Group(m_monTool,monTopoCtp);

  return StatusCode::SUCCESS;
}

StatusCode L1TopoOnlineMonitor::doHwMon( DecisionBits& decisionBits, const EventContext& ctx ) const {
  
  SG::ReadHandle<xAOD::L1TopoRawDataContainer> cont(m_l1topoRawDataKey, ctx);
  if(!cont.isValid()){
    ATH_MSG_FATAL("Could not retrieve L1Topo RAW Data Container from the BS data.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("----got L1Topo Raw Data container: " << cont.key());

  std::bitset<s_nTopoCTPOutputs>& triggerBits = DecisionBits::createBits(decisionBits.triggerBits);
  for (size_t i=0; i<s_nTopoCTPOutputs; ++i) {
    //Do something here when decoders are available
    triggerBits[i] = 0;
  }

  //This will be filled up when the phase1 L1Topo decoders are ready.
  return StatusCode::FAILURE;
}

StatusCode L1TopoOnlineMonitor::doComp( DecisionBits& decisionBits ) const {
  if (!decisionBits.triggerBitsSim.has_value()) {
    ATH_MSG_DEBUG("Simulation bits not set. Skipping simulation to hardware comparison");
    return StatusCode::FAILURE;
  }

  std::bitset<s_nTopoCTPOutputs> triggerBitsSim = decisionBits.triggerBitsSim.value(); // Alias
  std::bitset<s_nTopoCTPOutputs> triggerBitsHdw;

  if (decisionBits.triggerBits.has_value())
    {triggerBitsHdw = decisionBits.triggerBits.value();}
  else if (decisionBits.triggerBitsCtp.has_value())
    {triggerBitsHdw = decisionBits.triggerBitsCtp.value();}
  else {
    ATH_MSG_DEBUG("Hardware bits not set. Skipping simulation to hardware comparison");
    return StatusCode::FAILURE;
  }

  std::vector<size_t> triggerBitIndicesSimNotHdw = bitsetIndices(triggerBitsSim & (~triggerBitsHdw));
  std::vector<size_t> triggerBitIndicesHdwNotSim = bitsetIndices(triggerBitsHdw & (~triggerBitsSim));
  auto monSimNotHdw = Monitored::Collection("SimNotHdwL1TopoResult", triggerBitIndicesSimNotHdw);
  auto monHdwNotSim = Monitored::Collection("HdwNotSimL1TopoResult", triggerBitIndicesHdwNotSim);

  Monitored::Group(m_monTool, monSimNotHdw, monHdwNotSim);


  return StatusCode::SUCCESS;
}


std::vector<unsigned> L1TopoOnlineMonitor::getCtpIds( const TrigConf::L1Menu& l1menu ) {
  
  // Topo
  std::vector<std::string> connNames = l1menu.connectorNames();
  std::vector<std::string> labelsTopoEl(s_nTopoCTPOutputs);
  for( const std::string connName : {"Topo2El", "Topo3El"}) {
    if( find(connNames.begin(), connNames.end(), connName) == connNames.end() ) {
      continue;
    }
    for(uint fpga : {0,1}) {
      for(uint clock : {0,1}) {
	for(auto & tl : l1menu.connector(connName).triggerLines(fpga,clock)) {
	  uint flatIndex = tl.flatindex() + 64 * (connName == "Topo3El"); 
	  labelsTopoEl[flatIndex] = tl.name();
	}
      }
    }
  }

  ATH_MSG_DEBUG("Obtaining CTPIds for Phase1 L1Topo Monitoring");
  std::vector<unsigned> ctpIds(s_nTopoCTPOutputs,999);
  for( const auto & item : l1menu ) {
    std::string definition = item.definition();
    if (definition.substr(0,5) == "TOPO_" &&
	definition.find(' ') == std::string::npos) {
      std::string trigger = definition.substr(0, definition.find('['));
      auto pos = std::find(labelsTopoEl.begin(),labelsTopoEl.end(),trigger);
      if (pos != labelsTopoEl.end()) {
	ATH_MSG_DEBUG("Found one CTP; ,CTPId: " << item.ctpId() << " ,Name: " << item.name() << " ,Definition: " << definition);
	unsigned index = std::distance(labelsTopoEl.begin(),pos);
	ctpIds[index]=item.ctpId();
      }
    }
  }

  return ctpIds;
}
