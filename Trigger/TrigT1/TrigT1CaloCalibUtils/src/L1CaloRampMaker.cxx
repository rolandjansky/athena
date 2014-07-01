/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>

#include "AthenaKernel/errorcheck.h"

#include "TrigT1CaloCalibUtils/L1CaloRampMaker.h"
// #include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TrigT1CaloCalibConditions/L1CaloRampData.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
// #include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
// #include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"

L1CaloRampMaker::L1CaloRampMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_triggerTowerCollectionName("TriggerTowers"),
    m_caloCellContainerName("AllCalo"),
    m_l1CaloCells2TriggerTowersToolName("L1CaloCells2TriggerTowers"),
    m_outputFolderName("/L1CaloRampMaker/L1CaloRampDataContainer"),
    m_gainStrategyFolder("/TRIGGER/Receivers/Conditions/Strategy"),
    m_doLAr(false),
    m_doTile(false),
    m_isGain1(true),
    m_isOvEmb(false),
    m_isOvEmec(false),
    m_isFcalLowEta(false),
    m_isFcalHighEta(false),
    m_checkProvenance(false),
    m_nEventsPerStep(200),
    m_nSteps(9),
    m_fadcSaturationCut(963),
    m_tileSaturationCut(150.),
    m_ttTool("LVL1::L1TriggerTowerTool/LVL1::L1TriggerTowerTool"),
    m_beamInfo("TileBeamInfoProvider/TileBeamInfoProvider"),
    m_log(msgSvc(), name),
    m_nEvent(1),
    m_firstEvent(true),
    m_storeGate(0),
    m_detStore(0),
    m_condSvc(0),
    m_lvl1Helper(0),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers"),
    m_jmTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_rampDataContainer(0),
    m_pprLutContainer(0),
    m_pprDisabledChannelContainer(0),
    m_larEnergy("LVL1::L1CaloLArTowerEnergy/L1CaloLArTowerEnergy")
{
    declareProperty("TriggerTowerCollectionName", m_triggerTowerCollectionName);
    declareProperty("CaloCellContainerName", m_caloCellContainerName);
    declareProperty("L1CaloCells2TriggerTowersToolName", m_l1CaloCells2TriggerTowersToolName);
    declareProperty("OutputFolderName", m_outputFolderName);
    declareProperty("GainStrategyFolder", m_gainStrategyFolder);
    declareProperty("DoLAr", m_doLAr);
    declareProperty("DoTile", m_doTile);
    declareProperty("IsGain1", m_isGain1);
    declareProperty("IsOvEmb", m_isOvEmb);
    declareProperty("IsOvEmec", m_isOvEmec);
    declareProperty("IsFcalLowEta", m_isFcalLowEta);
    declareProperty("IsFcalHighEta", m_isFcalHighEta);
    declareProperty("CheckProvenance", m_checkProvenance);
    declareProperty("EventsPerEnergyStep", m_nEventsPerStep);
    declareProperty("NumberOfEnergySteps", m_nSteps);
    declareProperty("FADCSaturationCut", m_fadcSaturationCut);
    declareProperty("TileSaturationCut", m_tileSaturationCut);
    declareProperty("L1TriggerTowerTool", m_ttTool);
    declareProperty("TileBeamInfoProvider", m_beamInfo);
    declareProperty("SpecialChannelRange", m_specialChannelRange);
}

L1CaloRampMaker::~L1CaloRampMaker() 
{
}

StatusCode L1CaloRampMaker::initialize()
{
    StatusCode sc;
    
    sc = service("StoreGateSvc", m_storeGate);
    if(sc.isFailure()) {
        m_log << MSG::FATAL << "Cannot access StoreGateSvc!" << endreq;
        return sc;
    }

    sc = service("DetectorStore", m_detStore);
    if(sc.isFailure()) {
        m_log << MSG::FATAL << "Cannot access DetectorStore!" << endreq;
        return sc;
    }
    
    sc = service("L1CaloCondSvc", m_condSvc);
    if(sc.isFailure()) {
        m_log << MSG::FATAL << "Cannot access L1CaloCondSvc!" << endreq;
        return sc;
    }

    const CaloIdManager *caloMgr;
    sc = m_detStore->retrieve(caloMgr);
    if(sc.isFailure()) {
	m_log << MSG::FATAL << "Cannot retrieve CaloIdManager from DetectorStore." << endreq;
	return sc;
    }
    m_lvl1Helper = caloMgr->getLVL1_ID();
    if(!m_lvl1Helper) {
	m_log << MSG::FATAL << "Cannot access CaloLVL1_ID helper." << endreq;
	return StatusCode::FAILURE;
    }

    sc = m_cells2tt.retrieve();
    if(sc.isFailure()){
      m_log << MSG::FATAL << "Cannot retrieve L1CaloCells2TriggerTowers" << endreq;
     return sc;
    }

    sc = m_larEnergy.retrieve();
    if(sc.isFailure()){
      m_log << MSG::FATAL << "Cannot retrieve L1CaloLArTowerEnergy" << endreq;
     return sc;
    }

    sc = m_jmTools.retrieve();
    if(sc.isFailure()){
      m_log << MSG::FATAL << "Cannot retrieve L1CaloOfflineTriggerTowerTools" << endreq;
     return sc;
    }    
    
//     IToolSvc *toolSvc;
//     IAlgTool* algTool;
//     sc = service("ToolSvc", toolSvc);
//     if(sc.isFailure()) {
// 	m_log << MSG::FATAL << "Cannot retrieve ToolSvc." << endreq;
// 	return sc;
//     }

//     sc = toolSvc->retrieveTool("L1CaloCells2TriggerTowers", algTool);
//     m_cells2tt = dynamic_cast<L1CaloCells2TriggerTowers*>(algTool);
//     if(sc.isFailure() || !m_cells2tt) {
// 	m_log << MSG::FATAL << "Cannot retrieve L1CaloCells2TriggerTowers from ToolSvc." << endreq;
// 	return sc;
//     }

//     sc = toolSvc->retrieveTool("L1CaloLArTowerEnergy", algTool);
//     m_larEnergy = dynamic_cast<L1CaloLArTowerEnergy*>(algTool);
//     if(sc.isFailure() || !m_larEnergy) {
//         m_log << MSG::FATAL << "Cannot retrieve L1CaloLArTowerEnergy from ToolSvc." << endreq;
//         return StatusCode::FAILURE;
//     }

//     sc = toolSvc->retrieveTool("TriggerTowerTools",algTool);
//     m_jmTools = dynamic_cast<TriggerTowerTools*>(algTool);
//     if(sc.isFailure() || !m_jmTools) {
//         m_log << MSG::FATAL << "Could not retrieve TriggerTowerTools" <<endreq;
// 	return sc;
//     }

    // setup access to Calib1 folder of L1CALO database
    m_pprLutContainerFolderMap.insert(
	 std::pair<L1CaloPprLutContainer::eCoolFolders, std::string>(L1CaloPprLutContainer::ePprLutChanCalib,
								     "/TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib"));
    m_pprDisabledChannelContainerFolderMap.insert(std::pair<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>(L1CaloPprDisabledChannelContainer::ePpmDeadChannels, "/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"));
    m_pprDisabledChannelContainerFolderMap.insert(std::pair<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>(L1CaloPprDisabledChannelContainer::ePprChanCalib, "/TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib"));
    m_pprDisabledChannelContainerFolderMap.insert(std::pair<L1CaloPprDisabledChannelContainer::eCoolFolders, std::string>(L1CaloPprDisabledChannelContainer::eDisabledTowers, "/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"));

    return StatusCode::SUCCESS;
}

StatusCode L1CaloRampMaker::execute()
{

    // Skip spurious events

    unsigned int wantedEvents = m_nEventsPerStep*m_nSteps;
    if (m_nEvent > wantedEvents) {
        if (m_nEvent == wantedEvents+1) {
	    m_log << MSG::INFO << "Processed " << wantedEvents
	          << " events, skipping the rest" << endreq;
        }
	++m_nEvent;
        return StatusCode::SUCCESS;
    }
    unsigned int evtNumber = 0;
    const EventInfo* evInfo = 0; 
    const EventID* evID = 0;
    StatusCode sc = m_storeGate->retrieve(evInfo);
    if (sc.isFailure()) {
        m_log << MSG::WARNING << "No EventInfo found" << endreq;
    } else {
        evID = evInfo->event_ID();
        if (evID) evtNumber = evID->event_number();
    }
    if (evtNumber == 0 && m_nEvent > 1) { // Only allow event 0 as first event
        m_log << MSG::WARNING << "Skipping spurious event number 0" << endreq;
	return StatusCode::SUCCESS;
    }


    const TriggerTowerCollection *triggerTowerCollection(0);
    sc = m_storeGate->retrieve(triggerTowerCollection, m_triggerTowerCollectionName);
    if(!sc.isSuccess()) {
        m_log << MSG::ERROR << "Cannot retrieve TriggerTowerCollection '"
              << m_triggerTowerCollectionName << "' from StoreGate." << endreq;
        return StatusCode::RECOVERABLE;
    }

    const CaloCellContainer *caloCellContainer(0);
    sc = m_storeGate->retrieve(caloCellContainer, m_caloCellContainerName);
    if(!sc.isSuccess()) {
	m_log << MSG::ERROR << "Cannot retrieve CaloCellContainer '" << m_caloCellContainerName
	    << "'." << endreq;
	return StatusCode::RECOVERABLE;
    }

    // init trigger tower to cell mapping - needed each event?
    // not needed - done by m_larEnergy-->init...
    if(!m_cells2tt->initCaloCellsTriggerTowers(*caloCellContainer)) {
        m_log << MSG::ERROR << "Can not initialize L1CaloCells2TriggerTowers with CaloCellContainer '"
              << m_caloCellContainerName << "." << endreq;
        return StatusCode::RECOVERABLE;
    }

    // access L1CaloPPrLutContinaer
    // since no key is given, we will override the default lookup
    // to the folder specified in initialize (i.e. the .../Calib1/PprChanCalib version)
    CHECK( m_condSvc->retrieve(m_pprLutContainer,
			       m_pprLutContainerFolderMap) );
    CHECK (m_condSvc->retrieve(m_pprDisabledChannelContainer,
			       m_pprDisabledChannelContainerFolderMap) );

    // init larTowerEnergy tool
    /*
    if(!m_larEnergy->initL1CaloLArTowerEnergy(*caloCellContainer, *triggerTowerCollection)) {
        m_log << MSG::ERROR << "Can not initialize L1CaloLArTowerEnergy with CaloCellContainer '"
              << m_caloCellContainerName
              << " and TriggerTowerCollection "
              << m_triggerTowerCollectionName
              << "." << endreq;
        return StatusCode::RECOVERABLE;
    }
    */

    if(m_firstEvent) {
      unsigned int runNumber = 0;
      if (evID) runNumber = evID->run_number();
      std::string gainStrategy("");
      const CondAttrListCollection* gainStrategyColl = 0;
      sc = m_detStore->retrieve(gainStrategyColl, m_gainStrategyFolder);
      if(sc.isSuccess()) {
        CondAttrListCollection::const_iterator itr  = gainStrategyColl->begin();
	CondAttrListCollection::const_iterator itrE = gainStrategyColl->end();
	bool consistent(true);
	for (; itr != itrE; ++itr) {
	  const unsigned int channel = itr->first;
          if (channel != 1 && channel != 2) continue;
	  const AthenaAttributeList& attrList(itr->second);
          const std::string strategy(attrList["name"].data<std::string>());
          const std::string status(attrList["status"].data<std::string>());
	  m_log << MSG::INFO << "Gain Strategy: channel = " << channel
	                     << ", name = " << strategy
		             << ", status = " << status << endreq;
	  if (gainStrategy == "") gainStrategy = strategy;
          else if (gainStrategy != strategy) consistent = false;
        }
        std::string newStrategy("");
        if (runNumber == 219978) newStrategy = "GainOneOvEmecFcalLowEta";
        if (runNumber == 219979) newStrategy = "GainOneOvEmbFcalHighEta";
        if (runNumber == 219980) newStrategy = "CalibGainsEt";
        if (runNumber == 220013) newStrategy = "GainOneOvEmecFcalLowEta";
        if (runNumber == 220014) newStrategy = "GainOneOvEmbFcalHighEta";
        if (runNumber == 220015) newStrategy = "CalibGainsEt";
        if (runNumber == 222871) newStrategy = "GainOne";
        if (runNumber == 222872) newStrategy = "GainOne";
        if (runNumber == 222873) newStrategy = "GainOne";
        if (runNumber == 223073) newStrategy = "GainOne";
        if (runNumber == 223074) newStrategy = "GainOne";
        if (runNumber == 223075) newStrategy = "GainOne";
        if (newStrategy != "") {
          m_log << MSG::INFO << "Changing Gain Strategy to " << newStrategy
                             << endreq;
          gainStrategy = newStrategy;
        }
	if (gainStrategy != "" && consistent) {
	  m_isGain1 = (gainStrategy.find("GainOne") != std::string::npos);
	  m_isOvEmb = (gainStrategy.find("OvEmb") != std::string::npos);
	  m_isOvEmec = (gainStrategy.find("OvEmec") != std::string::npos);
	  m_isFcalLowEta = (gainStrategy.find("FcalLowEta") != 
	                                                    std::string::npos);
	  m_isFcalHighEta = (gainStrategy.find("FcalHighEta") !=
	                                                    std::string::npos);
	} else if (gainStrategy == "") {
	  m_log << MSG::WARNING << "Gain Strategy collection empty" << endreq;
	} else {
	  m_log << MSG::WARNING << "Inconsistent Gain Strategies" << endreq;
	}
      } else {
        m_log << MSG::WARNING << "No Gain Strategy collection found" << endreq;
      }
      m_log << MSG::INFO << "isGain1 = "         << m_isGain1
                         << ", isOvEmb = "       << m_isOvEmb
                         << ", isOvEmec = "      << m_isOvEmec
                         << ", isFcalLowEta = "  << m_isFcalLowEta
                         << ", isFcalHighEta = " << m_isFcalHighEta << endreq;

      //unsigned int runNumber = 0;
      //if (evID) runNumber = evID->run_number();

      setupRampDataContainer(triggerTowerCollection);
      m_rampDataContainer->setRunNumber(runNumber);
      m_rampDataContainer->setGainStrategy(gainStrategy);
      m_firstEvent = false;
    }

    LVL1::TriggerTower *tt;
    TriggerTowerCollection::const_iterator p_itTT = triggerTowerCollection->begin();
    TriggerTowerCollection::const_iterator p_itTTEnd = triggerTowerCollection->end();
    Identifier id;
    unsigned int coolId;
    std::vector<int>::const_iterator max;
    std::map<int, int>::iterator m_specialChannelIt,
      m_specialChannelRangeEnd(m_specialChannelRange.end());
    double caloEnergy, level1Energy;
    bool nextStep = (m_nEvent % m_nEventsPerStep == 0);
    bool isSaturated(false);

    for(;p_itTT != p_itTTEnd; ++p_itTT) {
        tt = *p_itTT;

        // electromagnetic sampling
	isSaturated = tt->isEMSaturated();
	if(!isSaturated) {
	    // isSaturated flag is not enough to check - test FADC for saturation, too
	    max = std::max_element(tt->emADC().begin(), tt->emADC().end());
	    if(*max >= m_fadcSaturationCut) isSaturated = true; // take ~ 2 times pedestal safety margin
	}
        if(m_doLAr && !isSaturated) {
            id = m_ttTool->identifier(tt->eta(), tt->phi(), 0);
            coolId = m_ttTool->channelID(id).id();

            if(!m_ttTool->disabledChannel(coolId)) {
		if(m_checkProvenance) checkProvenance(id, coolId);
                level1Energy = getTriggerTowerEnergy(tt->emADC(), id, coolId, tt->eta());
		caloEnergy = getCaloEnergy(id, tt);

		// see if we have a special channel and cut on level1 energy
		m_specialChannelIt = m_specialChannelRange.find(coolId);
		if(m_specialChannelIt == m_specialChannelRangeEnd || level1Energy < m_specialChannelIt->second) {
		    m_rampDataContainer->rampData(coolId)->addData(caloEnergy, level1Energy);
		} else {
		    // make sure we don't accidently miss the threshold again
		    m_specialChannelIt->second = -1000;
		}
		if(nextStep) m_rampDataContainer->rampData(coolId)->nextStep();
            }
        }

        // hadronic sampling
	if(tt->isHadSaturated()) {
	    continue;
	} else {
	    // isSaturated flag is not enough to check - test FADC for saturation, too
	    max = std::max_element(tt->hadADC().begin(), tt->hadADC().end());
	    if(*max >= m_fadcSaturationCut) continue; // take ~ 2 times pedestal safety margin
	}
 
        id = m_ttTool->identifier(tt->eta(), tt->phi(), 1);
        coolId = m_ttTool->channelID(id).id();

	if(m_ttTool->disabledChannel(coolId)) continue;

	if(m_checkProvenance) checkProvenance(id, coolId);
        if((m_doTile && m_lvl1Helper->is_tile(id)) || (m_doLAr && !m_lvl1Helper->is_tile(id))) {
	   caloEnergy = getCaloEnergy(id, tt);
           level1Energy = getTriggerTowerEnergy(tt->hadADC(), id, coolId, tt->eta());

	   // cut on 150 GeV ADC for Tile, lots of saturating calibration signals
	   if(m_lvl1Helper->is_tile(id) && level1Energy > m_tileSaturationCut) continue;
	   
	   // see if we have a special channel and cut on level1 energy
	   m_specialChannelIt = m_specialChannelRange.find(coolId);
	   if(m_specialChannelIt == m_specialChannelRangeEnd || level1Energy < m_specialChannelIt->second) {
	       m_rampDataContainer->rampData(coolId)->addData(caloEnergy, level1Energy);
	   } else {
	       // make sure we don't accidently miss the threshold again
	       m_specialChannelIt->second = -1000;
	   }
           if(nextStep) m_rampDataContainer->rampData(coolId)->nextStep();
        }
    }

    ++m_nEvent;

    return StatusCode::SUCCESS;
}

void L1CaloRampMaker::setupRampDataContainer(const TriggerTowerCollection* triggerTowerCollection)
{
    m_rampDataContainer = new L1CaloRampDataContainer();
    
    TriggerTowerCollection::const_iterator p_itTT = triggerTowerCollection->begin();
    TriggerTowerCollection::const_iterator p_itTTEnd = triggerTowerCollection->end();
    L1CaloRampData rd;
    unsigned int coolId;
    for(;p_itTT != p_itTTEnd; ++p_itTT) {
        if(m_doLAr) {
            // layer 0 is always LAr
            coolId = m_ttTool->channelID((*p_itTT)->eta(), (*p_itTT)->phi(), 0).id();
            m_rampDataContainer->addRampData(coolId, rd);

            if(!m_lvl1Helper->is_tile(m_ttTool->identifier((*p_itTT)->eta(), (*p_itTT)->phi(), 1))) {
                coolId = m_ttTool->channelID((*p_itTT)->eta(), (*p_itTT)->phi(), 1).id();
                m_rampDataContainer->addRampData(coolId, rd);
            }
        }
        
        if(m_doTile && m_lvl1Helper->is_tile(m_ttTool->identifier((*p_itTT)->eta(), (*p_itTT)->phi(), 1))) {
            // layer 1 in barrel region is tile
            coolId = m_ttTool->channelID((*p_itTT)->eta(), (*p_itTT)->phi(), 1).id();
            m_rampDataContainer->addRampData(coolId, rd);
        }
    }            
}

// return E_T of cells comprising the trigger tower with @c id
double L1CaloRampMaker::getCaloEnergy(const Identifier& id, const LVL1::TriggerTower* tt)
{
    double et(0.);
    double eta = tt->eta();
    if (m_lvl1Helper->is_fcal(id)) {
	if (m_lvl1Helper->sampling(id) == 0) { // em
            et = m_jmTools->emTTCellsEt(tt);
	} else {          // had
	    std::vector<float> etRec = m_jmTools->hadTTCellsEtByReceiver(tt);
	    if (etRec.size() == 2) {
	        if      (m_isFcalLowEta)  et = etRec[0];
		else if (m_isFcalHighEta) et = etRec[1];
	        else                      et = etRec[0] + etRec[1];
            } else if (etRec.size() == 1) et = etRec[0];
	    //double etall = m_jmTools->hadTTCellsEt(tt);
	    //std::cout << "Fcal eta/phi: " << eta << "/" << tt->phi() << " receivers = " << etRec.size()
	    //          << " et = " << et <<  " etall = " << etall << std::endl;
        }
    } else {
	if (m_lvl1Helper->is_barrel_end(id) && (m_isOvEmb || m_isOvEmec)) {
	    std::vector<float> energyRec = m_jmTools->emTTCellsEnergyByReceiver(tt);
	    if (energyRec.size() == 2) {
	        double energy(0.);
		if (m_isOvEmec) energy = energyRec[0];
		else            energy = energyRec[1];
		et = energy / std::cosh(eta);
		//std::cout << "Et at eta/phi: " << eta << "/" << tt->phi() << " is " << et
		//          << " Et other: " << ((eta > 0.) ? energyRec[1] : energyRec[0])/std::cosh(eta)
                //          << " Et all: " << m_larEnergy->EtLArg(id);
            //} else et = m_larEnergy->EtLArg(id);
            } else et = energyRec[0] / std::cosh(eta);
	} else {
	    //if (m_lvl1Helper->is_tile(id)) {
	    if (m_lvl1Helper->is_tile(id) || m_lvl1Helper->is_hec(id)) {
	        et = m_jmTools->hadTTCellsEnergy(tt) / std::cosh(eta);
	    //} else et = m_larEnergy->EtLArg(id); // includes missing FEB correction
	    } else et = m_jmTools->emTTCellsEnergy(tt) / std::cosh(eta);
	}
    }
    return et;
}

// calculate trigger tower E_T from adc values
// if run is with gain 1 loaded, need special treatment of E_T for HEC and TILE
double L1CaloRampMaker::getTriggerTowerEnergy(const std::vector<int>& adc, const Identifier& id, unsigned int coolId, double eta) {
    // calibration is against ADC energy "(adc peak value - pedestal) / 4"

    std::vector<int>::const_iterator max = std::max_element(adc.begin(), adc.end());
    const L1CaloPprLut *pprLut = m_pprLutContainer->pprLut(coolId);
    double energy = (*max - int(pprLut->pedValue())) * 0.25;
    if (energy < 0.) energy = 0.;
    //if (m_lvl1Helper->is_barrel_end(id)) {
    //    std::cout << " TT Et: " << energy << " maxADC: " << *max << " pedVal: " << pprLut->pedValue() << std::endl;
    //}

    // no corrections if gain is set correctly
    if(!m_isGain1) return energy;
    
    // correct hec and tile if gain 1
    if(m_lvl1Helper->is_hec(id)) {
        energy /= std::cosh(eta);
        if(fabs(eta) > 2.5) {
            energy *= 10.;
        } else if(fabs(eta) > 1.9) {
            energy *= 5.;
        } else {
            energy *= 3.125;
        }
    } else if(m_lvl1Helper->is_tile(id)) {
        energy /= std::cosh(eta);
    }

    return energy;
}

// function fills a map of cooldid -> (number of cells failing quality, avg. cell timing)
// used for fcal debugging
void L1CaloRampMaker::checkProvenance(const Identifier& ttId, unsigned int coolId) {
    const std::vector<const CaloCell*>& cells(m_cells2tt->caloCells(ttId));
    unsigned int s(cells.size());
    std::map<unsigned int, std::pair<unsigned int, double> >::iterator it;
    for(unsigned int i = 0; i < s; ++i) {
	it = m_mapBadOFCIteration.find(coolId);
	if(it == m_mapBadOFCIteration.end()) {
	    m_mapBadOFCIteration.insert(
              std::pair<unsigned int, std::pair<unsigned int, double> >(coolId,
			std::pair<unsigned int, double>(!((cells[i])->provenance() & 0x100),
							(cells[i]->time()/double(s)))));
	    continue;
	}
	if(!((cells[i])->provenance() & 0x100)) {
	    it->second.first += 1;
	}
	it->second.second += (cells[i]->time()/double(s));
    }
}

StatusCode L1CaloRampMaker::finalize()
{
    StatusCode sc;

    sc = m_detStore->record(m_rampDataContainer, m_outputFolderName);
    if(sc.isFailure()) {
        m_log << MSG::FATAL << "Cannot record L1CaloRampDataContainer in DetectorStore" << endreq;
        return sc;
    }

    if(m_checkProvenance) {
	std::ofstream bad_ofciter("bad-ofciter.txt");
	std::map<unsigned int, std::pair<unsigned int, double> >::const_iterator it(m_mapBadOFCIteration.begin()), itE(m_mapBadOFCIteration.end());
	for(;it != itE; ++it) {
	    bad_ofciter << "0x" << std::hex << it->first << " "
			<< std::dec << double(it->second.first) / double(m_nEvent)
			<< " " << it->second.second / double(m_nEvent) << std::endl;
	}
	bad_ofciter.close();
    }

    return StatusCode::SUCCESS;
}

