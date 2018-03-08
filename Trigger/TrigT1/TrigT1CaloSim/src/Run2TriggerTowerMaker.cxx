/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// ================================================
// Run2TriggerTowerMaker class Implementation
// ================================================

#include "TrigT1CaloSim/Run2TriggerTowerMaker.h"

// trigger include(s)
#include "TrigT1CaloCalibConditions/L1CaloModuleType.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloDisabledTowersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannelsContainer.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// calorimeter include(s)
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "TileConditions/TileInfo.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "xAODEventInfo/EventInfo.h"

// For the Athena-based random numbers.
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/Randomize.h"
#include "CLHEP/Random/RandomEngine.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <limits> // for std::numeric_limits<std::streamsize>
#include <utility> // for std::move
#include <stdexcept>
#include <sys/types.h>

namespace LVL1 {

namespace {
constexpr static int ADCMAX = 1023;
constexpr static int SATURATIONVALUE = 255;

// keep these local to this compilation unit
xAOD::TriggerTower::Decorator<int> firDecorator("fir");

}

Run2TriggerTowerMaker::Run2TriggerTowerMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_rndGenSvc("AtRndmGenSvc", name),
    m_condSvc("L1CaloCondSvc", name),
    m_rndmADCs(0),
    m_TTtool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_mappingTool("LVL1::PpmMappingTool/PpmMappingTool"),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_caloId(0),
    m_digitScale(250.),
    m_cpLutScale(1.),
    m_jepLutScale(1.),
    m_TileToMeV(s_MEV/4.1), // Scale for converting ET -> counts
    m_TileTTL1Ped(0.), // TileTTL1 pedestal value - need to subtract if set non-zero
    m_isDataReprocessing(false)
{
  declareProperty("RndmSvc", m_rndGenSvc, "Random number service");
  declareProperty("DigiEngine", m_digiEngine = "TrigT1CaloSim_Digitization");

  declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty("PpmMappingTool", m_mappingTool);
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool);

  declareProperty("inputTTLocation", m_inputTTLocation=TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("EmTTL1ContainerName",m_EmTTL1ContainerName= "LArTTL1EM");
  declareProperty("HadTTL1ContainerName",m_HadTTL1ContainerName= "LArTTL1HAD");
  declareProperty("TileTTL1ContainerName",m_TileTTL1ContainerName= "TileTTL1Cnt");
  declareProperty("RequireAllCalos",m_requireAllCalos=true,"Should EM,Had and Tile all be available?");

  declareProperty("TriggerTowerLocation", m_outputLocation= TrigT1CaloDefs::xAODTriggerTowerLocation);
  declareProperty("CellType", m_cellType = TTL1);

  // ADC simulation
  declareProperty("ADCStep", m_adcStep=250.);
  declareProperty("ADCNoise", m_adcVar=0.65);
  declareProperty("CalibrationUncertainty", m_gainCorr=0.);

  declareProperty("DecorateFIR", m_decorateFIR = false, "Add FIR values to the xAOD::TriggerTowers");

  declareProperty("ZeroSuppress", m_ZeroSuppress = true, "Do not save towers with 0 energy");

  declareProperty("ChanCalibFolderKey",
                  m_chanCalibKey = "/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib",
                  "PprChanCalib key");
  declareProperty("ChanDefaultsFolderKey",
                  m_chanDefaultsKey = "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults",
                  "PprChanDefaults key");
  declareProperty("DisabledTowersFolderKey",
                  m_disabledTowersKey = "/TRIGGER/L1Calo/V2/Conditions/DisabledTowers",
                  "DisabledTowers key");
  declareProperty("DeadChannelsFolderKey",
                  m_deadChannelsKey = "/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels",
                  "PpmDeadChannels key");

  // Create hash table for E->ET conversions
  /* Fill table with dummy values */
  m_sinThetaHash.fill(-1.);

  /* set values for barrel region with granularity of 0.1*/
  for(unsigned int i = 0; i < 25; i++) {
    m_sinThetaHash[i] = 1.0/cosh((i+0.5)* 0.1);
  }

  /* set values for EndCap with granularity of 0.2 except tt by |3.2|
     eta values are are: 2.6, 2.8, 3.0, 3.15 */
  m_sinThetaHash[26] = 1.0/cosh(2.6);
  m_sinThetaHash[28] = 1.0/cosh(2.8);
  m_sinThetaHash[30] = 1.0/cosh(3.0);
  m_sinThetaHash[31] = 1.0/cosh(3.15);

  /* fcal granularity is 0.425 */
  m_sinThetaHash[ (unsigned int)(32 + (0.5*0.425)*10.) ] = 1.0/cosh(3.2 + 0.5*0.425);
  m_sinThetaHash[ (unsigned int)(32 + (1.5*0.425)*10.) ] = 1.0/cosh(3.2 + 1.5*0.425);
  m_sinThetaHash[ (unsigned int)(32 + (2.5*0.425)*10.) ] = 1.0/cosh(3.2 + 2.5*0.425);
  m_sinThetaHash[ (unsigned int)(32 + (3.5*0.425)*10.) ] = 1.0/cosh(3.2 + 3.5*0.425);
}

Run2TriggerTowerMaker::~Run2TriggerTowerMaker() {}

StatusCode Run2TriggerTowerMaker::initialize()
{
  ATH_MSG_DEBUG("Initialising");

  CHECK(detStore()->retrieve(m_caloId).isSuccess());
  CHECK(m_configSvc.retrieve());
  CHECK(m_mappingTool.retrieve());
  CHECK(m_TTtool.retrieve());
  CHECK(m_rndGenSvc.retrieve());
  CHECK(m_lumiBlockMuTool.retrieve());
  CHECK(m_condSvc.retrieve());

  m_rndmADCs = m_rndGenSvc->GetEngine(m_digiEngine);
  if(!m_rndmADCs) {
    ATH_MSG_ERROR("Failed to retrieve random engine");
    return StatusCode::FAILURE;
  }

  // Listen for BeginRun
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  CHECK(incSvc.retrieve());
  incSvc->addListener(this, "BeginRun");

  // reserve enough storage for the amps
  m_xaodTowersAmps.assign(7168, std::vector<double>());

  return StatusCode::SUCCESS;
}

/** Best if initialisation which uses COOL-derived values is done here
    rather than in initialize() */
void Run2TriggerTowerMaker::handle(const Incident& inc)
{
  if(inc.type() != "BeginRun") return;
  /// Get global scales from configSvc
  double globalScale = m_configSvc->thresholdConfig()->caloInfo().globalScale();
  m_cpLutScale = m_configSvc->thresholdConfig()->caloInfo().globalEmScale();
  m_jepLutScale = m_configSvc->thresholdConfig()->caloInfo().globalJetScale();

  ATH_MSG_INFO("REGTEST Digit scale = " << globalScale << " GeV/count");
  ATH_MSG_INFO("REGTEST CP scale = " << m_cpLutScale << " count/GeV");
  ATH_MSG_INFO("REGTEST JEP scale = " << m_jepLutScale << " count/GeV");

  /// globalScale is number of GeV/count. As code is already written to use
  /// MeV/count, safest thing here is to convert:
  m_digitScale = 1000.*globalScale;

  // retrieve conditions
  m_condSvc->retrieve(m_chanCalibContainer, m_chanCalibKey).ignore();
  m_condSvc->retrieve(m_disabledTowersContainer, m_disabledTowersKey).ignore();
  m_condSvc->retrieve(m_deadChannelsContainer, m_deadChannelsKey).ignore();
  L1CaloPprChanDefaultsContainer *cDC = nullptr;
  m_condSvc->retrieve(cDC, m_chanDefaultsKey).ignore();
  if(!m_chanCalibContainer || !cDC ||
     !m_disabledTowersContainer || !m_deadChannelsContainer) {
    ATH_MSG_ERROR("Could not retrieve database containers. Aborting ...");
    throw std::runtime_error("Run2TriggerTowerMaker: database container not accesible");
  }
  
  auto* defaults = cDC->pprChanDefaults(0); // non-owning ptr
  if(!defaults) {
    ATH_MSG_ERROR("Could not retrieve channel 0 PprChanDefaults folder. Aborting ...");
    throw std::runtime_error("Run2TriggerTowerMaker: channel 0 of PprChanDefaults not accesible");
  }
  m_chanDefaults = *defaults;

  const TileInfo* tileInfo = nullptr;
  if(detStore()->retrieve(tileInfo, "TileInfo").isFailure()) {
    ATH_MSG_ERROR("Failed to find TileInfo");
    m_TileToMeV = s_MEV/4.1;
  }

  m_TileToMeV = s_MEV/tileInfo->TTL1Calib({});
  ATH_MSG_DEBUG("Tile TTL1 calibration scale = " << tileInfo->TTL1Calib({}));
  m_TileTTL1Ped = tileInfo->TTL1Ped({});
  ATH_MSG_DEBUG("Tile TTL1 pedestal value = " << m_TileTTL1Ped);
  
  // try to determine wheter we run on data or on simulation
  auto ei = dynamic_cast<const EventIncident*>(&inc);
  const EventType *eventType=nullptr;
  if(ei) {
    if (ei->eventInfo().event_type()){
      eventType=ei->eventInfo().event_type();
    }
  }
  if (eventType==nullptr){
    const EventInfo* eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo).isSuccess() ) {
      eventType=eventInfo->event_type();
    }
  }
  if (eventType==nullptr) {
    ATH_MSG_WARNING("Could not determine if input file is data or simulation. Will assume simulation.");
  } else {
    m_isDataReprocessing = !eventType->test(EventType::IS_SIMULATION);
    if(m_isDataReprocessing) {
      ATH_MSG_INFO("Detected data reprocessing. Will take pedestal correction values from input trigger towers.");
    } else {
      ATH_MSG_VERBOSE("No data reprocessing - running normal simulation.");
    }
  }

}

StatusCode Run2TriggerTowerMaker::finalize() {
  ATH_MSG_DEBUG("Finalizing");
  return StatusCode::SUCCESS;
}

/** Checks that the Cell Type is supported (terminates with errors if not)
    and calls relevant routine to look for the cells.
*/
StatusCode Run2TriggerTowerMaker::execute() {
  ATH_MSG_VERBOSE("Executing");

  m_xaodTowers.reset(new xAOD::TriggerTowerContainer);
  m_xaodTowersAux.reset(new xAOD::TriggerTowerAuxContainer);
  m_xaodTowers->setStore(m_xaodTowersAux.get());
  m_xaodTowers->resize(7168); // avoid frequent reallocations
  m_curIndex = 0u;

  switch(m_cellType) {
  case TRIGGERTOWERS:
    ATH_MSG_VERBOSE("Looking for TriggerTower input");
    CHECK(getTriggerTowers());
    break;
  case TTL1:
    ATH_MSG_VERBOSE("Looking for calo towers");
    CHECK(getCaloTowers());
    digitize(); // digitisation
    break;
  default:
    ATH_MSG_ERROR("Unsupported Cell Type!!!!!!"); return StatusCode::FAILURE;
  }

  CHECK(preProcess()); // FIR, BCID etc

  // store them thar trigger towers
  CHECK(store());

  return StatusCode::SUCCESS;
}

/** Stores Trigger Towers in the TES, at a
    location defined in m_outputLocation.

    Returns FAILURE if the towers could not be saved.
*/
StatusCode LVL1::Run2TriggerTowerMaker::store()
{
  ATH_MSG_DEBUG("Storing TTs in DataVector");
  if(m_outputLocation.empty()) return StatusCode::SUCCESS;

  if(m_ZeroSuppress) {
    // remove trigger towers whose energy is 0
    m_xaodTowers->erase(std::remove_if(m_xaodTowers->begin(), m_xaodTowers->end(),
                                       [](const xAOD::TriggerTower* tt){
                                         return tt->cpET() == 0 && tt->jepET() == 0;
                                       }),
                        m_xaodTowers->end());
  }

  CHECK(evtStore()->record(m_xaodTowers.release(), m_outputLocation));
  CHECK(evtStore()->record(m_xaodTowersAux.release(), m_outputLocation+"Aux."));

  return StatusCode::SUCCESS;
} // end of LVL1::Run2TriggerTowerMaker::store(){


/** gets TriggerTowers from input collection and copies ADC digits into
    xAOD::TriggerTowers for reprocessing */
StatusCode LVL1::Run2TriggerTowerMaker::getTriggerTowers()
{
  const xAOD::TriggerTowerContainer* inputTTs = nullptr;
  ATH_MSG_DEBUG("Retrieve input TriggerTowers " << m_inputTTLocation);
  CHECK(evtStore()->retrieve(inputTTs, m_inputTTLocation));
  ATH_MSG_VERBOSE("Found " << inputTTs->size() << " input TriggerTowers");

  for(const auto& tower : *inputTTs) {
    auto t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
    *t = *tower;
  }

  /// If < 7168 towers in input data will be unallocated pointers in vector.
  //  /// So clean-up m_xaodTowers before these cause problems later.
  m_xaodTowers->erase(std::remove_if(m_xaodTowers->begin(), m_xaodTowers->end(),
                      [](const xAOD::TriggerTower* tt){return (tt == 0);}),
                      m_xaodTowers->end());

  return StatusCode::SUCCESS;
} // end of getTriggerTowers()

/** fetches LAr & Tile calorimeter towers */
StatusCode LVL1::Run2TriggerTowerMaker::getCaloTowers()
{
  // Find LAr towers in TES
  StatusCode sc1;
  const DataHandle<LArTTL1Container> EMTowers;
  if(!evtStore()->contains<LArTTL1Container>(m_EmTTL1ContainerName)) {
    ATH_MSG_WARNING("EM LArTTL1Container not found");
    sc1 = StatusCode::FAILURE;
  } else {
    sc1 = evtStore()->retrieve(EMTowers, m_EmTTL1ContainerName);
  }

  StatusCode sc2;
  const DataHandle<LArTTL1Container> HECTowers;
  if(!evtStore()->contains<LArTTL1Container>(m_HadTTL1ContainerName)) {
    ATH_MSG_WARNING("Had LArTTL1Container not found");
    sc2 = StatusCode::FAILURE;
  } else {
    sc2 = evtStore()->retrieve(HECTowers, m_HadTTL1ContainerName);
  }

  // Find Tile towers in TES
  StatusCode sc3;
  const DataHandle<TileTTL1Container> TileTowers;
  if(!evtStore()->contains<TileTTL1Container>(m_TileTTL1ContainerName)) {
    ATH_MSG_WARNING("Tile TTL1Container not found");
    sc3 = StatusCode::FAILURE;
  } else {
    sc3 = evtStore()->retrieve(TileTowers, m_TileTTL1ContainerName);
  }

  if(m_requireAllCalos && ((sc1==StatusCode::FAILURE) ||
                           (sc2==StatusCode::FAILURE) ||
                           (sc3==StatusCode::FAILURE))) {
    ATH_MSG_ERROR("Can't find calo towers - stopping processing" << endmsg
                  << "Found Em  LArTTL1 : "<<sc1 << endmsg
                  << "Found Had LArTTL1 : "<<sc2 << endmsg
                  << "Found TileTTL1    : "<<sc3<< endmsg
                  );
    return StatusCode::FAILURE;
  }

  // lets now try to create some trigger towers
  if(sc1 == StatusCode::SUCCESS) {
    ATH_MSG_DEBUG("Found " << EMTowers->size() << " EM towers ");
    processLArTowers(EMTowers);
  }
  if(sc2 == StatusCode::SUCCESS) {
    ATH_MSG_DEBUG("Found "<< HECTowers->size() << " HEC Towers");
    processLArTowers(HECTowers);
  }
  if(sc3 == StatusCode::SUCCESS) {
    ATH_MSG_DEBUG("Found "<< TileTowers->size() << " Tile Towers");
    processTileTowers(TileTowers);
  }

  /// If < 7168 towers in input data will be unallocated pointers in vector.
  //  /// So clean-up m_xaodTowers before these cause problems later.
  m_xaodTowers->erase(std::remove_if(m_xaodTowers->begin(), m_xaodTowers->end(),
                      [](const xAOD::TriggerTower* tt){return (tt == 0);}),
                      m_xaodTowers->end());

  return StatusCode::SUCCESS;
}

/** steps over Calo towers and creates/fills trigger towers */

void LVL1::Run2TriggerTowerMaker::processLArTowers(const LArTTL1Container * towers)
{
  int towerNumber=0;
  for(const auto& tower : *towers){
    ATH_MSG_VERBOSE("Looking at retrieved tower number "<<towerNumber++<<" ***********");

    // Obtain identifier
    Identifier id = tower->ttOfflineID();
    double eta = IDeta(id, m_caloId);
    double phi = IDphi(id, m_caloId);
    int layer = int(m_caloId->sampling(id) != 0);
    L1CaloCoolChannelId coolId = channelId(eta, phi, layer);

    // Tower amplitudes and type
    int nsamples = tower->nsamples();
    std::vector<float> tower_amps = tower->samples();

    // Now calibrate tower_amps then fill TriggerTower amplitude vector
    // TTL1 should have 7 samples, but this little kludge handles other
    // eventualities, provided peak is in centre of the pulse
    int offset = (nsamples - (s_FIRLENGTH+2))/2;
    std::vector<double> amps(s_FIRLENGTH+2);

    ATH_MSG_VERBOSE("nsamples = " << nsamples << " offset = " << offset);

    for(int i = 0; i < s_FIRLENGTH+2; i++) {
      int j = i + offset;
      if(j >= 0 && j < nsamples) {
        amps[i] = tower_amps[j];
      }
      else {
        amps[i] = 0.;
      }
      ATH_MSG_VERBOSE("amps[" << i << "] = " << amps[i]);
    }

    // Create TriggerTower
    auto t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
    t->setCoolId(coolId.id());
    t->setEta(eta);
    t->setPhi(phi);
    m_xaodTowersAmps[t->index()] = std::move(amps);
  } // end for loop
}

void LVL1::Run2TriggerTowerMaker::processTileTowers(const TileTTL1Container * towers)
{
  // Step over all towers
  int towerNumber=0;
  for(const auto& tower : *towers) {
    ATH_MSG_VERBOSE("Looking at retrieved tower number "<<towerNumber++<<" ***********");

    // Obtain identifier
    Identifier id = tower->TTL1_ID();
    double cal = m_TileToMeV;//calib(id, m_caloId)*m_TileToMeV;

    // Check this tower is used by the trigger
    // don't use gap or mbias scinitllators
    if(m_caloId->is_tile(id)) {
      double tower_eta = IDeta(id, m_caloId);
      double tower_phi = IDphi(id, m_caloId);

      // need to convert tower energy to E_t later
      unsigned Ieta = unsigned(fabs(tower_eta)*10.0);
      if(Ieta >= m_maxIetaBins){
        ATH_MSG_WARNING("TileTTL1 with invalid index for m_sinThetaHash: Ieta = " << Ieta);
        Ieta = 0u;
      }

      /* Extract amplitudes and rescale according to tower eta */
      int nsamples = tower->nsamples();
      std::vector<float> tower_amps = tower->fsamples();

      /* Debug message */
      ATH_MSG_VERBOSE(" nsamples = " << nsamples);

      // Want 7 samples, but this little kludge allows us to accept other
      // numbers, provided peak is in centre of the pulse
      int offset = (nsamples - (s_FIRLENGTH+2))/2;
      std::vector<double> amps(s_FIRLENGTH+2);
      for(int i = 0; i < 7; i++) {
        int j = i + offset;
        if(j >= 0 && j < nsamples) {
          amps[i] = (tower_amps[j]-m_TileTTL1Ped)*m_sinThetaHash[Ieta]*cal; // rescale to MeV
        }
        else {
          amps[i] = 0.;
        }
        /* Debug message */
        ATH_MSG_VERBOSE("amps[" << i << "] = " << amps[i]);
      }

      // Create TriggerTower
      // m_xaodTowers->push_back(new xAOD::TriggerTower);
      // auto t = m_xaodTowers->back();
      auto t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
      t->setCoolId(channelId(tower_eta, tower_phi, 1).id());
      t->setEta(tower_eta);
      t->setPhi(tower_phi);
      m_xaodTowersAmps[t->index()] = std::move(amps);
    } // end check on whether tower is used
  } // end for loop

  return;
}

/** Digitize pulses and store results back in xAOD::TriggerTowers */
void LVL1::Run2TriggerTowerMaker::digitize()
{
  // Loop over all existing towers and digitize pulses
  for(auto tower : *m_xaodTowers) {
    // First process EM layer
    L1CaloCoolChannelId id(tower->coolId());
    std::vector<int> digits = ADC(id, m_xaodTowersAmps[tower->index()]); // ADC simulation
    tower->setAdc(std::vector<uint16_t>(std::begin(digits), std::end(digits)));
    tower->setAdcPeak(digits.size()/2);
  }
}

namespace {
// This is the non-linear LUT function corresponding to strategy 3.
// This should actually go into LVL1::L1TriggerTowerTools (TrigT1CaloTools)
// but for now we keep it here to keep the number of touched packages small
// and make it easier to change some parts of the definition later on.
int non_linear_lut(int lutin, unsigned short offset, unsigned short slope, unsigned short noiseCut, unsigned short scale, short par1, short par2, short par3, short par4) {
  // turn shorts into double (database fields are shorts ... )

  // turn shorts into double
  double nll_slope = 0.001 * scale;
  double nll_offset = 0.001 * par1;
  double nll_ampl = 0.001 * par2;
  double nll_expo = 0.;
  if(par3) {
    nll_expo = -1. / (4096 * 0.001*par3);
  } else {
    nll_ampl = 0.;
  }
  double nll_noise = 0.001 * par4;

  // noise cut
  if (lutin * slope < offset + nll_noise * noiseCut) {
    return 0;
  }

  // actual calculation
  int output = int((((int)(2048 + nll_slope * (lutin * slope - offset)))>>12) + nll_offset + nll_ampl * std::exp(nll_expo * (lutin * slope - offset)));
  if(output >= 255) return 255;
  if(output < 0) return 0;
  return output;
}

template <typename MSG, typename T>
void printVec(MSG& msg, const std::vector<T>& v) {
  for(auto x : v) msg << (int)x << endmsg;
}

template <typename MSG, typename T, std::size_t N>
void printVec(MSG& msg, const std::array<T, N>& v) {
  for(auto x : v) msg << (int)x << endmsg;
}

} // namespace

StatusCode LVL1::Run2TriggerTowerMaker::preProcessTower(xAOD::TriggerTower *tower, int eventBCID)
{
  // get database information
  auto* chanCalib = m_chanCalibContainer->pprChanCalib(tower->coolId());
  if(!chanCalib) {
    ATH_MSG_ERROR("Tower with coolId: " << tower->coolId()
                  << " not stored in database! Aborting ...");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("::coolId: " << std::hex << tower->coolId() << std::dec);

  /// retrieve digits
  std::vector<int> digits(std::begin(tower->adc()), std::end(tower->adc()));

  /// process tower -- digitial filter
  std::vector<int> fir;
  m_TTtool->fir(digits,
                { chanCalib->firCoeff5(), chanCalib->firCoeff4(), chanCalib->firCoeff3(),
                    chanCalib->firCoeff2(), chanCalib->firCoeff1() }, // reverse order in database
                fir);

  /// dynamic pedestal correction
  std::vector<int16_t> correction;
  // a few cases follow
  // 1.) simulation and pedestal correction enabled
  // 2.) data reprocessing and pedestal correction enabled
  // 3.) pedestal correction disabled
  if(chanCalib->pedFirSum() && !m_isDataReprocessing) {
    // case 1.) (database "abuses" pedFirSum to steer pedestal correction)
    // apply the parameterized pedestal correction
    int firPed = (chanCalib->firCoeff5() + chanCalib->firCoeff4() + chanCalib->firCoeff3() +
                  chanCalib->firCoeff2() + chanCalib->firCoeff1()) * int(chanCalib->pedMean() + 0.5);
    m_TTtool->pedestalCorrection(fir,
                                 firPed,
                                 etaToElement(tower->eta(), tower->layer()),
                                 tower->layer(),
                                 eventBCID,
                                 m_lumiBlockMuTool->actualInteractionsPerCrossing(),
                                 correction);
  } else if(chanCalib->pedFirSum() && m_isDataReprocessing) {
    // case 2.) (database "abuses" pedFirSum to steer pedestal correction)
    // apply the recorded pedestal correction
    if(!tower->correctionEnabled().empty() && tower->correctionEnabled().front()) {
      std::size_t offset = (fir.size() - tower->correction().size())/2;

      for(std::size_t i = offset, e = fir.size() - offset; i != e; ++i) {
        correction.push_back(tower->correction()[i-offset]);
        fir[i] -= tower->correction()[i-offset];
      }
      ATH_MSG_VERBOSE("::correction: (from data");
      printVec(this->msg(MSG::VERBOSE), correction);
    } // in case the correction wasn't enabled in the readout nothing has to be done
  } else {
    // case 3.)
  }

  std::vector<int> lutIn;
  m_TTtool->dropBits(fir, chanCalib->firStartBit(), lutIn);

  // linear LUTs - CP
  std::vector<int> lutOut_cp;
  ATH_MSG_VERBOSE("::cp-lut: strategy: " << chanCalib->lutCpStrategy());
  if(chanCalib->lutCpStrategy() < 3) {
    // for new strategy lutSlope, lutOffset and lutNoiseCut are in units of LUTOut
    // and need to be multiplied by the scale factor
    double scale = (chanCalib->lutCpStrategy() == 0) ? 1. : m_cpLutScale;

    m_TTtool->lut(lutIn,
                  scale * chanCalib->lutCpSlope(),
                  scale * chanCalib->lutCpOffset(),
                  scale * chanCalib->lutCpNoiseCut(),
                  32 /* unused */,
                  chanCalib->lutCpStrategy() > 0,
                  false, // TODO - disabled?
                  lutOut_cp);
  } else if(chanCalib->lutCpStrategy() == 3) {
    for(auto l : lutIn) lutOut_cp.push_back(non_linear_lut(l, chanCalib->lutCpOffset(), chanCalib->lutCpSlope(), chanCalib->lutCpNoiseCut(), chanCalib->lutCpScale(), chanCalib->lutCpPar1(), chanCalib->lutCpPar2(), chanCalib->lutCpPar3(), chanCalib->lutCpPar4()));
  }
  ATH_MSG_VERBOSE("::cp-lut: lut:");
  printVec(this->msg(MSG::VERBOSE), lutOut_cp);


  // linear LUTs - JEP
  std::vector<int> lutOut_jep;
  ATH_MSG_VERBOSE("::jep-lut: strategy: " << chanCalib->lutJepStrategy());
  if(chanCalib->lutJepStrategy() < 3) {
    // for new strategy lutSlope, lutOffset and lutNoiseCut are in units of LUTOut
    // and need to be multiplied by the scale factor
    double scale = (chanCalib->lutJepStrategy() == 0) ? 1. : m_jepLutScale;

    m_TTtool->lut(lutIn,
                  scale * chanCalib->lutJepSlope(),
                  scale * chanCalib->lutJepOffset(),
                  scale * chanCalib->lutJepNoiseCut(),
                  32 /* unused */,
                  chanCalib->lutJepStrategy() > 0,
                  false, // TODO - disabled?
                  lutOut_jep);
  } else if(chanCalib->lutJepStrategy() == 3) {
    for(auto l : lutIn) lutOut_jep.push_back(non_linear_lut(l, chanCalib->lutJepOffset(), chanCalib->lutJepSlope(), chanCalib->lutJepNoiseCut(), chanCalib->lutJepScale(), chanCalib->lutJepPar1(), chanCalib->lutJepPar2(), chanCalib->lutJepPar3(), chanCalib->lutJepPar4()));
  }
  ATH_MSG_VERBOSE("::jep-lut: lut:");
  printVec(this->msg(MSG::VERBOSE), lutOut_jep);


  /// BCID algorithms (only possible if 7 slices readout)
  std::vector<int> BCIDOut;
  if(!m_isDataReprocessing || tower->adc().size() >= 7) {
    m_TTtool->bcid(fir, digits,
                   m_chanDefaults.peakFinderCond(),
                   chanCalib->satBcidThreshLow(),
                   chanCalib->satBcidThreshHigh(),
                   chanCalib->satBcidLevel(),
                   BCIDOut);
  } else {
    // in data reprocessing with less than 7 slices take decision from data
    BCIDOut.assign(tower->bcidVec().begin(), tower->bcidVec().end());
    ATH_MSG_VERBOSE("::bcidOut: (from data):");
    printVec(this->msg(MSG::VERBOSE), BCIDOut);
  }

  std::size_t peak = lutOut_jep.size()/2; // both cp & jep have the same length
  std::vector<uint_least8_t> etResultVectorCp { uint8_t(lutOut_cp[peak]) };
  ATH_MSG_VERBOSE("::etResultVector: cp:");
  printVec(this->msg(MSG::VERBOSE), etResultVectorCp);
  std::vector<uint_least8_t> etResultVectorJep { uint8_t(lutOut_jep[peak]) };
  ATH_MSG_VERBOSE("::etResultVector: jep:");
  printVec(this->msg(MSG::VERBOSE), etResultVectorJep);

  // identify BCID range
  int range;
  if(!(m_chanDefaults.decisionSource() & 0x1)) {
    range = EtRange(digits[digits.size()/2], chanCalib->bcidEnergyRangeLow(), chanCalib->bcidEnergyRangeHigh());
  } else {
    range = EtRange(fir[fir.size()/2], chanCalib->bcidEnergyRangeLow(), chanCalib->bcidEnergyRangeHigh());
  }
  ATH_MSG_VERBOSE("::range: " << range);

  // correct BCID for this range?
  std::array<int, 3> bcidDecision {
    {m_chanDefaults.bcidDecision1(), m_chanDefaults.bcidDecision2(), m_chanDefaults.bcidDecision3()}
  };
  ATH_MSG_VERBOSE("::bcidDecision:");
  printVec(this->msg(MSG::VERBOSE), bcidDecision);

  std::array<int, 3> satOverride {
    {m_chanDefaults.satOverride1(), m_chanDefaults.satOverride2(), m_chanDefaults.satOverride3()}
  };
  ATH_MSG_VERBOSE("::satOverride:");
  printVec(this->msg(MSG::VERBOSE), satOverride);

  if((bcidDecision[range]) & (0x1 << (BCIDOut[BCIDOut.size()/2]))) {
    if((satOverride[range]) & 0x1) {
      // return saturation if set
      etResultVectorCp[0] = SATURATIONVALUE;
      etResultVectorJep[0] = SATURATIONVALUE;
    }
  } else {
    // zero if fail BCID
    etResultVectorCp[0] = 0;
    etResultVectorJep[0] = 0;
  }
  
  // Overlay protection
  if (m_inputTTLocation == "NoneForOverlay") return StatusCode::SUCCESS;

  tower->setLut_cp(std::move(etResultVectorCp));
  tower->setLut_jep(std::move(etResultVectorJep));
  tower->setBcidVec({uint8_t(BCIDOut[BCIDOut.size()/2])});
  ATH_MSG_VERBOSE("::set bcidVec:");
  printVec(this->msg(MSG::VERBOSE), tower->bcidVec());
  tower->setPeak(0u); // we only added one item to etResultVector
  if(m_decorateFIR) firDecorator(*tower) = fir[fir.size()/2];

  /// In simulation external BCID is always zero, but for consistency with
  /// data we need to add it to the TriggerTower objects
  tower->setBcidExt(std::vector<uint8_t>(tower->adc().size(), 0u));

  // fill the pedestal correction
  if(chanCalib->pedFirSum()) {
    // online database abuses pedFirSum to steer pedestal correction
    tower->setCorrectionEnabled(std::vector<uint8_t>(tower->lut_cp().size(), 1u));
    tower->setCorrection(std::vector<int16_t>(tower->lut_cp().size(),
                                              correction[correction.size()/2]));
    ATH_MSG_VERBOSE("::set correction:");
    printVec(this->msg(MSG::VERBOSE), tower->correction());
  } else {
    tower->setCorrectionEnabled(std::vector<uint8_t>(tower->lut_cp().size(), 0u));
    tower->setCorrection(std::vector<int16_t>(tower->lut_cp().size(), 0u));
  }
  return StatusCode::SUCCESS;
}

/** Emulate FIR filter, bunch-crossing identification & LUT, and create & fill
    TriggerTowers. */
StatusCode LVL1::Run2TriggerTowerMaker::preProcess()
{
  // Pedestal Correction: Get the BCID number
  const xAOD::EventInfo* evt = nullptr;
  CHECK(evtStore()->retrieve(evt));
  auto eventBCID = evt->bcid();

  // Loop over all existing towers and simulate preprocessor functions
  for(auto tower : *m_xaodTowers) {
    CHECK(preProcessTower(tower, eventBCID));
  }
  return StatusCode::SUCCESS;
}

} // end of namespace bracket

std::vector<int> LVL1::Run2TriggerTowerMaker::ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps) const
{
  auto* chanCalib = m_chanCalibContainer->pprChanCalib(channel);
  if(!chanCalib) { ATH_MSG_ERROR("No database entry for tower " << channel.id()); return {}; }
  double ped = chanCalib->pedMean();

  // dice the calibration uncertainty if requested
  double adcCal = (m_gainCorr > 0.) ? CLHEP::RandGaussZiggurat::shoot(m_rndmADCs, 1., m_gainCorr) : 1.;  

  std::vector<int> digits;
  const int nSamples = amps.size();
  digits.reserve(nSamples);
  for(int i = 0; i < nSamples; ++i) {
    // dice the adc noise if requested
    double adcNoise = (m_adcVar > 0.) ? CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,m_adcVar) : 0.;

    int digit = int((amps[i]*adcCal/m_adcStep) + ped + adcNoise);
    if(digit > ADCMAX) digit = ADCMAX;
    if(digit < 0) digit = 0;
    digits.push_back(digit);
  }
  return digits;
}

int LVL1::Run2TriggerTowerMaker::EtRange(int et, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh) const
{
  if(et < bcidEnergyRangeLow)  return 0;
  if(et < bcidEnergyRangeHigh) return 1;
  return 2;
}

double LVL1::Run2TriggerTowerMaker::IDeta(const Identifier& id, const CaloLVL1_ID* l1id)
{
  int region = l1id->region(id);
  int ieta = l1id->eta(id);
  int sign = l1id->pos_neg_z(id);

  double gran[4] = {0.1, 0.2, 0.1, 0.425};
  double offset[4] = {0., 2.5, 3.1, 3.2};
  double eta;

  if(region>=0 && region<=3) {
    eta = sign* (((ieta+0.5) * gran[region]) + offset[region]);
  }
  else {
    eta = 0.;
  }

  return eta;
}


double LVL1::Run2TriggerTowerMaker::IDphi(const Identifier& id, const CaloLVL1_ID* l1id)
{
  Identifier regId = l1id->region_id(id);

  double phiMax = l1id->phi_max(regId);
  int iphi = l1id->phi(id);
  double phi = (iphi+0.5)*2*M_PI/(phiMax+1);

  return phi;
}

/** Function to compute L1CaloCoolChannelId from eta/phi/layer.
    Unlike L1TriggerTowerTool::channelID this function can cope with old geometries (hence
    the hard-coded numbers). So don't remove this until all ATLAS-CSC datasets are irrevokably
    deprecated */
L1CaloCoolChannelId LVL1::Run2TriggerTowerMaker::channelId(double eta, double phi, int layer)
{
  int crate, module, channel;
  m_mappingTool->mapping(eta, phi, layer, crate, module, channel);
  int slot = module + 5;
  int pin = channel % 16;
  int asic = channel / 16;
  return L1CaloCoolChannelId(crate, L1CaloModuleType::Ppm, slot, pin, asic, false);
}

int LVL1::Run2TriggerTowerMaker::etaToElement(float feta, int layer) const
{
  constexpr static int NELEMENTS = 33;
  /// Get integer eta bin
  float shiftedEta = feta + 4.9;
  uint eta = (uint)floor(shiftedEta*10.0);
  if(fabs(shiftedEta*10.0 - (uint)(eta+1)) < 0.01) eta++;
  if(fabs(shiftedEta) < 0.01) eta = 0;

  constexpr int nBins = 16;
  constexpr uint map[nBins] = {2,6,10,14,17,19,21,23,75,77,79,80,83,87,91,95};
  int element = -1;
  if(eta > 23 && eta < 74) {
    element = eta - 16;
  } else {
    for(int i = 0; i < nBins; i++) {
      if(eta == map[i]) {
        if(i < 8) element = i;
        else element = i + 50;
        break;
      }
    }
  }
  if      (layer == 1 && (element == 0 || element == 64)) element = 1; // FCal2-2
  else if (layer == 1 && (element == 1 || element == 65)) element = 0; // FCal3-2
  else if (layer == 1 && (element == 2 || element == 62)) element = 3; // FCal2-1
  else if (layer == 1 && (element == 3 || element == 63)) element = 2; // FCal3-1
  else if (element > 32) element = 65-element;

  // element 29 = FCal2-1, element 30 = FCal3-1, element 31 = FCal2-2, element 32 = FCal3-2
  element = NELEMENTS-element-1;

  return element;
}
