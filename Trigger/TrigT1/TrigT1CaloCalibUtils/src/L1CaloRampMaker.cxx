/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloRampMaker.h"

#include <fstream>
#include <utility> // make_pair

#include "AthenaKernel/errorcheck.h"

#include "xAODEventInfo/EventInfo.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloEvent/CaloCell.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloxAODOfflineTriggerTowerTools.h"

#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloRampData.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// temporary includes to work around limitations in L1CaloxAODOfflineTriggerTowerTools
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloEvent/TriggerTower.h"

#include "xAODTrigL1Calo/TriggerTower.h"

// define Accessors
namespace {
xAOD::TriggerTower::ConstAccessor<float> ttCellsEnergy("CaloCellEnergy");
xAOD::TriggerTower::ConstAccessor<float> ttCellsET("CaloCellET");
}

L1CaloRampMaker::L1CaloRampMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_triggerTowerContainerName(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation),
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
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_xAODTTTools("LVL1::L1CaloxAODOfflineTriggerTowerTools/L1CaloxAODOfflineTriggerTowerTools"),
    m_jmTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_condSvc("L1CaloCondSvc", name),
    m_nEvent(1),
    m_firstEvent(true),
    m_lvl1Helper(nullptr),
    m_rampDataContainer(nullptr),
    m_pprLutContainer(nullptr)
    //m_pprDisabledChannelContainer(nullptr)
{
    declareProperty("TriggerTowerCollectionName", m_triggerTowerContainerName);
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
    declareProperty("SpecialChannelRange", m_specialChannelRange);
}

L1CaloRampMaker::~L1CaloRampMaker()
{
}

StatusCode L1CaloRampMaker::initialize()
{
    CHECK(m_condSvc.retrieve());

    const CaloIdManager *caloMgr = nullptr;
    CHECK(detStore()->retrieve(caloMgr));
    m_lvl1Helper = caloMgr->getLVL1_ID();
    if(!m_lvl1Helper) {
        ATH_MSG_FATAL( "Cannot access CaloLVL1_ID helper." );
        return StatusCode::FAILURE;
    }

    CHECK(m_ttTool.retrieve());
    CHECK(m_xAODTTTools.retrieve());
    CHECK(m_jmTools.retrieve());

    // setup access to Calib1 folder of L1CALO database
    m_pprLutContainerFolderMap.insert(std::make_pair(L1CaloPprConditionsContainerRun2::ePprChanCalib, "/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanCalib"));
    m_pprLutContainerFolderMap.insert(std::make_pair(L1CaloPprConditionsContainerRun2::ePprChanDefaults, "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"));
    // m_pprDisabledChannelContainerFolderMap.insert(std::make_pair(L1CaloPprDisabledChannelContainer::ePpmDeadChannels,
    //                                                              "/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels"));
    // m_pprDisabledChannelContainerFolderMap.insert(std::make_pair(L1CaloPprDisabledChannelContainer::ePprChanCalib,
    //                                                              "/TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib"));
    // m_pprDisabledChannelContainerFolderMap.insert(std::make_pair(L1CaloPprDisabledChannelContainer::eDisabledTowers,
    //                                                              "/TRIGGER/L1Calo/V1/Conditions/DisabledTowers"));

    return StatusCode::SUCCESS;
}

StatusCode L1CaloRampMaker::execute()
{
    // Skip spurious events
    unsigned int wantedEvents = m_nEventsPerStep*m_nSteps;
    if (m_nEvent > wantedEvents) {
        if (m_nEvent == wantedEvents+1) {
            ATH_MSG_INFO( "Processed " << wantedEvents
                  << " events, skipping the rest" );
        }
        ++m_nEvent;
        return StatusCode::SUCCESS;
    }

    const xAOD::EventInfo* eventInfo = nullptr;
    CHECK(evtStore()->retrieve(eventInfo));
    if (eventInfo->eventNumber() == 0 && m_nEvent > 1) {
      // Only allow event 0 as first event
        ATH_MSG_WARNING( "Skipping spurious event number 0" );
	return StatusCode::SUCCESS;
    }

    const xAOD::TriggerTowerContainer *tts = nullptr;
    auto sc = evtStore()->retrieve(tts, m_triggerTowerContainerName);
    if(!sc.isSuccess()) {
        ATH_MSG_ERROR( "Cannot retrieve TriggerTowerCollection '"
                       << m_triggerTowerContainerName << "' from StoreGate." );
        return StatusCode::RECOVERABLE;
    }

    // init trigger tower to cell mapping
    CHECK(m_xAODTTTools->initCaloCells());

    // CHECK(m_condSvc->retrieve(m_pprLutContainer, m_pprLutContainerFolderMap));
    // CHECK(m_condSvc->retrieve(m_pprDisabledChannelContainer, m_pprDisabledChannelContainerFolderMap));
    
    // access L1CaloPPrLutContinaer
    // since no key is given, we will override the default lookup
    // to the folder specified in initialize (i.e. the .../Calib1/PprChanCalib version)
    CHECK(m_condSvc->retrieve(m_pprLutContainer, m_pprLutContainerFolderMap));
    CHECK(m_ttTool->retrieveConditions());

    if(m_firstEvent) {
      unsigned int runNumber = eventInfo->runNumber();
      std::string gainStrategy("");
      const CondAttrListCollection* gainStrategyColl = 0;
      sc = detStore()->retrieve(gainStrategyColl, m_gainStrategyFolder);
      if(sc.isSuccess()) {
        CondAttrListCollection::const_iterator itr  = gainStrategyColl->begin();
	CondAttrListCollection::const_iterator itrE = gainStrategyColl->end();
	bool consistent(true);
	for (; itr != itrE; ++itr) {
	  const unsigned int channel = itr->first;
          if (channel != 1 && channel != 2) continue;
	  const coral::AttributeList& attrList = itr->second;
          const std::string strategy(attrList["name"].data<std::string>());
          const std::string status(attrList["status"].data<std::string>());
	  ATH_MSG_INFO( "Gain Strategy: channel = " << channel
	                     << ", name = " << strategy
		             << ", status = " << status );
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
          ATH_MSG_INFO( "Changing Gain Strategy to " << newStrategy);
          gainStrategy = newStrategy;
        }
	if (gainStrategy != "" && consistent) {
	  m_isGain1 = (gainStrategy.find("GainOne") != std::string::npos);
	  m_isOvEmb = (gainStrategy.find("OvEmb") != std::string::npos);
	  m_isOvEmec = (gainStrategy.find("OvEmec") != std::string::npos);
	  m_isFcalLowEta = (gainStrategy.find("FcalLowEta") != std::string::npos);
	  m_isFcalHighEta = (gainStrategy.find("FcalHighEta") != std::string::npos);
	} else if (gainStrategy == "") {
	  ATH_MSG_WARNING( "Gain Strategy collection empty" );
	} else {
	  ATH_MSG_WARNING( "Inconsistent Gain Strategies" );
	}
      } else {
        ATH_MSG_WARNING( "No Gain Strategy collection found" );
      }
      ATH_MSG_INFO( "isGain1 = "         << m_isGain1
                    << ", isOvEmb = "       << m_isOvEmb
                    << ", isOvEmec = "      << m_isOvEmec
                    << ", isFcalLowEta = "  << m_isFcalLowEta
                    << ", isFcalHighEta = " << m_isFcalHighEta );

      setupRampDataContainer(tts);
      m_rampDataContainer->setRunNumber(runNumber);
      m_rampDataContainer->setGainStrategy(gainStrategy);
      m_firstEvent = false;
    }

    auto specialChannelRangeEnd = m_specialChannelRange.end();
    bool nextStep = (m_nEvent % m_nEventsPerStep == 0);
    for(auto *tt : *tts) {
        // skip saturated towers
        if(tt->isJepSaturated()) continue;

        // isSaturated flag is not enough to check - test FADC for saturation, too
        auto max = std::max_element(tt->adc().begin(), tt->adc().end());
        if(*max >= m_fadcSaturationCut) continue;

        // skip disabled channels
        if(m_ttTool->disabledChannel(tt->coolId())) continue;

        bool isTile = m_xAODTTTools->isTile(*tt);

        if((m_doLAr && !isTile) || (m_doTile && isTile)) {
            if(m_checkProvenance) checkProvenance(tt);
            double level1Energy = getTriggerTowerEnergy(tt);
            double caloEnergy = getCaloEnergy(tt);

            // cut on 150 GeV ADC for Tile, lots of saturating calibration signals
            if(isTile && level1Energy > m_tileSaturationCut) continue;

            // see if we have a special channel and cut on level1 energy
            auto specialChannelIt = m_specialChannelRange.find(tt->coolId());
            if(specialChannelIt == specialChannelRangeEnd || level1Energy < specialChannelIt->second) {
              ATH_MSG_DEBUG("Adding Energy for " << tt->coolId() << ":" << caloEnergy << " vs. " << level1Energy);
              m_rampDataContainer->rampData(tt->coolId())->addData(caloEnergy, level1Energy);
            } else {
                // make sure we don't accidently miss the threshold again
                specialChannelIt->second = -1000;
            }
            if(nextStep) m_rampDataContainer->rampData(tt->coolId())->nextStep();
        }
    }

    ++m_nEvent;

    return StatusCode::SUCCESS;
}

void L1CaloRampMaker::setupRampDataContainer(const xAOD::TriggerTowerContainer* tts)
{
    m_rampDataContainer.reset(new L1CaloRampDataContainer());

    L1CaloRampData rd;
    for(auto* tt: *tts) {
      bool isTile = m_xAODTTTools->isTile(*tt);
      if((m_doLAr && !isTile) || (m_doTile && isTile)) {
        m_rampDataContainer->addRampData(tt->coolId(), rd);
      }
    }
}

// return E_T of cells comprising the trigger tower @c tt
double L1CaloRampMaker::getCaloEnergy(const xAOD::TriggerTower* tt)
{
    double et(0.);
    auto id = m_ttTool->identifier(tt->eta(), tt->phi(), tt->sampling());

    if(m_lvl1Helper->is_fcal(id)) {
	if(tt->sampling() == 0) { // fcal em - use decorated value
            et = ttCellsET(*tt);
	} else {          // had
            LVL1::TriggerTowerKey K(tt->phi(), tt->eta());
            LVL1::TriggerTower T(tt->phi(), tt->eta(), K.ttKey(tt->phi(), tt->eta()));
	    std::vector<float> etRec = m_jmTools->hadTTCellsEtByReceiver(&T);
	    if (etRec.size() == 2) {
	        if      (m_isFcalLowEta)  et = etRec[0];
		else if (m_isFcalHighEta) et = etRec[1];
	        else                      et = etRec[0] + etRec[1];
            } else if (etRec.size() == 1) et = etRec[0];
        }
    } else {
	if(m_lvl1Helper->is_barrel_end(id) && (m_isOvEmb || m_isOvEmec)) {
            LVL1::TriggerTowerKey K(tt->phi(), tt->eta());
            LVL1::TriggerTower T(tt->phi(), tt->eta(), K.ttKey(tt->phi(), tt->eta()));
	    std::vector<float> energyRec = m_jmTools->emTTCellsEnergyByReceiver(&T);
	    if (energyRec.size() == 2) {
	        double energy(0.);
		if (m_isOvEmec) energy = energyRec[0];
		else            energy = energyRec[1];
		et = energy / std::cosh(tt->eta());
            } else et = energyRec[0] / std::cosh(tt->eta());
	} else {
            et = ttCellsEnergy(*tt) / std::cosh(tt->eta());
	}
    }
    return et;
}

// calculate trigger tower E_T from adc values
// if run is with gain 1 loaded, need special treatment of E_T for HEC and TILE
double L1CaloRampMaker::getTriggerTowerEnergy(const xAOD::TriggerTower* tt) {
    // calibration is against ADC energy "(adc peak value - pedestal) / 4"
    auto max = std::max_element(tt->adc().begin(), tt->adc().end());

    const L1CaloPprConditionsRun2 *pprConditions = m_pprLutContainer->pprConditions(tt->coolId());
    if(!pprConditions) {
        ATH_MSG_WARNING("Empty PPrConditions for 0x "
                        << std::hex << tt->coolId() << std::dec << "!");
        return 0.;
    }

    double energy = (*max - int(pprConditions->pedValue())) * 0.25;
    if (energy < 0.) energy = 0.;

    // no corrections if gain is set correctly
    if(!m_isGain1) return energy;

    // correct hec and tile if gain 1
    auto id = m_ttTool->identifier(tt->eta(), tt->phi(), tt->sampling());
    if(m_lvl1Helper->is_hec(id)) {
        energy /= std::cosh(tt->eta());
        if(fabs(tt->eta()) > 2.5) {
            energy *= 10.;
        } else if(fabs(tt->eta()) > 1.9) {
            energy *= 5.;
        } else {
            energy *= 3.125;
        }
    } else if(m_lvl1Helper->is_tile(id)) {
        energy /= std::cosh(tt->eta());
    }

    return energy;
}

// function fills a map of cooldid -> (number of cells failing quality, avg. cell timing)
// used for fcal debugging
void L1CaloRampMaker::checkProvenance(const xAOD::TriggerTower* tt) {
    const std::vector<const CaloCell*>& cells(m_xAODTTTools->getCaloCells(*tt));
    double oneOverNCells = 1./double(cells.size());
    for(const auto *cell : cells) {
	auto it = m_mapBadOFCIteration.find(tt->coolId());
	if(it == m_mapBadOFCIteration.end()) {
	    m_mapBadOFCIteration.insert(std::make_pair(tt->coolId(), std::make_pair(!(cell->provenance() & 0x100),
                                                                                    (cell->time()*oneOverNCells))));
	    continue;
	}
	if(!(cell->provenance() & 0x100)) {
	    it->second.first += 1;
	}
	it->second.second += (cell->time()*oneOverNCells);
    }
}

StatusCode L1CaloRampMaker::finalize()
{
    CHECK(detStore()->record(std::move(m_rampDataContainer), m_outputFolderName));

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

