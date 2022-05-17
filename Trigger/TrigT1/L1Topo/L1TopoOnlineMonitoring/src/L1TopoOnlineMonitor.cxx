/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "L1TopoOnlineMonitor.h"

// Trigger includes
#include "xAODTrigger/L1TopoSimResults.h"

// System includes
#include <utility>


// =============================================================================
// Standard constructor
// =============================================================================
L1TopoOnlineMonitor::L1TopoOnlineMonitor(const std::string& name, ISvcLocator* svcLoc)
: AthMonitorAlgorithm(name, svcLoc)
{}

// =============================================================================
// Implementation of AthReentrantAlgorithm::initialize
// =============================================================================
StatusCode L1TopoOnlineMonitor::initialize() {

  ATH_CHECK(m_l1topoKey.initialize());
  ATH_CHECK(m_monTool.retrieve(DisableTool{m_monTool.name().empty()}));

  return AthMonitorAlgorithm::initialize();
}


StatusCode L1TopoOnlineMonitor::fillHistograms( const EventContext& ctx ) const {

  //SG::ReadHandle<xAOD::L1TopoSimResultsContainer> cont(isLegacy? m_legacyL1topoKey : m_l1topoKey);
  SG::ReadHandle<xAOD::L1TopoSimResultsContainer> cont(m_l1topoKey, ctx);
  if(!cont.isValid()){
    ATH_MSG_FATAL("Could not retrieve EDM Container");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("----got L1Topo container: " << cont.key());

  for(const auto l1topo_dec : * cont){
    ATH_MSG_DEBUG( "Reading L1Topo EDM:: Connection ID: " << l1topo_dec->connectionId() << " Clock: " << l1topo_dec->clock() << " Bit-length: " << l1topo_dec->bitWidth() << " Word: " << l1topo_dec->topoWord() << " Word64: " << l1topo_dec->topoWord64() );
    
    std::vector<unsigned> topoword;

    if (l1topo_dec->bitWidth() == 32) {
      for(unsigned int i=0; i<32; ++i) {
	uint32_t mask = 0x1; mask <<= i;
	if ((l1topo_dec->topoWord() & mask) !=0)
	  {topoword.push_back(32*l1topo_dec->clock()+i);}
      }
      std::string name = "CableElec_";
      name += std::to_string(l1topo_dec->connectionId());
      auto monTopoDec = Monitored::Collection(name, topoword);
      Monitored::Group(m_monTool,monTopoDec);
    }
    else if (l1topo_dec->bitWidth() == 64) {
      for(unsigned int i=0; i<64; ++i) {
	uint64_t mask = 0x1; mask <<= i;
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
    }
  }
  

  return StatusCode::SUCCESS;
}
