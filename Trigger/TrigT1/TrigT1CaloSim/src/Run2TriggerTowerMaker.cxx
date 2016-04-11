/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// Run2TriggerTowerMaker class Implementation
// ================================================
#include "TrigT1CaloSim/Run2TriggerTowerMaker.h"

// trigger include(s)
#include "TrigT1CaloCalibConditions/L1CaloModuleType.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// calorimeter include(s)
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "TileConditions/TileInfo.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
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
#include <sys/types.h>

namespace LVL1 {

namespace {
// keep these local to this compilation unit
xAOD::TriggerTower::Decorator<int> firDecorator("fir");
}

/** This is the constructor for TTMaker and is where you define the relevant
    parameters.
*/
Run2TriggerTowerMaker::Run2TriggerTowerMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
    m_rndGenSvc("AtRndmGenSvc", name),
    m_rndmPeds(0),
    m_rndmADCs(0),
    m_TTtool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_mappingTool("LVL1::PpmMappingTool/PpmMappingTool"),
    m_triggerTowerService("CaloTriggerTowerService/CaloTriggerTowerService"),
    m_caloId(0),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_decorateFIR(false),
    m_digitScale(250.),
    m_cpLutScale(1.),
    m_jepLutScale(1.),
    m_TileToMeV(s_MEV/4.1), // Scale for converting ET -> counts
    m_TileTTL1Ped(0.) // TileTTL1 pedestal value - need to subtract if set non-zero
{
  declareProperty("RndmSvc", m_rndGenSvc, "Random number service");
  declareProperty("PedEngine",m_pedEngine = "TrigT1CaloSim_Pedestal");
  declareProperty("DigiEngine",m_digiEngine = "TrigT1CaloSim_Digitization");

  declareProperty("BaselineCorrection", m_correctFir = true, "Pedestal Correction");
  declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty("PpmMappingTool", m_mappingTool);
  declareProperty("CaloTriggerTowerService", m_triggerTowerService);
  declareProperty("LumiBlockMuTool", m_lumiBlockMuTool);

  declareProperty("inputTTLocation", m_inputTTLocation=TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("EmTTL1ContainerName",m_EmTTL1ContainerName= "LArTTL1EM");
  declareProperty("HadTTL1ContainerName",m_HadTTL1ContainerName= "LArTTL1HAD");
  declareProperty("TileTTL1ContainerName",m_TileTTL1ContainerName= "TileTTL1Cnt");
  declareProperty("RequireAllCalos",m_requireAllCalos=true,"Should EM,Had and Tile all be available?");

  declareProperty("TriggerTowerLocation", m_outputLocation= TrigT1CaloDefs::TriggerTowerLocation);
  declareProperty("xAODTriggerTowerLocation", m_xOutputLocation = TrigT1CaloDefs::xAODTriggerTowerLocation );
  declareProperty("CellType", m_cellType = TTL1);

  declareProperty("EMTowerThreshold", m_emThresh = 4000,
                  "Default noise cut for all EM towers, if not specified in regions");
  declareProperty("HadTowerThreshold", m_hadThresh =4000,
                  "Default noise cut for all HAD towers, if not specified in regions");

  // ADC simulation
  declareProperty("PedestalValue", m_pedVal=32.);
  declareProperty("PedestalSpread", m_pedVar=1.);
  declareProperty("ADCStep", m_adcStep=250.);
  declareProperty("ADCNoise", m_adcVar=0.65);
  declareProperty("CalibrationUncertainty", m_gainCorr=0.);

  declareProperty("AutoCalibrateLUT", m_AutoCalibrateLUT = true,  "Local LUT calibration adjustments. If false, you *must* load the slopes manually");

  declareProperty("LUTStrategy", m_LUTStrategy=1, "1 = New, 0 = Old Strategy");
  declareProperty("RoundPedestal", m_RoundPed = true, "Rounding pedestal inside LUT");
  declareProperty("MatchFIR", m_MatchFIR = true, "automatically match FIR coefficients to pulse shape");
  declareProperty("DecorateFIR", m_decorateFIR, "Add FIR values to the xAOD::TriggerTowers");

  /** Set CalibLUT factor to 1 by default. For m_AutoCalibrateLUT = true this means
      making no correction to the receiver calibration.
      Note: if you set m_AutoCalibrateLUT = false, you will need to load the actual LUT
      slopes into these variables, in which case you'd better know how the LUT slope
      actually works - 1 will *not* be the correct value in this case! */
  // parameters for elements (eta bins)
  for(int i = 0; i < s_NLAYER; ++i) {
    m_CalibLUTElement[i].assign(s_NELEMENT, 1.);
    m_ThresholdElementCP[i].assign(s_NELEMENT, 0);
    m_ThresholdElementJEP[i].assign(s_NELEMENT, 0);

    m_SatLowElement[i].assign(s_NELEMENT, 0);
    m_SatHighElement[i].assign(s_NELEMENT, 0);
    for(int j = 0; j < s_NELEMENT; ++j) {
      m_elementInfo[i][j].FIRCoeff.assign(s_FIRLENGTH, 0); //default all to zero
    }
  }
  declareProperty("EmThreshElementCP", m_ThresholdElementCP[0]);
  declareProperty("HadThreshElementCP", m_ThresholdElementCP[1]);
  declareProperty("EmThreshElementJEP", m_ThresholdElementJEP[0]);
  declareProperty("HadThreshElementJEP", m_ThresholdElementJEP[1]);
  declareProperty("EmSlopeElement", m_CalibLUTElement[0]);
  declareProperty("HadSlopeElement", m_CalibLUTElement[1]);
  declareProperty("EmSatLowElement", m_SatLowElement[0]);
  declareProperty("HadSatLowElement", m_SatLowElement[1]);
  declareProperty("EmSatHighElement", m_SatHighElement[0]);
  declareProperty("HadSatHighElement", m_SatHighElement[1]);


  // Pedestal Correction: Em
  declareProperty("FilterCoeffsEmElement0", m_elementInfo[0][0].FIRCoeff);
  declareProperty("FilterCoeffsEmElement1", m_elementInfo[0][1].FIRCoeff);
  declareProperty("FilterCoeffsEmElement2", m_elementInfo[0][2].FIRCoeff);
  declareProperty("FilterCoeffsEmElement3", m_elementInfo[0][3].FIRCoeff);
  declareProperty("FilterCoeffsEmElement4", m_elementInfo[0][4].FIRCoeff);
  declareProperty("FilterCoeffsEmElement5", m_elementInfo[0][5].FIRCoeff);
  declareProperty("FilterCoeffsEmElement6", m_elementInfo[0][6].FIRCoeff);
  declareProperty("FilterCoeffsEmElement7", m_elementInfo[0][7].FIRCoeff);
  declareProperty("FilterCoeffsEmElement8", m_elementInfo[0][8].FIRCoeff);
  declareProperty("FilterCoeffsEmElement9", m_elementInfo[0][9].FIRCoeff);
  declareProperty("FilterCoeffsEmElement10", m_elementInfo[0][10].FIRCoeff);
  declareProperty("FilterCoeffsEmElement11", m_elementInfo[0][11].FIRCoeff);
  declareProperty("FilterCoeffsEmElement12", m_elementInfo[0][12].FIRCoeff);
  declareProperty("FilterCoeffsEmElement13", m_elementInfo[0][13].FIRCoeff);
  declareProperty("FilterCoeffsEmElement14", m_elementInfo[0][14].FIRCoeff);
  declareProperty("FilterCoeffsEmElement15", m_elementInfo[0][15].FIRCoeff);
  declareProperty("FilterCoeffsEmElement16", m_elementInfo[0][16].FIRCoeff);
  declareProperty("FilterCoeffsEmElement17", m_elementInfo[0][17].FIRCoeff);
  declareProperty("FilterCoeffsEmElement18", m_elementInfo[0][18].FIRCoeff);
  declareProperty("FilterCoeffsEmElement19", m_elementInfo[0][19].FIRCoeff);
  declareProperty("FilterCoeffsEmElement20", m_elementInfo[0][20].FIRCoeff);
  declareProperty("FilterCoeffsEmElement21", m_elementInfo[0][21].FIRCoeff);
  declareProperty("FilterCoeffsEmElement22", m_elementInfo[0][22].FIRCoeff);
  declareProperty("FilterCoeffsEmElement23", m_elementInfo[0][23].FIRCoeff);
  declareProperty("FilterCoeffsEmElement24", m_elementInfo[0][24].FIRCoeff);
  declareProperty("FilterCoeffsEmElement25", m_elementInfo[0][25].FIRCoeff);
  declareProperty("FilterCoeffsEmElement26", m_elementInfo[0][26].FIRCoeff);
  declareProperty("FilterCoeffsEmElement27", m_elementInfo[0][27].FIRCoeff);
  declareProperty("FilterCoeffsEmElement28", m_elementInfo[0][28].FIRCoeff);
  declareProperty("FilterCoeffsEmElement29", m_elementInfo[0][29].FIRCoeff);
  declareProperty("FilterCoeffsEmElement30", m_elementInfo[0][30].FIRCoeff);
  declareProperty("FilterCoeffsEmElement31", m_elementInfo[0][31].FIRCoeff);
  declareProperty("FilterCoeffsEmElement32", m_elementInfo[0][32].FIRCoeff);
  // Pedestal Correction: Had
  declareProperty("FilterCoeffsHadElement0", m_elementInfo[1][0].FIRCoeff);
  declareProperty("FilterCoeffsHadElement1", m_elementInfo[1][1].FIRCoeff);
  declareProperty("FilterCoeffsHadElement2", m_elementInfo[1][2].FIRCoeff);
  declareProperty("FilterCoeffsHadElement3", m_elementInfo[1][3].FIRCoeff);
  declareProperty("FilterCoeffsHadElement4", m_elementInfo[1][4].FIRCoeff);
  declareProperty("FilterCoeffsHadElement5", m_elementInfo[1][5].FIRCoeff);
  declareProperty("FilterCoeffsHadElement6", m_elementInfo[1][6].FIRCoeff);
  declareProperty("FilterCoeffsHadElement7", m_elementInfo[1][7].FIRCoeff);
  declareProperty("FilterCoeffsHadElement8", m_elementInfo[1][8].FIRCoeff);
  declareProperty("FilterCoeffsHadElement9", m_elementInfo[1][9].FIRCoeff);
  declareProperty("FilterCoeffsHadElement10", m_elementInfo[1][10].FIRCoeff);
  declareProperty("FilterCoeffsHadElement11", m_elementInfo[1][11].FIRCoeff);
  declareProperty("FilterCoeffsHadElement12", m_elementInfo[1][12].FIRCoeff);
  declareProperty("FilterCoeffsHadElement13", m_elementInfo[1][13].FIRCoeff);
  declareProperty("FilterCoeffsHadElement14", m_elementInfo[1][14].FIRCoeff);
  declareProperty("FilterCoeffsHadElement15", m_elementInfo[1][15].FIRCoeff);
  declareProperty("FilterCoeffsHadElement16", m_elementInfo[1][16].FIRCoeff);
  declareProperty("FilterCoeffsHadElement17", m_elementInfo[1][17].FIRCoeff);
  declareProperty("FilterCoeffsHadElement18", m_elementInfo[1][18].FIRCoeff);
  declareProperty("FilterCoeffsHadElement19", m_elementInfo[1][19].FIRCoeff);
  declareProperty("FilterCoeffsHadElement20", m_elementInfo[1][20].FIRCoeff);
  declareProperty("FilterCoeffsHadElement21", m_elementInfo[1][21].FIRCoeff);
  declareProperty("FilterCoeffsHadElement22", m_elementInfo[1][22].FIRCoeff);
  declareProperty("FilterCoeffsHadElement23", m_elementInfo[1][23].FIRCoeff);
  declareProperty("FilterCoeffsHadElement24", m_elementInfo[1][24].FIRCoeff);
  declareProperty("FilterCoeffsHadElement25", m_elementInfo[1][25].FIRCoeff);
  declareProperty("FilterCoeffsHadElement26", m_elementInfo[1][26].FIRCoeff);
  declareProperty("FilterCoeffsHadElement27", m_elementInfo[1][27].FIRCoeff);
  declareProperty("FilterCoeffsHadElement28", m_elementInfo[1][28].FIRCoeff);
  declareProperty("FilterCoeffsHadElement29", m_elementInfo[1][29].FIRCoeff);
  declareProperty("FilterCoeffsHadElement30", m_elementInfo[1][30].FIRCoeff);
  declareProperty("FilterCoeffsHadElement31", m_elementInfo[1][31].FIRCoeff);
  declareProperty("FilterCoeffsHadElement32", m_elementInfo[1][32].FIRCoeff);

  // BCID thresholds, ranges, decision criteria
  declareProperty("EnergyLow", m_EnergyLow=0);
  declareProperty("EnergyHigh", m_EnergyHigh=255);
  declareProperty("DecisionSource", m_DecisionSource=0);    // Use ADC for range decision
  declareProperty("BcidDecision1", m_BcidDecision[0]=0xF0); // FIR+peakfinder
  declareProperty("BcidDecision2", m_BcidDecision[1]=0xF0); // FIR+peakfinder
  declareProperty("BcidDecision3", m_BcidDecision[2]=0xCC); // Saturated pulse BCID
  declareProperty("SatOverride1", m_SatOverride[0]=0);
  declareProperty("SatOverride2", m_SatOverride[1]=0);
  declareProperty("SatOverride3", m_SatOverride[2]=1);
  declareProperty("PeakFinderCond", m_PeakFinderCond=0);

  declareProperty("SatLevel", m_SatLevel=1013);

  declareProperty("ZeroSuppress", m_ZeroSuppress = true, "Do not save towers with 0 energy");

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

Run2TriggerTowerMaker::~Run2TriggerTowerMaker() {
}

StatusCode Run2TriggerTowerMaker::initialize()
{
  ATH_MSG_DEBUG("Initialising");

  CHECK(detStore()->retrieve(m_caloId).isSuccess());
  CHECK(m_configSvc.retrieve());
  CHECK(m_mappingTool.retrieve());
  CHECK(m_triggerTowerService.retrieve());
  CHECK(m_TTtool.retrieve());
  CHECK(m_rndGenSvc.retrieve());
  CHECK(m_lumiBlockMuTool.retrieve());

  m_rndmPeds = m_rndGenSvc->GetEngine(m_pedEngine);
  m_rndmADCs = m_rndGenSvc->GetEngine(m_digiEngine);
  if(m_rndmPeds == 0 || m_rndmADCs == 0) {
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

  /// Now need to (re)-initialise parameters which depend on this value.
  if(initPulse().isFailure()) ATH_MSG_ERROR("Error initialising pulse");
}

/** the initPulse() method sets up FIR coefficients & LUT parameters for
    each pulse type. Where possible it updates from the calorimeter configuration
    files or datastore. FIR coefficients may be set by user or automatically
    matched to pulse shape */
StatusCode Run2TriggerTowerMaker::initPulse()
{
  /* FIR BCID simulation includes several factors which must be
     computed from the selected parameters.

     So, the procedure used here is that for each pulse type:
     - start with a default pulse profile
     - update from calorimeter data if available
     - optionally match FIR coefficients to pulse shape
     - compute calibration factors and pedestal subtraction from pulse & FIR */

  // LAr EM
  // define a default - updated 180114
  std::vector<double> EMBPulse = {0.10, 0.47, 1., 0.60, 0.12};
  std::vector<double> EMECPulse = {0.10, 0.47, 1., 0.60, 0.12};
  initLArPulse(EMBPulse, EmB, "LArEmLvl1.data");
  initLArPulse(EMECPulse, EmEC, "LArEmLvl1.data");

  // LAr HEC
  std::vector<double> HECPulse = {0.03, 0.61, 1., 0.76, 0.39};
  initLArPulse(HECPulse, Hec, "LArHecLvl1.data");

  // LAr Em FCAL
  std::vector<double> FCAL1Pulse = {0.00,0.21,1.,0.30,-0.45};
  initLArPulse(FCAL1Pulse, FcalE, "LArFcalLvl1.data");

  // LAr Had FCAL
  std::vector<double> FCAL23Pulse = {0.00,0.26,1.,0.52,-0.23};
  initLArPulse(FCAL23Pulse, FcalH, "LArFcalLvl1.data");

  // Tile
  std::vector<double> TILEPulse = {0.06,0.56,1.,0.67,0.28};
  initTile(TILEPulse);

  initLUTsElement(EmB, EMBPulse);
  initLUTsElement(EmEC, EMECPulse);
  initLUTsElement(FcalE, FCAL1Pulse);
  initLUTsElement(Tile, TILEPulse);
  initLUTsElement(Hec, HECPulse);
  initLUTsElement(FcalH, FCAL23Pulse);

  std::vector< std::vector<double> > Pulses;

  // the pulses must be push_back in order of Run2TriggerTowerMaker::TowerTypes
  // (EMB, EMEC, FCAL1, TILE, HEC, FCAL23)
  Pulses.push_back(std::move(EMBPulse));
  Pulses.push_back(std::move(EMECPulse));
  Pulses.push_back(std::move(FCAL1Pulse));
  Pulses.push_back(std::move(TILEPulse));
  Pulses.push_back(std::move(HECPulse));
  Pulses.push_back(std::move(FCAL23Pulse));

  // Have all of the pulse parameters now, so finally can set up the LUTs
  initLUTs(Pulses);

  // and the saturated BCID and ET range parameters
  initSatBCID(EmB, Pulses[EmB]);
  initSatBCID(EmEC, Pulses[EmEC]);
  initSatBCID(FcalE, Pulses[FcalE]);
  initSatBCID(Tile, Pulses[Tile]);
  initSatBCID(Hec, Pulses[Hec]);
  initSatBCID(FcalH, Pulses[FcalH]);

  return StatusCode::SUCCESS;
}

namespace {
std::array<float, 7> readPulseLArEM(std::ifstream& infile) {
  std::array<float, 7> pulseShape;
  float energy;
  infile >> energy
         >> pulseShape[0]
         >> pulseShape[1]
         >> pulseShape[2]
         >> pulseShape[3]
         >> pulseShape[4]
         >> pulseShape[5]
         >> pulseShape[6];
  return pulseShape;
}

std::array<float, 7> readPulseLArHEC(std::ifstream& infile) {
  std::array<float, 7> pulseShape;
  infile >> pulseShape[0]
         >> pulseShape[1]
         >> pulseShape[2]
         >> pulseShape[3]
         >> pulseShape[4]
         >> pulseShape[5]
         >> pulseShape[6];
  return pulseShape;
}

std::array<float, 7> readPulseLArFCAL(std::ifstream& infile, int Module) {
  static std::string line;
  // skip 3 lines per module we are not interested in
  int skip = Module - 1;
  while(skip--) {
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  // skip first line of module we are interested in
  infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::array<float, 7> pulseShape;
  infile >> pulseShape[0]
         >> pulseShape[1]
         >> pulseShape[2]
         >> pulseShape[3]
         >> pulseShape[4]
         >> pulseShape[5]
         >> pulseShape[6];
  return pulseShape;
}

} // anonymous namespace

void Run2TriggerTowerMaker::initLArPulse(std::vector<double>& Pulse, TowerTypes tType, const std::string& dataFile)
{
  if(msgLvl(MSG::DEBUG)) {
    for(int i = 0; i < s_FIRLENGTH; i++) {
      ATH_MSG_DEBUG("Initial pulse shape: Sample " << i << " amp = " << Pulse[i]);
    }
  }

  std::string pulsedataname = PathResolver::find_file(dataFile, "DATAPATH");
  if(pulsedataname == "") {
    ATH_MSG_ERROR("Could not locate LArEmLvl1.data file");
    ATH_MSG_ERROR("Use defaults and trust to luck!");
    return;
  }

  std::ifstream infile(pulsedataname);
  if(!infile) {
    ATH_MSG_ERROR("cannot open file " << pulsedataname);
    return;
  }
  ATH_MSG_DEBUG(dataFile << " file opened ");

  std::array<float, 7> pulseShape;
  switch(tType) {
  case EmB:
  case EmEC:
    pulseShape = readPulseLArEM(infile);
    break;
  case FcalE:
    pulseShape = readPulseLArFCAL(infile, 1);
    break;
  case Hec:
    pulseShape = readPulseLArHEC(infile);
    break;
  case FcalH:
    pulseShape = readPulseLArFCAL(infile, 2);
    break;
  case Tile:
    ATH_MSG_ERROR("default-clause should be unreachable");
    assert(false);
  }

  // map the inner @s_FIRLENGTH samples of @pulseShape to @Pulse
  constexpr int iTrig = 3;
  constexpr int offset = iTrig - int(s_FIRLENGTH/2);
  Pulse.assign(std::begin(pulseShape) + offset, std::begin(pulseShape) + offset + s_FIRLENGTH);
}

/** initTile method reads pulse shape & other parameters from TileInfo */
void Run2TriggerTowerMaker::initTile(std::vector<double>& Pulse)
{
  const TileInfo* tileInfo;
  StatusCode sc = detStore()->retrieve(tileInfo, "TileInfo");

  if(sc.isFailure()) {
    ATH_MSG_ERROR("Failed to find TileInfo");
    m_TileToMeV = s_MEV/4.1;
  }

  const int iTrig = tileInfo->ItrigSample();   // index of the triggering time slice
  const int offset = iTrig - int(s_FIRLENGTH/2);
  std::vector<double> pulseShape = tileInfo->ttl1Shape();
  Pulse.assign(std::begin(pulseShape) + offset, std::begin(pulseShape) + offset + s_FIRLENGTH);

  Identifier dummy;
  m_TileToMeV = s_MEV/tileInfo->TTL1Calib(dummy);
  ATH_MSG_DEBUG("Tile TTL1 calibration scale = " << tileInfo->TTL1Calib(dummy));
  m_TileTTL1Ped = tileInfo->TTL1Ped(dummy);
  ATH_MSG_DEBUG("Tile TTL1 pedestal value = " << m_TileTTL1Ped);
}

/** matchFIR method computes matched filter coefficients from given pulse shape */
std::vector<int> Run2TriggerTowerMaker::matchFIR(const std::vector<double>& Pulse)
{
  /** Find matched filter which gives best resolution into LUT.
      Logic is arranges so that if two ranges give similar precision it
      is biassed towards the one with finer coefficients (larger range) */
  int maxBest = 0;
  int useBest = 0;
  for(int drop = 32; drop >= 16; drop /= 2) { /// loop over plausible LUT ranges
    int sum;
    int use;
    int max = 16;
    bool ok = false;
    while (!ok) {
      sum = 0;
      use = 0;
      max -= 1;
      std::vector<int> coeff = calcFIR(Pulse,max); /// calculate filter for this scale
      for(unsigned int sample = 0; sample < Pulse.size(); ++sample) {
        use += int((1023-m_pedVal-1)*Pulse[sample]*coeff[sample]);
        sum += int(((1023-m_pedVal-1)*Pulse[sample] + m_pedVal+1)*coeff[sample]);
      }
      if(sum/drop < 1023) ok = true; /// ensure max adc amp within lut range
    }
    /// Now have largest filter coeffs that fit within this LUT input range.
    /// Next we work out how effectively the range is used
    /// Requiring significant improvement means prefer larger coefficients if similar
    if(use/drop > 1.1*useBest) {
      useBest = use/drop;
      maxBest = max;
    }
  }

  return calcFIR(Pulse, maxBest);
}

/** calcFIR computes matched FIR coefficients for a given pulse shape and
    maximum filter coefficient. Assumes input pulse normalised to peak = 1.
    Also respects hardware constraints (coefficient ranges) and does not
    apply negative weights if pulse in undershoot already */
std::vector<int> Run2TriggerTowerMaker::calcFIR(const std::vector<double>& Pulse, int max)
{
  const int nSamples = Pulse.size();
  std::vector<int> FIRco(nSamples);
  if(max > 15) max = 15;
  for(int sample = 0; sample < nSamples; ++sample) {
    int coeff = int(Pulse[sample]*max+0.5);
    if(coeff < 0) coeff = 0;
    if((sample == 0 || sample > 3) && coeff > 7) coeff = 7;
    FIRco[sample] = coeff;
  }
  return FIRco;
}

void Run2TriggerTowerMaker::handy(int cal, int iLayer, int iElement) {
  auto etas = elementToEta(iElement, iLayer); // pair of -eta, eta
  auto etam = etas.first;
  auto etap = etas.second;

  // How to handle pedestal when truncating to 10 bit LUT range
  float round = (m_RoundPed ? 0.5 : 0.);

  int nPhi(64);
  switch(cal) {
    case FcalE:
    case FcalH:
      nPhi=16;
      break;
    case EmEC:
    case Hec:
      if(iElement>=25) nPhi=32;
      break;
  }

  ElementInfo& element = m_elementInfo[iLayer][iElement];
  int slope = element.slope;
  int FIRsum = element.FIRsum;
  int dropBits = element.nDrop;
  element.thresh_cp = m_ThresholdElementCP[iLayer][iElement];
  element.thresh_jep = m_ThresholdElementJEP[iLayer][iElement];
  // in case it is not given fall back to default value per layer
  if(element.thresh_cp <= 0) {
    element.thresh_cp = (iLayer == 0) ? m_emThresh : m_hadThresh;
  }
  if(element.thresh_jep <= 0) {
    element.thresh_jep = element.thresh_cp;
  }

  float calslope = int(slope*m_CalibLUTElement[iLayer][iElement]);
  for(int iphi = 0; iphi < nPhi; ++iphi) {
    double phi = (float(iphi)+0.5)*M_PI/(0.5 * double(nPhi));

    double pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
    double firped = FIRsum*(m_pedVal+pedvar);

    float pedsubReal = 0.;
    double divisor = 1./(1<<dropBits); // divide by 2**dropBits
    if(m_LUTStrategy == 0) {
      pedsubReal = firped * divisor;
    } else {
      pedsubReal = firped * calslope * divisor - 0.5*calslope;
    }
    int pedsub = int(pedsubReal + round);
    L1CaloCoolChannelId idp = channelId(etap, phi, iLayer);
    m_pedestals.insert(std::make_pair(idp.id(), PedestalInfo{m_pedVal+pedvar, pedsub}));

    if(cal==FcalH) {
      element.offset = pedsub;  // for FCal23 mapping
    }

    pedvar = CLHEP::RandFlat::shoot(m_rndmPeds,-m_pedVar,m_pedVar);
    firped = FIRsum*(m_pedVal+pedvar);
    pedsubReal = 0.;
    if(m_LUTStrategy == 0) {
      pedsubReal = firped * divisor;
    }
    else {
      pedsubReal = firped * calslope * divisor - 0.5 * calslope;
    }
    pedsub = int(pedsubReal + round);
    L1CaloCoolChannelId idm = channelId(etam, phi, iLayer);
    m_pedestals.insert(std::make_pair(idm.id(), PedestalInfo{m_pedVal+pedvar, pedsub}));
  }
}


/** InitLUTs method uses FIR coefficients and pulse profile to compute
    LUT calibration parameters */
void Run2TriggerTowerMaker::initLUTs(const std::vector< std::vector<double> >& Pulses)
{
  // Loop over regions and compute parameters
  for(int cal = 0; cal < s_TOWERTYPES; ++cal) {
    if(cal < int(Pulses.size())) {
      // Now apply, with local corrections ifany, to all towers in region
      int iLayer=0;
      if(cal == EmB || cal==Tile) {
        iLayer = (cal==Tile);
        for(int iElement = 0; iElement < 15; ++iElement) {
          handy(cal, iLayer, iElement);
        }
      } else if(cal == EmEC || cal==Hec) {
        iLayer= (cal==Hec);
        for(int iElement = 15; iElement < 29; ++iElement) {
          handy(cal, iLayer, iElement);
        }
      } else if(cal == FcalE||cal==FcalH) {
        iLayer= (cal==FcalH);
        for(int iElement = 29; iElement < s_NELEMENT; ++iElement) {
          handy(cal, iLayer, iElement);
        }
      } else ATH_MSG_WARNING("Unknown calorimeter module " << cal);
    } else ATH_MSG_ERROR("No LUT setup for calorimeter type " << cal);
  }
}

/** InitSatBCID method uses pulse profile and saturation threshold to compute
    thresholds for saturated pulse BCID
    CHECK: Currently the nominal (i.e. non-randomly spread) pedestal is taken in the calculation. */
void Run2TriggerTowerMaker::initSatBCID(TowerTypes cal, const std::vector<double>& Pulse)
{
  ATH_MSG_DEBUG("Initialise SatBCID for pulse type " << cal);

  int layer = 0;
  if(cal >= Tile) layer = 1;
  auto range = calToElementRange(cal);

  // Calculate SatLow and SatHigh
  double scale = m_SatLevel - m_pedVal;
  double margin = 10.*s_MEV/m_adcStep; // 10 GeV margin for noise
  ATH_MSG_DEBUG("SatBCID: SatLevel = " << m_SatLevel
                << "; scale = " << scale
                << "; margin = " << margin);

  // information about the pulse
  auto nSamples = Pulse.size();
  int satLow = int(m_pedVal);
  int satHigh = int(m_pedVal);
  // Calculate SatLow and SatHigh for the Pulse shape if enough information is available
  if(nSamples >= 5) {
    // Assumes pulse peaks in centre of vector
    auto peak = nSamples/2;

    /* Calculate thresholds. When peak starts to saturate, peak-1 and peak-2
       should pass their respective thresholds. However, we don't want it
       to be possible for noise fluctuations to pass SatLow (peak-2 threshold) */
    // Ideal amplitudes of preceeding samples when satuation begins
    double minusOne = Pulse[peak-1]*scale + m_pedVal;
    double minusTwo = Pulse[peak-2]*scale + m_pedVal;

    /* SatHigh setting:
       Most important thing is that this one is passed when saturation begins.
       So set to amp of peak-1 when saturation starts - margin
       Should be OK as pulse shape should be undistorted to this point */
    satHigh = int(minusOne - margin);
    if(satHigh <  m_pedVal+margin) {
      ATH_MSG_DEBUG("SatHigh[" << cal << "] has lower level than I'd like");
    }

    /* SatLow setting:
       This one can cause problems either if it is ever failed by peak-2
       OR if noise causes it to be passed by peak-3.
       Expect the former to be the bigger threat, so set accordingly, but
       warn if the latter seems to be possible. */
    satLow = int(minusTwo - margin);
    if(satLow <  m_pedVal+margin) {
      ATH_MSG_DEBUG("SatLow[" << cal << "] has lower level than I'd like");
      if(satLow < 0) satLow = 0;
    }

    // For info, print out the pulse shape and thresholds
    if(msgLvl(MSG::DEBUG)) {
      for(auto sample = peak-2; sample < peak+3; ++sample) {
        ATH_MSG_DEBUG("Pulse sample " << sample << " = " << Pulse[sample]);
      }
      ATH_MSG_DEBUG("SatLow[" << cal << "]  = " << satLow);
      ATH_MSG_DEBUG("SatHigh[" << cal << "] = " << satHigh);
    }
  } else {
    // insufficient samples in pulse - use defaults!
    ATH_MSG_WARNING("Only " << nSamples << " samples in pulse!! ");
    ATH_MSG_WARNING("Saturated pulse BCID should not be used");
  }

  // assign values to elements and calculate levels
  for(int iElem = range.first; iElem < range.second; ++iElem) {
    ElementInfo& ei = m_elementInfo[layer][iElem];

    // If DecisionSource = FIR, need to convert ET ranges -> FIR values for tower type
    // Otherwise assume they have been set correctly in ADC counts
    if(m_DecisionSource & 0x1) { // Use FIR for decision
      if(m_EnergyLow > 0) {
        ei.EnergyLow = int(m_EnergyLow*ei.FIRCal + ei.FIRped);
      } else {
        ei.EnergyLow = 0;
      }
      if(m_EnergyHigh > 0) {
        ei.EnergyHigh = int(m_EnergyHigh*ei.FIRCal + ei.FIRped);
      } else {
        ei.EnergyHigh = 0;
      }
      ATH_MSG_DEBUG("Decision ranges (FIR) = " << ei.EnergyLow
                    << "; " << ei.EnergyHigh);
    } else { // Use ADC for decision
      ei.EnergyLow = int(m_EnergyLow*m_digitScale/m_adcStep + m_pedVal);
      ei.EnergyHigh = m_SatLevel;
      ATH_MSG_DEBUG("Decision ranges (ADC counts) = " << ei.EnergyLow
                    << "; " << ei.EnergyHigh);
    }

    // Now to set thresholds for decision logic:
    // Have thresholds already been set? If so, don't override.
    if(m_SatHighElement[layer][iElem] > 0 || m_SatLowElement[layer][iElem] > 0) {
      ATH_MSG_DEBUG("SatBCID thresholds already set: SatLow = " << m_SatLowElement[layer][iElem]
                    << "; SatHigh = " << m_SatHighElement[layer][iElem]);
      continue;
    }

    // otherwise set them to the previously calculated values
    ei.SatLow = satLow;
    ei.SatHigh = satHigh;
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

  if(m_xOutputLocation.size()>0) {
    m_xaodTowers.reset(new xAOD::TriggerTowerContainer);
    m_xaodTowersAux.reset(new xAOD::TriggerTowerAuxContainer);
    m_xaodTowers->setStore(m_xaodTowersAux.get());
    m_xaodTowers->resize(7168); // avoid frequent reallocations
    m_curIndex = 0u;
  }

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

  if(!m_outputLocation.empty()) {
    std::unique_ptr<t_TTCollection> VectorOfTTs(new t_TTCollection);
    VectorOfTTs->reserve(7168/2); // em+had combined

    for(auto& it : m_TTMap) {
      ATH_MSG_DEBUG("TT has coords (" << it.second->phi() << ", " << it.second->eta()
                    << " and energies : " << it.second->emEnergy() << ", "
                    << it.second->hadEnergy() << " (Em,Had)");
      VectorOfTTs->push_back(it.second.release());
    }
    m_TTMap.clear();
    ATH_MSG_VERBOSE(VectorOfTTs->size() << " TTs have been generated");

    CHECK(evtStore()->overwrite(VectorOfTTs.release(), m_outputLocation, true, false, false));
    ATH_MSG_DEBUG("Stored TTs in TES at " << m_outputLocation);
  }

  if(m_ZeroSuppress) {
    // remove trigger towers whose energy is 0
    m_xaodTowers->erase(std::remove_if(m_xaodTowers->begin(), m_xaodTowers->end(),
                                       [](const xAOD::TriggerTower* tt){
                                         return tt->cpET() == 0 && tt->jepET() == 0;
                                       }),
                        m_xaodTowers->end());
  }

  if(!m_xOutputLocation.empty()) {
    CHECK(evtStore()->record(m_xaodTowers.release(), m_xOutputLocation));
    CHECK(evtStore()->record(m_xaodTowersAux.release(), m_xOutputLocation+"Aux."));
  }

  return StatusCode::SUCCESS;
} // end of LVL1::Run2TriggerTowerMaker::store(){


/** gets TriggerTowers from input collection and copies ADC digits into
    xAOD::TriggerTowers for reprocessing */
StatusCode LVL1::Run2TriggerTowerMaker::getTriggerTowers()
{
  /// Find  TriggerTowers in TES
  const t_TTCollection* inputTTs=0;

  ATH_MSG_DEBUG("Retrieve input TriggerTowers " << m_inputTTLocation);
  CHECK(evtStore()->retrieve(inputTTs, m_inputTTLocation));
  ATH_MSG_VERBOSE("Found " << inputTTs->size() << " input TriggerTowers");

  for(const auto& tower : *inputTTs) {
    // em layer
    // m_xaodTowers->push_back(new xAOD::TriggerTower);
    // auto t = m_xaodTowers->back();
    auto t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
    t->setCoolId(channelId(tower->eta(), tower->phi(), 0).id());
    t->setEta(tower->eta());
    t->setPhi(tower->phi());
    t->setAdc(std::vector<uint_least16_t>(std::begin(tower->emADC()), std::end(tower->emADC())));
    t->setAdcPeak(t->adc().size()/2);

    // had layer
    // m_xaodTowers->push_back(new xAOD::TriggerTower);
    // t = m_xaodTowers->back();
    t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
    t->setCoolId(channelId(tower->eta(), tower->phi(), 1).id());
    t->setEta(tower->eta());
    t->setPhi(tower->phi());
    t->setAdc(std::vector<uint_least16_t>(std::begin(tower->hadADC()), std::end(tower->hadADC())));
    t->setAdcPeak(t->adc().size()/2);
  }

  /// If < 7168 towers in input data will be unallocated pointers in vector.
  /// So clean-up m_xaodTowers before these cause problems later.
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
    ATH_MSG_ERROR("Can't find calo towers - stopping processing" << endreq
                  << "Found Em  LArTTL1 : "<<sc1 << endreq
                  << "Found Had LArTTL1 : "<<sc2 << endreq
                  << "Found TileTTL1    : "<<sc3<< endreq
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
  /// So clean-up m_xaodTowers before these cause problems later.
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
    // m_xaodTowers->push_back(new xAOD::TriggerTower);
    // auto t = m_xaodTowers->back();
    auto t = (*m_xaodTowers)[m_curIndex++] = new xAOD::TriggerTower;
    t->setCoolId(coolId.id());
    t->setEta(eta);
    t->setPhi(phi);
    m_xaodTowersAmps[t->index()] = std::move(amps);

    TriggerTowerKey testKey(0.0, 0.0);
    ATH_MSG_VERBOSE("Add to tower at (" << t->eta() << ", " << t->phi()
                    << "), key = " << TriggerTowerKey().ttKey(phi,eta));
  } // end for loop
}

void LVL1::Run2TriggerTowerMaker::processTileTowers(const TileTTL1Container * towers)
{
  // Step over all towers
  TileTTL1Container::const_iterator tower;
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
    tower->setAdc(std::vector<uint_least16_t>(std::begin(digits), std::end(digits)));
    tower->setAdcPeak(digits.size()/2);
  }
}

void LVL1::Run2TriggerTowerMaker::preProcessTower(xAOD::TriggerTower *tower, int m_eventBCID)
{
  // Use eta coordinate of tower to deduce type
  auto layer = tower->layer();

  // vectors to store intermediate results of preprocessing
  std::vector<int> fir;
  std::vector<int_least16_t> correction;
  std::vector<int> lutIn;
  std::vector<int> lutOut_jep;
  std::vector<int> lutOut_cp;
  std::vector<int> BCIDOut;

  /// retrieve digits
  std::vector<int> digits(std::begin(tower->adc()), std::end(tower->adc()));

  double pedvalue = m_pedVal;
  int pedsub = int(m_pedVal);
  auto itPed = m_pedestals.find(tower->coolId());
  if(itPed != m_pedestals.end()) {
    pedvalue = itPed->second.val;
    pedsub = itPed->second.sub;
  }
  else ATH_MSG_ERROR("No pedestal entry for channel " << tower->coolId());

  /// Map integer eta bin to element
  int iElement = etaToElement(tower->eta(), layer); // element range (0-32)
  ElementInfo& element = m_elementInfo[layer][iElement];

  //FIXME: Left this in because old code tries to use same offset for positive
  //and negative sides... not correct though just comment out to 'fix' CHECK
  if(layer == 1 && iElement > 28 && iElement < 33) {
    pedsub = element.offset;
  }
  int slope = element.slope;
  int thresh_cp = element.thresh_cp;
  int thresh_jep = element.thresh_jep;

  int firPed = element.FIRsum * floor(pedvalue + 0.5);

  /// process tower
  m_TTtool->fir(digits, element.FIRCoeff, fir);
  if(m_decorateFIR) firDecorator(*tower) = fir[fir.size()/2];
  if(m_correctFir) {
    m_TTtool->pedestalCorrection(fir, firPed, iElement, layer, m_eventBCID,
                                 m_lumiBlockMuTool->actualInteractionsPerCrossing(), correction);
  }
  m_TTtool->dropBits(fir, element.nDrop, lutIn);

  if(m_LUTStrategy == 1) {
    // for new strategy slope, pedsub and thresh are in units of LUTOut
    // and need to be multiplied by the scale factor

    // CP Lut
    m_TTtool->lut(lutIn, m_cpLutScale*slope, m_cpLutScale*pedsub, m_cpLutScale*thresh_cp,
                  int(m_pedVal), m_LUTStrategy, false, lutOut_cp);
    // JEP Lut
    m_TTtool->lut(lutIn, m_jepLutScale*slope, m_jepLutScale*pedsub, m_jepLutScale*thresh_jep,
                  int(m_pedVal), m_LUTStrategy, false, lutOut_jep);
  } else if(m_LUTStrategy == 0) {
    // for old strategy pedsub and thresh are in units of LUTIn and *do not*
    // need to be multiplied by the scale factor

    // CP Lut
    m_TTtool->lut(lutIn, m_cpLutScale*slope, pedsub, thresh_cp, int(m_pedVal), m_LUTStrategy, false, lutOut_cp);
    // JEP Lut
    m_TTtool->lut(lutIn, m_jepLutScale*slope, pedsub, thresh_jep, int(m_pedVal), m_LUTStrategy, false, lutOut_jep);
  }
  m_TTtool->bcid(fir, digits, m_PeakFinderCond, element.SatLow, element.SatHigh, m_SatLevel, BCIDOut);

  //ATH_MSG_VERBOSE("Tower BCID results for (" << tower->eta() << ", " << tower->phi() << "):");
  //for (unsigned int i = 0; i < digits.size(); ++i) {
  //  ATH_MSG_VERBOSE("   digit: " << digits[i] << ", BCID " << BCIDOut[i]);
  //}

  unsigned peak = lutOut_jep.size()/2; // both cp & jep have the same length
  std::vector<uint_least8_t> etResultVectorJep { uint_least8_t(lutOut_jep[peak]) };
  std::vector<uint_least8_t> etResultVectorCp { uint_least8_t(lutOut_cp[peak]) };

  // identify BCID range
  int range;
  if(!m_DecisionSource&0x1) {
    range = EtRange(digits[3], element);
  } else {
    range = EtRange(fir[peak], element);
  }

  // correct BCID for this range?
  if(m_BcidDecision[range]&(0x1<<BCIDOut[peak])) {
    if(m_SatOverride[range]&0x1) {
      // return saturation if set
      etResultVectorJep[0] = m_SaturationValue;
      etResultVectorCp[0] = m_SaturationValue;
    }
  }
  else {
    // zero if fail BCID
    etResultVectorJep[0] = 0;
    etResultVectorCp[0] = 0;
  }

  tower->setLut_cp(std::move(etResultVectorCp));
  tower->setLut_jep(std::move(etResultVectorJep));
  tower->setBcidVec({ uint_least8_t(BCIDOut[peak]) });
  tower->setPeak(0u); // we only added one item to etResultVector

  /// In simulation external BCID is always zero, but for consistency with
  /// data we need to add it to the TriggerTower objects
  tower->setBcidExt(std::vector<uint_least8_t>(tower->adc().size(), 0u));

  // fill the pedestal correction
  if(m_correctFir) {
    tower->setCorrectionEnabled(std::vector<uint_least8_t>(tower->lut_cp().size(), 1u));
    tower->setCorrection(std::vector<int_least16_t>(tower->lut_cp().size(), correction[peak]));
  } else {
    tower->setCorrectionEnabled(std::vector<uint_least8_t>(tower->lut_cp().size(), 0u));
    tower->setCorrection(std::vector<int_least16_t>(tower->lut_cp().size(), 0u));
  }
}

/** Emulate FIR filter, bunch-crossing identification & LUT, and create & fill
    TriggerTowers. */
StatusCode LVL1::Run2TriggerTowerMaker::preProcess()
{
  // Pedestal Correction: Get the BCID number
  const xAOD::EventInfo* evt;
  CHECK(evtStore()->retrieve(evt));
  auto m_eventBCID = evt->bcid();

  // needed as index to add the two xAOD::TriggerTowers to one TriggerTower
  TriggerTowerKey ttkey;

  // Loop over all existing towers and simulate preprocessor functions
  for(auto tower : *m_xaodTowers) {
    preProcessTower(tower, m_eventBCID);

    // only create legacy TriggerTowers if we intend to store them later on
    if(m_outputLocation.empty()) continue;

    // If tower has passed BCID and threshold, create TriggerTower and
    // add it to the map
    if(tower->cpET() > 0 || tower->jepET() > 0 || (!m_ZeroSuppress)) {
      int key = ttkey.ttKey(tower->phi(), tower->eta());
      auto it = m_TTMap.find(key);
      if(it == std::end(m_TTMap)) {
        // not yet in map - add
        it = m_TTMap.insert(std::make_pair(key, std::unique_ptr<TriggerTower>(new TriggerTower(tower->phi(), tower->eta(), key)))).first;
      }

      // get pointer to member function depending on layer
      auto add = (tower->layer() ? &TriggerTower::addHad : &TriggerTower::addEM);

      // missing const-specifiers in add(EM/Had) make creation of temporaries necessary ...
      std::vector<int> adc(std::begin(tower->adc()), std::end(tower->adc()));
      std::vector<int> lut(std::begin(tower->lut_cp()), std::end(tower->lut_cp()));
      std::vector<int> bcidExt(std::begin(tower->bcidExt()), std::end(tower->bcidExt()));
      std::vector<int> bcidVec(std::begin(tower->bcidVec()), std::end(tower->bcidVec()));

      // call member function - wierd syntax due to unique_ptr and pair
      ((it->second).get()->*add)(adc, lut, bcidExt, bcidVec, 0, tower->peak(), tower->adcPeak());
    }
  }
  return StatusCode::SUCCESS;
}

} // end of namespace bracket

std::vector<int> LVL1::Run2TriggerTowerMaker::ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps)
{
  double adcCal = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,1.,m_gainCorr);
  double ped = m_pedVal; // Give it a default in case of trouble

  auto itPed = m_pedestals.find(channel.id());
  if(itPed != m_pedestals.end()) ped = itPed->second.val;

  std::vector<int> digits;
  int nSamples = amps.size();
  digits.reserve(nSamples);
  for(int i=0; i<nSamples; i++) {
    double adcNoise = CLHEP::RandGaussZiggurat::shoot(m_rndmADCs,0.,m_adcVar);
    int digit = int((amps[i]*adcCal/m_adcStep) + ped + adcNoise);
    if(digit > m_adcMax) digit = m_adcMax;
    if(digit < 0)        digit = 0;
    digits.push_back(digit);
  }
  return digits;
}

int LVL1::Run2TriggerTowerMaker::EtRange(int et, const ElementInfo& ei)
{
  if(et < ei.EnergyLow)  return 0;
  if(et < ei.EnergyHigh) return 1;
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

std::pair<float,float> LVL1::Run2TriggerTowerMaker::elementToEta(int element, int layer) const
{
  if(element < 25) {
    float eta = (0.1f * element) + 0.05f;
    return std::make_pair(-eta, eta);
  }

  // elements (everything in coolid maps) provided in 2-1,3-1,2-2,3-2 pattern,
  // but tower etap is: 2-1,3-1,2-2,3-2, etam is: 2-2,3-2,2-1,3-1 (left to right)
  switch(element) {
  case 25: return std::make_pair(-2.6f, 2.6f);
  case 26: return std::make_pair(-2.8f, 2.8f);
  case 27: return std::make_pair(-3.0f, 3.0f);
  case 28: return std::make_pair(-3.15f, 3.15f);
  case 29: return layer ? std::make_pair(-4.2625f, 3.4125f) : std::make_pair(-3.4125f, 3.4125f);
  case 30: return layer ? std::make_pair(-4.6875f, 3.8375f) : std::make_pair(-4.2625f, 4.2625f);
  case 31: return layer ? std::make_pair(-3.4125f, 4.2625f) : std::make_pair(-3.8375f, 3.8375f);
  case 32: return layer ? std::make_pair(-3.8375f, 4.6875f) : std::make_pair(-4.6875f, 4.6875f);
  default:
    ATH_MSG_ERROR("element out of range");
    return std::make_pair(0.f, 0.f);
  }
}

// Pedestal Correction
int LVL1::Run2TriggerTowerMaker::etaToElement(float feta, int layer) const
{
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
  element = s_NELEMENT-element-1;

  return element;
}

std::pair<int, int> LVL1::Run2TriggerTowerMaker::calToElementRange(TowerTypes cal) const
{
  switch(cal) {
    case EmB: return std::make_pair(0, 15);
    case EmEC: return std::make_pair(15, 29);
    case FcalE: return std::make_pair(29, 33); // doesn't compile with s_NELEMENT in debug build ?!
    case Tile: return std::make_pair(0, 15);
    case Hec: return std::make_pair(15, 29);
    case FcalH: return std::make_pair(29, 33); // doesn't compile with s_NELEMENT in debug build ?!
    default:
      ATH_MSG_ERROR("default-clause should be unreachable");
      assert(false); return std::make_pair(0, 0);
  };
  // not reachable
}

LVL1::Run2TriggerTowerMaker::TowerTypes LVL1::Run2TriggerTowerMaker::elementToCal(int layer, int element) const
{
  TowerTypes cal;
  if(element < 15 && layer==0) cal = EmB;
  else if(element < 15 && layer==1) cal = Tile;
  else if(element < 29 && layer==0) cal = EmEC;
  else if(element < 29 && layer==1) cal = Hec;
  else if(layer==0) cal = FcalE;
  else if(layer==1) cal = FcalH;
  else {
      ATH_MSG_ERROR("else-clause should be unreachable");
      assert(false); cal = EmB;
  }
  return cal;
}

/** initLUTsElement method uses FIR coefficients and pulse profile to compute
    LUT calibration parameters.
*/
void LVL1::Run2TriggerTowerMaker::initLUTsElement(TowerTypes cal, const std::vector<double>& pulse)
{
  int layer = 0;
  if(cal >= Tile) layer = 1;

  auto range = calToElementRange(cal);
  for(int iElem = range.first; iElem < range.second; ++iElem) {
    ElementInfo& ei = m_elementInfo[layer][iElem];

    double FIRcal = 0.;
    int FIRsum = 0;
    int FIRsat = 0;
    for(int sample = 0; sample < s_FIRLENGTH; sample++) {
        FIRsum += ei.FIRCoeff[sample];
        FIRcal += ei.FIRCoeff[sample]*pulse[sample]*s_MEV/m_adcStep;
        FIRsat += int(ei.FIRCoeff[sample]*((pulse[sample]*(1023-m_pedVal)) + m_pedVal));

        ATH_MSG_INFO(" FIRco[" << sample << "] = " << ei.FIRCoeff
                     << "  ideal pulse amplitude = " << pulse[sample]
                     << "  element = " << iElem);
      }
    double FIRped = FIRsum*m_pedVal;

    ATH_MSG_DEBUG("Full precision: FIRcal = " << FIRcal
                  << " FIRsat = " << FIRsat << " FIRped = " << FIRped);

    // Identify 10 bit range to use in LUT
    int bitMax = 0;
    for(int i = 0; i < 16; i++) {
      if(FIRsat > (1<<i)) bitMax++; // CHECK: simplify
    }
    ei.nDrop = bitMax - 10; // keep a 10 bit range below 2**bitMAx
    if(ei.nDrop < 0) ei.nDrop = 0; // should not be possible anyway

    ATH_MSG_DEBUG("Highest bit used = " << bitMax
                  << " bits to drop = " << ei.nDrop);

    // Save the unshifted values for FIR energy range calibration
    ei.FIRCal = int(FIRcal+0.5);
    ei.FIRped = int(FIRped+0.5);

    // for LUT operations on selected 10 bits
    FIRcal /= double(1<<ei.nDrop); // divide by 2**(ei.nDrop)

    int slope  = 1;
    if(m_AutoCalibrateLUT) slope = int((4096./FIRcal)*(s_MEV/m_digitScale));
    ei.slope = slope;
    ei.FIRsum = FIRsum;

    ei.offset = 0; // for FCal23 mapping
  }
}
