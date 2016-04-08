/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/MuonRoiChainFindTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"

#include "MuonTrigCoinData/TgcCoinDataContainer.h"

namespace Trigger {
  MuonRoiChainFindTool::MuonRoiChainFindTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t, n, p), m_chains(0), m_bunchCrossingId(-1) 
  {
    declareProperty("LVL1_ROIName", m_LVL1_ROIName="LVL1_ROI");
    declareProperty("MuCTPI_RDOName", m_MuCTPI_RDOName="MUCTPI_RDO");
    declareProperty("tgcCoinDataContainerName", m_tgcCoinDataContainerName="TrigT1CoinDataCollection"); 
    declareProperty("skipRoiThresholdComparison", m_skipRoiThresholdComparison=false); 
  }

  StatusCode MuonRoiChainFindTool::queryInterface(const InterfaceID& riid, void** ppvIf) {
    ATH_MSG_DEBUG("queryInterface()");
    
    if(IMuonRoiChainFindTool::interfaceID().versionMatch(riid)) {
      *ppvIf = dynamic_cast<IMuonRoiChainFindTool*>(this); 
      addRef();
      ATH_MSG_DEBUG("InterfaceID successfully matched with IMuonRoiChainFindTool one.");
      return StatusCode::SUCCESS;
    }

    return AthAlgTool::queryInterface(riid, ppvIf);
  }

  StatusCode MuonRoiChainFindTool::initialize() {
    ATH_MSG_DEBUG("initialize()");

    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonRoiChainFindTool::finalize() {
    ATH_MSG_DEBUG("finalize()");

    return StatusCode::SUCCESS;
  }
  
  StatusCode MuonRoiChainFindTool::find(std::vector<MuonRoiChain*>* chains) {
    ATH_MSG_DEBUG("find()");

    m_chains = chains;

    // Get Bunch Crossing Id 
    StatusCode sc = getBunchCrossingId();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not get BunchCrossingId in find()");
      return sc;
    }

    // Find MuonRoiChains which have Muon_ROI
    sc = createChainsWithMuon_ROI();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("createChainsWithMuon_ROI() failed in find()");
      return sc;
    }

    // Find MuonRoiChains which do no have Muon_ROI but do have MuCTPI_RDO dataWord
    sc = associateAndCreateChainsWithMuCTPI_RDO_dataWord();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("associateAndCreateChainsWithMuCTPI_RDO_dataWord() failed in find()");
      return sc;
    }

    // Find MuonRoiChains which have TGC Sector Logic only
    sc = associateAndCreateChainsWithTgcCoinData();
    if(sc.isFailure()) {
      ATH_MSG_FATAL("associateAndCreateChainsWithTgcCoinData() failed in find()");
      return sc;
    }

    return sc;
  }

  StatusCode MuonRoiChainFindTool::getBunchCrossingId() {
    const EventInfo* pevt = 0;
    StatusCode sc = evtStore()->retrieve(pevt);
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not retrieve EventInfo");
      return sc;
    }
    m_bunchCrossingId = pevt->event_ID()->bunch_crossing_id();

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainFindTool::createChainsWithMuon_ROI() {
    ATH_MSG_DEBUG("createChainsWithMuon_ROI()");

    const LVL1_ROI* lvl1Roi = 0;
    StatusCode sc = evtStore()->retrieve(lvl1Roi, m_LVL1_ROIName);
    if(sc.isFailure() || !lvl1Roi) {
      ATH_MSG_FATAL("Could not retrieve LVL1_ROI (" << m_LVL1_ROIName.c_str() << ")"); 
      return StatusCode::FAILURE;
    }

    const LVL1_ROI::muons_type* muonRoi = &(lvl1Roi->getMuonROIs());
    if(!muonRoi) {
      ATH_MSG_FATAL("Could not retrieve Muon_ROI");
      return StatusCode::FAILURE;
    }
    
    int roiIndex = -1;
    LVL1_ROI::muons_type::const_iterator it   = muonRoi->begin();
    LVL1_ROI::muons_type::const_iterator it_e = muonRoi->end();
    for(; it!=it_e; it++) {
      roiIndex++;
      const Muon_ROI *roi = &(*it);

      // Create a new MuonRoiChain 
      MuonRoiChain* chain = new MuonRoiChain;
      if(!chain) {
	ATH_MSG_FATAL("new MuonRoiChain failed in createChainsWithMuon_ROI(). This should never happen.");
        return StatusCode::FAILURE;
      }
      m_chains->push_back(chain);

      // Associate Muon_ROI to the newly created MuonRoiChain 
      bool isOK = chain->setMuon_ROI(roi, roiIndex);
      if(!isOK) showWARNINGforSetMuon_ROI(roi, roiIndex);
    }  

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainFindTool::associateAndCreateChainsWithMuCTPI_RDO_dataWord() {
    ATH_MSG_DEBUG("associateAndCreateChainsWithMuCTPI_RDO_dataWord()");

    const MuCTPI_RDO* muCTPI_RDO = 0;
    StatusCode sc = evtStore()->retrieve(muCTPI_RDO, m_MuCTPI_RDOName);
    if(sc.isFailure() || !muCTPI_RDO) {
      ATH_MSG_FATAL("Could not retrieve MuCTPI_RDO (" << m_MuCTPI_RDOName.c_str() << ")");
      return StatusCode::FAILURE;
    } 

    int dataWordIndex = -1;
    std::vector<uint32_t> dataWords = muCTPI_RDO->dataWord();
    std::vector<uint32_t>::const_iterator it   = dataWords.begin();
    std::vector<uint32_t>::const_iterator it_e = dataWords.end();
    for(; it!=it_e; ++it) {
      dataWordIndex++;  
      MuCTPI_DataWord_Decoder dataWordDecoder(*it);

      // MuCTPI_RDO holds data words up to +-5 buch crossing wide window. 
      // Its BCID is 3-bit information. 
      // We select a data word at proper timing. 
      if(m_bunchCrossingId%8!=dataWordDecoder.getBCID()) continue; 

      // Hemisphere
      // For Side A, Muon_ROI 0 (Muon_ROI::Positive) and MuCTPI_RDO dataWord 1  
      // For Side C, Muon_ROI 1 (Muon_ROI::Negative) and MuCTPI_RDO dataWord 0   
      Muon_ROI::Hemisphere hemisphere = (dataWordDecoder.getHemisphere()==1 ? Muon_ROI::Positive : Muon_ROI::Negative);  

      // Barrel  : Muon_ROI::Barrel  (0) and  MuCTPI_RDO::BARREL  (0)
      // Endcap  : Muon_ROI::Endcap  (1) and  MuCTPI_RDO::ENDCAP  (1)
      // Forward : Muon_ROI::Forward (2) and  MuCTPI_RDO::FORWARD (2)
      MuCTPI_RDO::SectorLocation sectorLocation = dataWordDecoder.getSectorLocation();
      Muon_ROI::RoISource source = (sectorLocation==MuCTPI_RDO::BARREL ? Muon_ROI::Barrel :
				    (sectorLocation==MuCTPI_RDO::ENDCAP ? Muon_ROI::Endcap :
				     Muon_ROI::Forward));

      bool isAssociated = false; 
      
      unsigned int sizeChains = m_chains->size();
      for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
	MuonRoiChain* chain = m_chains->at(iChain);

	// Sources (Barrel, Endcap, Forward) are required to be the same. 
	if(chain->getSource()!=source) continue; 
	// Hemispheres (Side A, Side C) are required to be the same. 
	if(chain->getHemisphere()!=hemisphere) continue; 

	// The MuonRoiChain should have Muon_ROI. 
	const Muon_ROI* muonRoi = chain->getMuon_ROI(); 
	if(!muonRoi) continue;
	
	if((dataWordDecoder.getSectorAddress()        != muonRoi->getSectorAddress())   || // Sector Address
	   (dataWordDecoder.getRoiNumber()            != muonRoi->getRoI())             || // ROI number
	   (!m_skipRoiThresholdComparison && 
	    dataWordDecoder.getPt()                   != muonRoi->getThrNumber())       || // Threshold number
	   (dataWordDecoder.getCandidateIsHighestPt() != muonRoi->isFirstCandidate())   || // First or Second candidate
	   (dataWordDecoder.getSectorOverflow()       != muonRoi->isMoreCandInSector()) || // More candidate in sector
	   (dataWordDecoder.getRoiOverflow()          != muonRoi->isMoreCandInRoI())       // More candidate in roi
	   ) { 
	  continue;
	}

	isAssociated  = chain->setMuCTPI_RDO_dataWord((*it), dataWordIndex);
	if(!isAssociated) showWARNINGforSetMuCTPI_RDO_dataWord((*it), dataWordIndex);
      }

      // If there is no corresponding MuonRoiChain yet, a new MuonRoiChain is created. 
      if(!isAssociated) {
	MuonRoiChain* chain = new MuonRoiChain;
	if(!chain) {
	  ATH_MSG_FATAL("new MuonRoiChain failed in associateAndCreateChainsWithMuCTPI_RDO_dataWord(). This should never happen.");
	  return StatusCode::FAILURE;
	}
	m_chains->push_back(chain);

	// Associate MuCTPI_RDO dataWord to the newly created MuonRoiChain
	bool isOK = chain->setMuCTPI_RDO_dataWord((*it), dataWordIndex);
	if(!isOK) showWARNINGforSetMuCTPI_RDO_dataWord((*it), dataWordIndex);
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonRoiChainFindTool::associateAndCreateChainsWithTgcCoinData() {
    ATH_MSG_DEBUG("associateAndCreateChainsWithTgcCoinData()");

    const Muon::TgcCoinDataContainer* coinColl = 0;
    StatusCode sc = evtStore()->retrieve(coinColl, m_tgcCoinDataContainerName); 
    if(sc.isFailure() || !coinColl) {
      ATH_MSG_FATAL("Could not retrieve Muon::TgcCoinDataContainer (" << m_tgcCoinDataContainerName.c_str() << ")");
      return StatusCode::FAILURE;
    } 

    Muon::TgcCoinDataContainer::const_iterator ic   = coinColl->begin();
    Muon::TgcCoinDataContainer::const_iterator ic_e = coinColl->end();
    for(; ic!=ic_e; ic++) {
      Muon::TgcCoinDataCollection::const_iterator icc   = (*ic)->begin();
      Muon::TgcCoinDataCollection::const_iterator icc_e = (*ic)->end();
      for(; icc!=icc_e; icc++) {
	if((*icc)->type()!=Muon::TgcCoinData::TYPE_SL) continue;

	Muon_ROI::RoISource source = ((*icc)->isForward() ? Muon_ROI::Forward : Muon_ROI::Endcap);
	Muon_ROI::Hemisphere hemisphere = ((*icc)->isAside() ? Muon_ROI::Positive : Muon_ROI::Negative);

	bool isAssociated = false; 
	
	unsigned int sizeChains = m_chains->size();
	for(unsigned int iChain=0; iChain<sizeChains; iChain++) {
	  MuonRoiChain* chain = m_chains->at(iChain);

	  // Sources (Barrel, Endcap, Forward) are required to be the same. 
	  if(chain->getSource()!=source) continue; 
	  // Hemispheres (Side A, Side C) are required to be the same. 
	  if(chain->getHemisphere()!=hemisphere) continue; 

	  uint32_t muCTPI_RDO_dataWord = chain->getMuCTPI_RDO_dataWord();
	  MuCTPI_DataWord_Decoder dataWordDecoder(muCTPI_RDO_dataWord); 

	  if(((*icc)->roi() != dataWordDecoder.getRoiNumber()) || // ROI number 
	     ((*icc)->pt()  != dataWordDecoder.getPt())           // Threshold number 
	     ) {
	    continue;
	  }

	  int tgcSLSectorID = (*icc)->phi();
	  if(!(*icc)->isForward()) { // Endcap
	    tgcSLSectorID +=  1;
	    tgcSLSectorID %= 48;
	  } else { // Forward
	    tgcSLSectorID %= 24;
	  }
	  if(dataWordDecoder.getSectorID() != tgcSLSectorID) continue; // Sector ID
	  
	  isAssociated = chain->setTgcCoinData(*icc);
	  if(!isAssociated) showWARNINGforSetTgcCoinData(*icc);
	}

	// If there is no corresponding MuonRoiChain yet, a new MuonRoiChain is created. 
	if(!isAssociated) { 
	  MuonRoiChain* chain = new MuonRoiChain; 
	  if(!chain) {
	    ATH_MSG_FATAL("new MuonRoiChain failed in associateAndCreateChainsWithTgcCoinData(). This should never happen.");
	    return StatusCode::FAILURE;
	  }
	  m_chains->push_back(chain);

	  // Associate TgcCoinData to the newly created MuonRoiChain
	  bool isOK = chain->setTgcCoinData(*icc);
	  if(!isOK) showWARNINGforSetTgcCoinData(*icc);
	}
      }
    }

    return StatusCode::SUCCESS;
  }

  void MuonRoiChainFindTool::showWARNINGforSetMuon_ROI(const Muon_ROI* muonRoi, int roiIndex) {
    ATH_MSG_WARNING("setMuon_ROI failed. const Muon_ROI* " << muonRoi <<
		    " and/or int " << roiIndex <<
		    " are wrong."); 
  }

  void MuonRoiChainFindTool::showWARNINGforSetMuCTPI_RDO_dataWord(uint32_t muctpiRdoDataWord, int dataWordIndex) {
    ATH_MSG_WARNING("setMuCTPI_RDO_dataWord failed. uint32_t " << muctpiRdoDataWord <<
		    " and/or int " << dataWordIndex <<
		    " are wrong."); 
  }

  void MuonRoiChainFindTool::showWARNINGforSetTgcCoinData(const Muon::TgcCoinData* tgcSL) { 
    ATH_MSG_WARNING("setTgcCoinData failed. const Muon::TgcCoinData* tgcSL " << tgcSL << 
		    " is wrong."); 
  }

} // end of namespace Trigger
