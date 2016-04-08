/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/MuonRoiChainSvc.h"

#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/IIncidentSvc.h"

#include "MuonTrigCoinData/TgcCoinData.h"

namespace Trigger {

  MuonRoiChainSvc::MuonRoiChainSvc(const std::string& name, ISvcLocator* svc) : 
    AthService(name, svc),
    m_findTool("Trigger::MuonRoiChainFindTool"),
    m_debug(false), m_newEvent(false) 
  {
  }
  
  MuonRoiChainSvc::~MuonRoiChainSvc(void) 
  {}

  StatusCode MuonRoiChainSvc::queryInterface(const InterfaceID& riid,
						 void** ppvIF) 
  {
    if(IIncidentListener::interfaceID().versionMatch(riid)) { 
      *ppvIF = dynamic_cast<IIncidentListener*>(this); 
    } else if(IMuonRoiChainSvc::interfaceID().versionMatch(riid)) {
      *ppvIF = dynamic_cast<IMuonRoiChainSvc*>(this); 
    } else {
      return AthService::queryInterface(riid, ppvIF);
    }

    addRef();
    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainSvc::initialize(void) {
    m_debug = outputLevel() <= MSG::DEBUG;

    ATH_MSG_DEBUG("initialize()");

    // Retrieve MuonRoiChainFindTool 
    StatusCode sc = m_findTool.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("IMuonRoiChainFindTool could not be retrieved");
      return sc;
    }

    // Retrieve IncidentSvc 
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name()); 
    sc = incidentSvc.retrieve(); 
    if(sc.isFailure()) { 
      ATH_MSG_FATAL("IncidentSvc not found.");
      return StatusCode::FAILURE; 
    } 
    // BeginEvent incident is registered 
    incidentSvc->addListener(this, IncidentType::BeginEvent);
    
    return sc;
  }

  StatusCode MuonRoiChainSvc::finalize(void) {
    ATH_MSG_DEBUG("finalize()");
 
    // Delete memories 
    prepareForNewEvent();

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainSvc::execute(void) {
    ATH_MSG_DEBUG("execute()");

    if(!m_newEvent) {
      // This event is already processed and MuonRoiChain's are already found 
      ATH_MSG_DEBUG("This is not a new event.");
      return StatusCode::SUCCESS; 
    }

    // Delete memories 
    prepareForNewEvent();

    // Get MuonRoiChains
    StatusCode sc = m_findTool->find(&m_chainVector);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("IMuonRoiChainFindTool->find() failed.");
      return StatusCode::FAILURE;
    }
    
    //if(m_debug) showHierarchyVector(&hierarchyVector);
      
    ATH_MSG_DEBUG("m_hierarchyVector size is " << m_chainVector.size());

    // The search of MuonRoiChain's for this event has been done 
    m_newEvent = false;
    
    return sc;
  }

  StatusCode MuonRoiChainSvc::getChainWithRoiIndex(const MuonRoiChain** chain, int roiIndex) {
    *chain = 0;

    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
	ATH_MSG_FATAL("execute() failed in " << 
		      "MuonRoiChainSvc::getChainWithRoiIndex(const MuonRoiChain* chain, int roiIndex)");
	return sc;
      }
    }

    unsigned int sizeChains = m_chainVector.size();
    for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
      if(m_chainVector.at(iChain)->getMuon_ROIIndex()==roiIndex) {
	*chain = m_chainVector.at(iChain);
	break;
      }
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainSvc::getChain(const MuonRoiChain** chain, const Muon_ROI* muonRoi) {
    *chain = 0;

    if(!muonRoi) {
      ATH_MSG_WARNING("const Muon_ROI* muonRoi is null pointer");
      return StatusCode::SUCCESS;
    }
    uint32_t roiWord = muonRoi->getROIWord();

    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
	ATH_MSG_FATAL("execute() failed in " << 
		      "MuonRoiChainSvc::getChain(const MuonRoiChain* chain, const Muon_ROI* muonRoi)");
	return sc;
      }
    }

    unsigned int sizeChains = m_chainVector.size();
    for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
      const Muon_ROI* tmpMuonRoi = m_chainVector.at(iChain)->getMuon_ROI();
      if(tmpMuonRoi && tmpMuonRoi->getROIWord()==roiWord) {
        *chain = m_chainVector.at(iChain);
        break;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainSvc::getChain(const MuonRoiChain** chain, uint32_t muctpiRdoDataWord) {
    *chain = 0;

    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
	ATH_MSG_FATAL("execute() failed in " << 
		      "MuonRoiChainSvc::getChain(const MuonRoiChain* chain, uint32_t muctpiRdoDataWord)");
	return sc;
      }
    }

    unsigned int sizeChains = m_chainVector.size();
    for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
      if(m_chainVector.at(iChain)->getMuCTPI_RDO_dataWord()==muctpiRdoDataWord) {
        *chain = m_chainVector.at(iChain);
        break;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainSvc::getChain(const MuonRoiChain** chain, const Muon::TgcCoinData* coin) {
    *chain = 0;

    if(!coin) {
      ATH_MSG_WARNING("const TgcCoinData* coin is null pointer");
      return StatusCode::SUCCESS;
    }
    if(coin->type()!=Muon::TgcCoinData::TYPE_SL) {
      ATH_MSG_WARNING("const TgcCoinData* coin is not Sector Logic");
      return StatusCode::SUCCESS;
    }

    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
	ATH_MSG_FATAL("execute() failed in " << 
		      "MuonRoiChainSvc::getChain(const MuonRoiChain* chain, uint32_t muctpiRdoDataWord)");
	return sc;
      }
    }

    unsigned int sizeChains = m_chainVector.size();
    for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
      const Muon::TgcCoinData* tmpCoin = m_chainVector.at(iChain)->getTgcCoinData(); 
      if(tmpCoin                                                 && 
	 coin->isAside()          == tmpCoin->isAside()          &&
	 coin->isForward()        == tmpCoin->isForward()        &&
	 coin->phi()              == tmpCoin->phi()              &&
	 coin->channelIdOut()     == tmpCoin->channelIdOut()     &&
	 coin->identifyHash()     == tmpCoin->identifyHash()     &&
	 coin->trackletId()       == tmpCoin->trackletId()       &&
	 coin->trackletIdStrip()  == tmpCoin->trackletIdStrip()  &&
	 coin->roi()              == tmpCoin->roi()              &&
	 coin->pt()               == tmpCoin->pt()               &&
	 coin->isPositiveDeltaR() == tmpCoin->isPositiveDeltaR()
	 ) {
	*chain = m_chainVector.at(iChain);
        break;
      } 
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonRoiChainSvc::getChains(std::vector<const MuonRoiChain*>* chains) {
    if(!chains) {
      ATH_MSG_FATAL("Given std::vector<const MuonRoiChain*>* pointer is NULL. getChains failed.");
      return StatusCode::FAILURE;
    }
    // We don't need old information 
    chains->clear();
    
    // If this call is the first time for this event, execute is performed 
    if(m_newEvent) {
      StatusCode sc = execute();
      if(sc.isFailure()) {
	ATH_MSG_FATAL("execute() failed in " << 
		      "MuonRoiChainSvc::getChaisn(std::vector<const MuonRoiChain*>* chains)");
	return sc;
      }
    }

    unsigned int sizeChains = m_chainVector.size();
    for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
      chains->push_back(m_chainVector.at(iChain));
    }

    return StatusCode::SUCCESS;
  }

  void MuonRoiChainSvc::handle(const Incident& inc) { 
    // When processing for a new run is started, m_newEvent will become true   
    if(inc.type()==IncidentType::BeginEvent) { 
      ATH_MSG_DEBUG("handle() BeginEvent");
    }

    m_newEvent = true;
  }

  void MuonRoiChainSvc::prepareForNewEvent() {
    // Delete MuonRoiChain's in m_hierarchyVector
    while(!m_chainVector.empty()) {
      delete m_chainVector.back();
      m_chainVector.pop_back();
    }
  }

} // end of namespace Trigger
