//////////////////////////////////////////////////////////////////////
//  L1TriggerTowerTool.cxx (c) Alan Watson
///////////////////////////////////////////////////////////////////////
#include "TrigT1CaloTools/L1TriggerTowerTool.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibConditions/FineTimeErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefs.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"

#include "TrigT1CaloCalibConditions/L1CaloDerivedRunPars.h"
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunParsContainer.h"

#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategy.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategyContainer.h"

#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloToolInterfaces/IL1DynamicPedestalProvider.h"

#include <cstdint>
#include <tuple>

namespace LVL1 
{

//================ Static Constants ============================================

const int L1TriggerTowerTool::s_saturationValue;
const int L1TriggerTowerTool::s_maxTowers;

//================ Constructor =================================================

L1TriggerTowerTool::L1TriggerTowerTool(const std::string& t,
                          const std::string& n,
                          const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_caloMgr(0),
  m_lvl1Helper(0),
  m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
  m_ttSvc("CaloTriggerTowerService/CaloTriggerTowerService"),
  m_mappingTool("LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool"),
  m_l1CondSvc("L1CaloCondSvc", n),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", n),
  m_isRun2(false),
  m_dbFineTimeRefsTowers(0),
  m_correctFir(false)
{
  declareInterface<IL1TriggerTowerTool>(this);

  declareProperty( "BaselineCorrection", m_correctFir );
  declareProperty( "L1DynamicPedestalProvider", m_dynamicPedestalProvider );
  declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

//================ Destructor =================================================

L1TriggerTowerTool::~L1TriggerTowerTool()
{
}

//================ Initialisation =================================================

StatusCode L1TriggerTowerTool::initialize()
{
  m_debug = msgLvl(MSG::VERBOSE); // May want to make this VERBOSE!


  CHECK(m_l1CondSvc.retrieve());
  CHECK(m_configSvc.retrieve());
  CHECK(m_l1CaloTTIdTools.retrieve());

  if(!m_ttSvc.retrieve().isSuccess()) {
    ATH_MSG_WARNING( "Could not retrieve CaloTriggerTowerService Tool" );
  } else {
    ATH_MSG_INFO("Retrieved Tool " << m_ttSvc);
  }
  
  StatusCode scID = detStore()->retrieve(m_caloMgr);
  if (scID.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve m_caloMgr" );
  } else {
    m_lvl1Helper = m_caloMgr->getLVL1_ID();
  }
 
  StatusCode sc = m_mappingTool.retrieve(); 
  if (sc.isFailure()) { 
    ATH_MSG_WARNING( "Failed to retrieve tool " << m_mappingTool ); 
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_mappingTool ); 
  }

  // Incident Service:
  IIncidentSvc* incSvc = 0;
  sc = service("IncidentSvc", incSvc);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve pointer to IncidentSvc " );
    //return StatusCode::FAILURE;
  }

  // Pedestal Correction
  if (m_correctFir) {
    CHECK(m_dynamicPedestalProvider.retrieve());
    ATH_MSG_INFO( "Retrieved L1DynamicPedestalProvider: " << m_dynamicPedestalProvider ); 
  }

  //start listening to "BeginRun"
  if (incSvc) incSvc->addListener(this, "BeginRun");
  
  ATH_MSG_INFO( "Initialization completed" );
  
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1TriggerTowerTool::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Reset mapping table at start of run ============================

void L1TriggerTowerTool::handle(const Incident& inc)
{
  if (inc.type()=="BeginRun") {
    ATH_MSG_DEBUG( "Resetting mapping table at start of run" );
   
    m_idTable.clear();

    const EventInfo* pevt = 0; // pointer for the event
    StatusCode status = evtStore()->retrieve(pevt); // retrieve the pointer to the event
    if(!status.isSuccess() || pevt == 0) {
      ATH_MSG_WARNING("Cannot determine run");
      return;
    }
    // determine whether this is Run-1 or Run-2 to get the correct conditions later on
    EventID* pei = pevt->event_ID();
    EventType* pet = pevt->event_type();
    if(pei == 0 || pet == 0) {
      ATH_MSG_WARNING("Cannot determine run");
      return;
    }
    if(pet->test(EventType::IS_SIMULATION) || pei->run_number() >= 253377) {
        m_isRun2 = true;
    } else {
        m_isRun2 = false;
    }
    ATH_MSG_INFO("Determined run to be from " << (m_isRun2 ? "Run-2" : "Run-1"));
  }
  return;
}

//================= Now the actual user calls ===================================

namespace { // helper function
  template<class T>
  StatusCode retrieveGeneric(ServiceHandle<L1CaloCondSvc>& svc, boost::any& target) {
    T* C = nullptr;
    CHECK_WITH_CONTEXT(svc->retrieve(C), "L1TriggerTowerTool");
    target = C;
    return StatusCode::SUCCESS;
  }

  template<class T, class FolderMap>
  StatusCode retrieveGenericWithFolders(ServiceHandle<L1CaloCondSvc>& svc, const FolderMap& fmap, boost::any& target) {
    T* C = nullptr;
    CHECK_WITH_CONTEXT(svc->retrieve(C, fmap), "L1TriggerTowerTool");
    target = C;
    return StatusCode::SUCCESS;
  }

} // anonymous namespace

/** Retrieve pointers to the L1Calo conditions containers */
StatusCode L1TriggerTowerTool::retrieveConditions()
{
  ATH_MSG_VERBOSE("L1TriggerTowerTool::retrieveConditions");
  if (m_l1CondSvc) {
    ATH_MSG_VERBOSE( "Retrieving Conditions Containers" );
    bool verbose = msgLvl(MSG::VERBOSE);

    if(m_isRun2) {
      CHECK_WITH_CONTEXT(m_l1CondSvc->retrieve(m_derivedRunParsContainer), "L1TriggerTowerTool");
      if (std::cbegin(*m_derivedRunParsContainer) == std::cend(*m_derivedRunParsContainer)) {
        ATH_MSG_WARNING("Empty L1CaloDerivedRunParsContainer");
        return StatusCode::FAILURE;
      }

      CHECK_WITH_CONTEXT(m_l1CondSvc->retrieve(m_runParametersContainer), "L1TriggerTowerTool");
      if (std::cbegin(*m_runParametersContainer) == std::cend(*m_runParametersContainer)) {
        ATH_MSG_WARNING("Empty L1CaloRunParametersContainer");
        return StatusCode::FAILURE;
      }


      std::string timingRegime = std::cbegin(*m_derivedRunParsContainer)->timingRegime();

      CHECK_WITH_CONTEXT(m_l1CondSvc->retrieve(m_strategyContainer), "L1TriggerTowerTool");
      
      std::string strategy;
      for(const auto& it: *m_strategyContainer){
        if (it.timingRegime() == timingRegime){
          strategy = it.strategy();
        }
      }

      std::map<L1CaloPprConditionsContainerRun2::eCoolFolders, std::string> 
        coolFoldersKeysMap = {
           {
            L1CaloPprConditionsContainerRun2::ePprChanDefaults,
            "/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"
           }
         };
      
      if (strategy.empty()){
        coolFoldersKeysMap[L1CaloPprConditionsContainerRun2::ePprChanCalib] 
          = "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime + "/PprChanCalib";
      } else {
        coolFoldersKeysMap[L1CaloPprConditionsContainerRun2::ePprChanCalibCommon] = 
            "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime + "/PprChanCommon";
        coolFoldersKeysMap[L1CaloPprConditionsContainerRun2::ePprChanCalibStrategy] =  
            "/TRIGGER/L1Calo/V2/Calibration/" + timingRegime + "/PprChan" + strategy;
      }

      CHECK(retrieveGenericWithFolders<L1CaloPprConditionsContainerRun2>(
          m_l1CondSvc, coolFoldersKeysMap, m_conditionsContainer));

      CHECK(retrieveGeneric<L1CaloPprDisabledChannelContainerRun2>(m_l1CondSvc, m_disabledChannelContainer));
    } else {
      CHECK(retrieveGeneric<L1CaloPprConditionsContainer>(m_l1CondSvc, m_conditionsContainer));
      CHECK(retrieveGeneric<L1CaloPprDisabledChannelContainer>(m_l1CondSvc, m_disabledChannelContainer));
    }

    
    if(verbose) {
      ATH_MSG_VERBOSE( "Retrieved ConditionsContainer" );
      if(m_isRun2){
        boost::any_cast<L1CaloPprConditionsContainerRun2*>(m_conditionsContainer)->dump();
      } else{
        boost::any_cast<L1CaloPprConditionsContainer*>(m_conditionsContainer)->dump();
      }
    }

    if(verbose) {
      if(m_isRun2){
        ATH_MSG_VERBOSE( "Retrieved DerivedRunParsContainer" );
        m_derivedRunParsContainer->dump();
	ATH_MSG_VERBOSE( "Retrieved RunParametersContainer" );
	m_runParametersContainer->dump();
        ATH_MSG_VERBOSE( "Retrieved StrategyContainer" );
        m_strategyContainer->dump();
      }
    }

    ATH_MSG_VERBOSE( "Retrieved DisabledChannelContainer" );
    if (verbose) {
      if(m_isRun2)
        boost::any_cast<L1CaloPprDisabledChannelContainerRun2*>(m_disabledChannelContainer)->dump();
      else
        boost::any_cast<L1CaloPprDisabledChannelContainer*>(m_disabledChannelContainer)->dump();
    }

  } else {
    ATH_MSG_ERROR("Could not retrieve Conditions Containers");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** All-in-one routine - give it the ADC counts and TT identifier, and
    it returns the results */
void L1TriggerTowerTool::process(const std::vector<int> &digits, double eta, double phi, int layer,
             std::vector<int> &et, std::vector<int> &bcidResults,
             std::vector<int> &bcidDecisions, bool useJepLut /* = true */)
{
  /// Find channelID for this tower
  L1CaloCoolChannelId id = channelID(eta, phi, layer);

  /// then process the tower
  process(digits, id, et, bcidResults, bcidDecisions, useJepLut);
}


/** All-in-one routine - give it the ADC counts and TT identifier, and
    it returns the results */
void L1TriggerTowerTool::process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
                                 std::vector<int> &et, std::vector<int> &bcidResults,
                                 std::vector<int> &bcidDecisions, bool useJepLut /* = true */)
{
  if (m_debug) {
    ATH_MSG_VERBOSE( "::process: ==== Entered Process ====" );
    ATH_MSG_VERBOSE( "::process: digits: ");
    printVec(digits);
    ATH_MSG_VERBOSE( " channelID: " << MSG::hex << channelId.id() << MSG::dec );
  }
 
  /// Initialise
  et.clear();
  bcidResults.clear();
  bcidDecisions.clear();

  ATH_MSG_VERBOSE( "::process: ---- FIR filter ----" );
     
  /// emulate FIR filter
  std::vector<int> filter;
  fir(digits, channelId, filter);
  std::vector<int> lutInput;
  dropBits(filter, channelId, lutInput);

  ATH_MSG_VERBOSE( "::process: ---- BCID algorithms ----" );

  /// emulate the two BCID algorithms
  bcid(filter, digits, channelId, bcidResults);

  ATH_MSG_VERBOSE( "::process: ---- BCID decisions ----" );

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  bcidDecisionRange(lutInput, digits, channelId, decisionRange);
  bcidDecision(bcidResults, decisionRange, channelId, bcidDecisions);

  ATH_MSG_VERBOSE( "::process: ---- LUT ET calculation ----" );

  /// LUT ET calculation
  std::vector<int> lutOutput;
  if(m_isRun2) {
    if(useJepLut) jepLut(lutInput, channelId, lutOutput);
    else cpLut(lutInput, channelId, lutOutput);
  } else {
    lut(lutInput, channelId, lutOutput);
  }

  ATH_MSG_VERBOSE( "::process: ---- use ET range ----" );

  /// Use ET range to return appropriate ET value
  /// do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably
  applyEtRange(lutOutput, decisionRange, channelId, et);

  ATH_MSG_VERBOSE( "::process: ==== Leaving Process ====" );
}

namespace {
// helper function to convert vectors of different type
template <typename DST, typename SRC>
std::vector<DST> convertVectorType(const std::vector<SRC>& s) {
   std::vector<DST> d(s.size());
   std::transform(std::begin(s), std::end(s), std::begin(d),
      [](SRC v){return static_cast<DST>(v);});
   return d;
} 
}


/** All-in-one routine - give it the TT identifier, and  it returns the results */
void L1TriggerTowerTool::simulateChannel(const xAOD::TriggerTower& tt, std::vector<int>& outCpLut, std::vector<int>& outJepLut, std::vector<int>& bcidResults, std::vector<int>& bcidDecisions)
{

  //If we have 80 MHz readout, we need to extract the 40 MHz samples. The central 80 MHz sample is always a 40 MHz sample. We use the cool database (runParameters folder) to understand if we are in 80MHz readout

  unsigned int readoutConfigID   = std::cbegin(*m_runParametersContainer)->readoutConfigID();

  if(m_debug){
    ATH_MSG_VERBOSE("ReadoutConfigID = " << readoutConfigID );
  }

  std::vector<uint16_t> digits40;

  if(readoutConfigID == 5 or readoutConfigID == 6){

    if(m_debug){
      ATH_MSG_VERBOSE("80 MHz readout detected, emulating 40 MHz samples");
    }
 
    int nSlices = tt.adc().size();

    for (int i=0 ; i < (nSlices-1)/2 ; i++ ){
      digits40.push_back(tt.adc().at(2*i+1));
    }

  }else{
    if(m_debug){
      ATH_MSG_VERBOSE("40 MHz readout detected");
    }
    digits40 = tt.adc();
  }
  
  const auto& digits = convertVectorType<int>(digits40);

  L1CaloCoolChannelId channelId {tt.coolId()}; 

  if (m_debug) {
    ATH_MSG_VERBOSE( "::simulateChannel: ==== Entered Process ====" );
    ATH_MSG_VERBOSE( "::simulateChannel: digits: ");
    printVec(digits);
    ATH_MSG_VERBOSE( "::simulateChannel: channelID: " << MSG::hex << channelId.id() << MSG::dec );
  }
  
  /// Initialise
  outCpLut.clear();
  outJepLut.clear();
  bcidResults.clear();
  bcidDecisions.clear();
      
  /// emulate FIR filter
  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- FIR filter ----" );
  std::vector<int> filter;
  fir(digits, channelId, filter);
  if (m_debug) printVec(filter);


  /// apply pedestal correction
  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- pedestalCorrection ----" );
  // the correction is only available for each LUT slice in the read-out (not ADC/Filter slice)
  // therefore we can only apply it to the #LUT central filter slices
  const std::size_t nCorr = tt.correctionEnabled().size();
  const std::size_t filterOffset = filter.size()/2 - nCorr/2;
  for(std::size_t iCorr = 0; iCorr < nCorr; ++iCorr) {
    filter[filterOffset + iCorr] -= tt.correction()[iCorr] * tt.correctionEnabled()[iCorr];
  }
  if (m_debug)  printVec(filter);

  std::vector<int> lutInput;
  dropBits(filter, channelId, lutInput);

  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- BCID algorithms ----" );

  /// emulate the two BCID algorithms
  bcid(filter, digits, channelId, bcidResults);

  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- BCID decisions ----" );

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  bcidDecisionRange(lutInput, digits, channelId, decisionRange);
  bcidDecision(bcidResults, decisionRange, channelId, bcidDecisions);

  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- LUT ET calculation ----" );

  /// LUT ET calculation
  std::vector<int> cpLutOutput, jepLutOutput;
  cpLut(lutInput, channelId, cpLutOutput);
  jepLut(lutInput, channelId, jepLutOutput);

  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ---- use ET range ----" );

  /// Use ET range to return appropriate ET value
  /// do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably
  applyEtRange(cpLutOutput, decisionRange, channelId, outCpLut);
  applyEtRange(jepLutOutput, decisionRange, channelId, outJepLut);

  if (m_debug) ATH_MSG_VERBOSE( "::simulateChannel: ==== Leaving Process ====" );
}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vector of predicted BCID result words */
    
void L1TriggerTowerTool::bcid(const std::vector<int> &filter, const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  // Get decision flags for the 2 BCID algorithms
  std::vector<int> peak;
  peakBcid(filter, channelId, peak);
  std::vector<int> sat;
  satBcid(digits, channelId, sat);

  output.clear();
  output.reserve(sat.size()); // avoid frequent reallocations

  std::vector<int>::iterator itpeak = peak.begin();
  std::vector<int>::iterator itsat = sat.begin();
  for ( ; itpeak!=peak.end() && itsat!=sat.end(); ++itpeak, ++itsat ) {
   output.push_back( (*itpeak<<2) + (*itsat<<1) );
  }
  if (m_debug) {
   ATH_MSG_VERBOSE( "::bcid: bcidResults: ");
   printVec(output);
  ATH_MSG_VERBOSE(" ");
  }
}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vector of predicted BCID result words */
    
void L1TriggerTowerTool::bcid(const std::vector<int> &filter, const std::vector<int> &digits,
                                          unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &output)
{
  // Get decision flags for the 2 BCID algorithms
  std::vector<int> peak;
  peakBcid(filter, strategy, peak);
  std::vector<int> sat;
  satBcid(digits, satLow, satHigh, satLevel, sat);

  output.clear();
  output.reserve(sat.size()); // avoid frequent reallocations

  std::vector<int>::iterator itpeak = peak.begin();
  std::vector<int>::iterator itsat = sat.begin();
  for ( ; itpeak!=peak.end() && itsat!=sat.end(); ++itpeak, ++itsat ) {
   output.push_back( (*itpeak<<2) + (*itsat<<1) );
  }
  if (m_debug) {
   ATH_MSG_VERBOSE( "::bcid: bcidResults: ");
   printVec(output);
  ATH_MSG_VERBOSE(" ");
  }
}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vectors of predicted BCID result and decision words */
    
void L1TriggerTowerTool::bcid(const std::vector<int> &filter, const std::vector<int> &lutInput, const std::vector<int> &digits, int energyLow, int energyHigh, int decisionSource, std::vector<unsigned int> &decisionConditions,
                              unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &result, std::vector<int> &decision)
{
  // Get decision flags for the 2 BCID algorithms
  std::vector<int> peak;
  peakBcid(filter, strategy, peak);
  std::vector<int> sat;
  satBcid(digits, satLow, satHigh, satLevel, sat);

  result.clear();
  result.reserve(sat.size()); // avoid frequent reallocations
  decision.clear();

  std::vector<int>::iterator itpeak = peak.begin();
  std::vector<int>::iterator itsat = sat.begin();
  for ( ; itpeak!=peak.end() && itsat!=sat.end(); ++itpeak, ++itsat ) {
   result.push_back( (*itpeak<<2) + (*itsat<<1) );
  }
  if (m_debug) {
   ATH_MSG_VERBOSE( "::bcid: bcidResults: ");
   printVec(result);
  ATH_MSG_VERBOSE(" ");
  }

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  if (!decisionSource&0x1) etRange(digits, energyLow, energyHigh, decisionRange);
  else                     etRange(lutInput, energyLow, energyHigh, decisionRange);
  bcidDecision(result, decisionRange, decisionConditions, decision);
  if (m_debug) {
   ATH_MSG_VERBOSE( "::bcid: bcidDecisions: ");
   printVec(decision);
  ATH_MSG_VERBOSE(" ");
  }
}

namespace { // helper function
  template<class T>
  const std::vector<short int>* getFirCoefficients(unsigned int coolId, boost::any& C) {
    auto settings = boost::any_cast<T*>(C)->pprConditions(coolId);
    if(!settings) return nullptr;
    return &(settings->firCoefficients());
  }
} // anonymous namespace

/** This FIR simulation produces a vector of same length as digit vector,
    with peak positions corresponding. However, first 2 and last 2 FIR
    sums will be incomplete, and so are zeroed here */
void L1TriggerTowerTool::fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{   
  /// Get coefficients from COOL DB
  std::vector<int> firCoeffs;
  if(!m_conditionsContainer.empty()) {
    const std::vector<short int>* hwCoeffs;
    if(m_isRun2)
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), m_conditionsContainer);
    else
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainer>(channelId.id(), m_conditionsContainer);
    if(hwCoeffs) {
      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs->size()); // avoid frequent reallocations
      for (int i = hwCoeffs->size()-1; i >= 0; --i) firCoeffs.push_back((*hwCoeffs)[i]);

    } else ATH_MSG_WARNING( "::fir: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::fir: No Conditions Container retrieved" );

  if (m_debug) {
    ATH_MSG_VERBOSE( "::fir: FIR coefficients: ");
    printVec(firCoeffs);
    ATH_MSG_VERBOSE(" ");
  }

  fir(digits, firCoeffs, output);
}

/** This FIR simulation produces a vector of same length as digit vector, with
    peak positions corresponding. However, since there are less FIR coefficients
    than digits the first and last few sums will be incomplete, and so are
    zeroed here */
void L1TriggerTowerTool::fir(const std::vector<int> &digits, const std::vector<int> &firCoeffs, std::vector<int> &output)
{
  output.clear();
  output.reserve(digits.size()); // avoid frequent reallocations
  /** need to know first and last non-zero coefficient to know how which
      outputs can be calculated from supplied input digits */
  int firstFIR = -1;
  int lastFIR = 0;
  for (unsigned int i = 0; i < firCoeffs.size(); ++i) {
    if (firstFIR < 0 && firCoeffs[i] != 0) firstFIR = i;
    if (firCoeffs[i] != 0) lastFIR = i;
  }
  if (firstFIR < 0) firstFIR = lastFIR + 1;
  
  for (int i = 0; i < (int)digits.size(); i++) {
    int sum = 0;
    /// Only calculate where enough informations
    if (i >= 2-firstFIR && i < (int)digits.size()+2-lastFIR) {
      for (int j = firstFIR; j <= lastFIR; ++j) {
        sum += digits[i+j-2]*firCoeffs[j];
      }
    }
    if (sum < 0) sum = 0;
    output.push_back(sum);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::fir: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

namespace {
  template<typename T>
  unsigned int getStrategy(boost::any& C) {
    return boost::any_cast<T*>(C)->peakFinderCond();
  }
}

/** Peak finder BCID */
void L1TriggerTowerTool::peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& /*channelId*/, std::vector<int> &output)
{
  unsigned int strategy = 0;
  if(!m_conditionsContainer.empty()) {
    if(m_isRun2)
      strategy = getStrategy<L1CaloPprConditionsContainerRun2>(m_conditionsContainer);
    else
      strategy = getStrategy<L1CaloPprConditionsContainer>(m_conditionsContainer);
  } else ATH_MSG_WARNING( "::peakBcid: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::peakBcid: peak-finder strategy: " << strategy );
  
  peakBcid(fir, strategy, output);
}

/** Peak finder BCID */
void L1TriggerTowerTool::peakBcid(const std::vector<int> &fir, unsigned int strategy, std::vector<int> &output)
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations

  for (unsigned int i = 0; i < fir.size(); i++) {
    int result = 0;
    /// Cannot evaluate for first and last samples in vector
    if (i > 0 && i < fir.size()-1) {
      /// Select peak-finder strategy & test
      if (strategy&0x1) {
        if ( (fir[i-1]<fir[i]) && (fir[i+1]<fir[i]) ) result = 1;
      } else {
        if ( (fir[i-1]<fir[i]) && (fir[i+1]<=fir[i]) ) result = 1;
      }
    }
    output.push_back(result);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::peakBcid: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

namespace { // helper function
  template<class T>
  std::tuple<bool, int, int, int> getSaturation(unsigned int coolId, boost::any& C) {
    auto settings = boost::any_cast<T*>(C)->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0, 0, 0);
    return std::make_tuple(true, settings->satBcidLevel(), settings->satBcidThreshLow(), 
                           settings->satBcidThreshHigh());
  }
} // anonymous namespace

/** Saturated pulse BCID */
void L1TriggerTowerTool::satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{  
  int satLevel = 0;
  int satLow   = 0;
  int satHigh  = 0;
  if (!m_conditionsContainer.empty()) {
    bool available = false;
    if(m_isRun2)
      std::tie(available, satLevel, satLow, satHigh) = getSaturation<L1CaloPprConditionsContainerRun2>(channelId.id(), m_conditionsContainer);
    else
      std::tie(available, satLevel, satLow, satHigh) = getSaturation<L1CaloPprConditionsContainer>(channelId.id(), m_conditionsContainer);
    if(!available) ATH_MSG_WARNING( "::satBcid: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::satBcid: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::satBcid: satLevel: " << satLevel
      << " satLow: "  << satLow
      << " satHigh: " << satHigh );

  satBcid(digits, satLow, satHigh, satLevel, output);
}

/** Saturated pulse BCID */

void L1TriggerTowerTool::satBcid(const std::vector<int> &digits, int satLow, int satHigh, int satLevel, std::vector<int> &output)
{
  output.clear();
  output.reserve(digits.size()); // avoid frequent reallocations
  
  bool enabled = true;
  int flag[2] = {0,0};

  for (unsigned int i = 0; i<digits.size(); i++) {
    // Algorithm can set flag for following sample. So here we
    // propagate such flags into the current sample.
    flag[0] = flag[1];
    flag[1] = 0;
    
    if (digits[i]>=satLevel) {   // do we have saturation?
      if (enabled && i>1) {        // is algorithm active?
        bool low  = (digits[i-2]>satLow);
        bool high = (digits[i-1]>satHigh);
        if (high) {                             // flag current or next sample?
          if (low) {
            flag[0] = 1;
          }
          else {
            flag[1] = 1;
          }
        }
        else {
          flag[1] = 1;
        }
      }
      enabled = false;             // after first saturation, disable algorithm
    }
    else {
      enabled = true;              // unsaturated sample reenables algorithm
    }
    output.push_back(flag[0]);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::satBcid: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Evaluate BCID decision range */
namespace {
  template<typename T>
  unsigned int getDecisionSource(boost::any& C) {
    return boost::any_cast<T*>(C)->decisionSource();
  }
}

void L1TriggerTowerTool::bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  int decisionSource = 0;
  if (!m_conditionsContainer.empty()) {
    if(m_isRun2) decisionSource = getDecisionSource<L1CaloPprConditionsContainerRun2>(m_conditionsContainer);
    else decisionSource = getDecisionSource<L1CaloPprConditionsContainer>(m_conditionsContainer);

  } else ATH_MSG_WARNING( "::bcidDecisionRange: No Conditions Container retrieved" );

  if (!decisionSource&0x1) etRange(digits, channelId, output);
  else                     etRange(lutInput, channelId, output);
  if (m_debug) {
    ATH_MSG_VERBOSE( "::bcidDecisionRange: decisionSource: " << decisionSource);
    ATH_MSG_VERBOSE( " output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Evaluate BCID decision based on BCID word, ET range and channel ID */
namespace { // helper function
  template<class T>
  std::tuple<unsigned int, unsigned int, unsigned int> getBcidDecision(boost::any& C) {
    auto CC = boost::any_cast<T*>(C);
    return std::make_tuple(CC->bcidDecision1(), CC->bcidDecision2(), CC->bcidDecision3());
  }
} // anonymous namespace
void L1TriggerTowerTool::bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const L1CaloCoolChannelId& /*channelId*/, std::vector<int> &output)
{
  unsigned int decision1 = 0;
  unsigned int decision2 = 0;
  unsigned int decision3 = 0;
  if(!m_conditionsContainer.empty()) {
    if(m_isRun2)
      std::tie(decision1, decision2, decision3) = getBcidDecision<L1CaloPprConditionsContainerRun2>(m_conditionsContainer);
    else
      std::tie(decision1, decision2, decision3) = getBcidDecision<L1CaloPprConditionsContainer>(m_conditionsContainer);
  } else ATH_MSG_WARNING( "::bcidDecision: No Conditions Container retrieved" );

  // Reverse the order! (see elog 97082 9/06/10)
  std::vector<unsigned int> mask = { decision3, decision2, decision1 };
  
  ATH_MSG_VERBOSE( "::bcidDecision: masks: " << MSG::hex
         << decision3 << " " << decision2 << " " << decision1 << MSG::dec );

  bcidDecision(bcidResults, range, mask, output);
}

/** Evaluate BCID decision based on BCID word, ET range and decision mask */

void L1TriggerTowerTool::bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const std::vector<unsigned int> &mask, std::vector<int> &output)
{
  output.clear();
  output.reserve(bcidResults.size()); // avoid frequent reallocations

  std::vector<int>::const_iterator itBcid  = bcidResults.begin();
  std::vector<int>::const_iterator itRange = range.begin();
  int nRange = mask.size();

  for ( ; itBcid != bcidResults.end() && itRange != range.end(); ++itBcid, ++itRange) {
   if ((*itRange) < nRange && (mask[*itRange]&(0x1<<*itBcid))) output.push_back(1);
   else                                                        output.push_back(0);
  }
  if (m_debug) {
   ATH_MSG_VERBOSE( "::bcidDecision: output: ");
   printVec(output);
  ATH_MSG_VERBOSE(" ");
  }
}

/** LUT simulation: pedestal subtraction, energy calibration and threshold */
void L1TriggerTowerTool::lut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{   
  int strategy = 0;
  int offset   = 0;
  int slope    = 0;
  int cut      = 0;
  int ped      = 0;

  if(m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::lut: Run-2 data - behaviour undefined!");
  }

  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainer*>(m_conditionsContainer);
    const L1CaloPprConditions* settings = conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      strategy = settings->lutStrategy();
      offset   = settings->lutOffset();
      slope    = settings->lutSlope();
      cut      = settings->lutNoiseCut();
      ped      = settings->pedValue();
    } else ATH_MSG_WARNING( "::lut: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::lut: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::lut: LUT strategy/offset/slope/cut/ped: "
          << strategy << " " << offset << " " << slope << " " << cut << " " << ped << " " );

  unsigned int noiseCut = 0;
  bool disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;

  lut(fir, slope, offset, cut, ped, strategy, disabled, output);
}

// TODO implement scale
void L1TriggerTowerTool::cpLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{   
  int strategy = 0;
  int offset   = 0;
  int slope    = 0;
  int cut      = 0;
  unsigned short scale = 0;
  int ped      = 0;

  if(!m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::cpLut: Run-1 data - behaviour undefined!");
  }

  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainerRun2*>(m_conditionsContainer);
    const L1CaloPprConditionsRun2* settings = conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      strategy = settings->lutCpStrategy();
      offset   = settings->lutCpOffset();
      slope    = settings->lutCpSlope();
      cut      = settings->lutCpNoiseCut();
      scale    = settings->lutCpScale();
      ped      = settings->pedValue();
    } else ATH_MSG_WARNING( "::cpLut: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::cpLut: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::cpLut: LUT strategy/offset/slope/cut/ped: "
          << strategy << " " << offset << " " << slope << " " << cut << " " << ped << " " );

  unsigned int noiseCut = 0;
  bool disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
  if(strategy == 2) {
    // take the global scale into account - translate strategy to 1 for Run-1 compatible treatment
    lut(fir, scale*slope, scale*offset, scale*cut, ped, 1, disabled, output);
  } else if(strategy == 1 || strategy == 0){
    lut(fir, slope, offset, cut, ped, strategy, disabled, output);
  } else ATH_MSG_WARNING(" ::cpLut: Unknown stragegy: " << strategy);
}

void L1TriggerTowerTool::jepLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{   
  int strategy   = 0;
  int offset     = 0;
  int slope      = 0;
  int cut        = 0;
  unsigned short scale_db   = 0;
  unsigned short scale_menu = 0;
  int ped        = 0;
  short par1     = 0;
  short par2     = 0;
  short par3     = 0;
  short par4     = 0;

  if(!m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::jepLut: Run-1 data - behaviour undefined!");
  }

  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainerRun2*>(m_conditionsContainer);
    const L1CaloPprConditionsRun2* settings = conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      strategy   = settings->lutJepStrategy();
      offset     = settings->lutJepOffset();
      slope      = settings->lutJepSlope();
      cut        = settings->lutJepNoiseCut();
      ped        = settings->pedValue();
      scale_db   = settings->lutJepScale();
      scale_menu = m_configSvc->thresholdConfig()->caloInfo().globalJetScale(); // Retrieve scale param from menu instead of coolDB
      if (strategy == 3) {
        par1  = settings->lutJepPar1();
        par2  = settings->lutJepPar2();
        par3  = settings->lutJepPar3();
        par4  = settings->lutJepPar4();
      }
    } else ATH_MSG_WARNING( "::jepLut: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::jepLut: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::jepLut: LUT strategy/offset/slope/cut/ped: "
          << strategy << " " << offset << " " << slope << " " << cut << " " << ped << " " );

  unsigned int noiseCut = 0;
  bool disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;

  if(strategy == 3) {
    nonLinearLut(fir, slope, offset, cut, scale_db, par1, par2, par3, par4, disabled, output);
  } 
  else if(strategy == 2) {
    // take the global scale into account - translate strategy to 1 for Run-1 compatible treatment
    lut(fir, scale_menu*slope, scale_menu*offset, scale_menu*cut, ped, 1, disabled, output);
  }else if(strategy == 1 || strategy == 0) {
    lut(fir, slope, offset, cut, ped, strategy, disabled, output);
  } else ATH_MSG_WARNING(" ::jepLut: Unknown stragegy: " << strategy);
}

/** LUT simulation: pedestal subtraction, energy calibration and threshold */

void L1TriggerTowerTool::lut(const std::vector<int> &fir, int slope, int offset, int cut, int /*ped*/, int strategy, bool disabled, std::vector<int> &output)
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations
  
  std::vector<int>::const_iterator it = fir.begin();
  for ( ; it != fir.end(); ++it) {
    int out = 0;
    if (!disabled) {
      if (strategy == 0 && (*it) >= offset+cut) { // Original scheme
        out = (((*it)-offset)*slope + 2048)>>12;
      } else if (strategy == 1 && (*it)*slope >= offset+cut) { // New scheme
        out = ((*it)*slope - offset + 2048)>>12;
      }
      if (out < 0)                 out = 0;
      if (out > s_saturationValue) out = s_saturationValue;
    }
    output.push_back(out);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::lut: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  } 
}

void L1TriggerTowerTool::nonLinearLut(const std::vector<int> &fir, int slope, int offset, int cut, int scale, short par1, short par2, short par3, short par4, bool disabled, std::vector<int> &output)
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations

  std::vector<int>::const_iterator it = fir.begin();
  for ( ; it != fir.end(); ++it) {
    int out = 0;
    if (!disabled) {
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
      if ((*it) * slope < offset + nll_noise * cut) {
        output.push_back(0);
        continue;
      }
      // actual calculation
      out = int((((int)(2048 + nll_slope * ((*it) * slope - offset)))>>12) + nll_offset + nll_ampl * std::exp(nll_expo * ((*it) * slope - offset)));

      if(out > s_saturationValue) out = s_saturationValue;
      if(out < 0)                 out = 0;
    }
    output.push_back(out);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::nonLinearLut: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  } 
}
/** Use ET range to return appropriate ET value
    Do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably */

void L1TriggerTowerTool::applyEtRange(const std::vector<int>& lut, const std::vector<int>& range, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  bool disabled = disabledChannel(channelId);
  std::vector<int>::const_iterator itlut   = lut.begin();
  std::vector<int>::const_iterator itrange = range.begin();
  while ( itlut != lut.end() && itrange != range.end() ) {
    if (!disabled && satOverride((*itrange), channelId)) output.push_back(s_saturationValue);
    else                                                 output.push_back(*itlut);
    ++itlut;
    ++itrange;
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::applyEtRange: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Identify BCID decision range */
namespace { // helper function
  template<class T>
  std::tuple<bool, int, int> getBcidEnergyRange(unsigned int coolId, boost::any& C) {
    auto settings = boost::any_cast<T*>(C)->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0, 0);
    return std::make_tuple(true, settings->bcidEnergyRangeLow(), settings->bcidEnergyRangeHigh());
  }
} // anonymous namespace

void L1TriggerTowerTool::etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  int energyLow  = 0;
  int energyHigh = 0;
  if (!m_conditionsContainer.empty()) {
    bool available = false;
    if(m_isRun2)
      std::tie(available, energyLow, energyHigh) = getBcidEnergyRange<L1CaloPprConditionsContainerRun2>(channelId.id(), m_conditionsContainer);
    else
      std::tie(available, energyLow, energyHigh) = getBcidEnergyRange<L1CaloPprConditionsContainer>(channelId.id(), m_conditionsContainer);
    
    if(!available) ATH_MSG_WARNING("::etRange: No L1CaloPprConditions found");
  } else ATH_MSG_WARNING("::etRange: No Conditions Container retrieved");

  ATH_MSG_VERBOSE( "::etRange: energyLow: " << energyLow
                          << " energyHigh: "          << energyHigh);

  etRange(et, energyLow, energyHigh, output);
}

/** Identify BCID decision range */

void L1TriggerTowerTool::etRange(const std::vector<int> &et, int energyLow, int energyHigh, std::vector<int> &output)
{
  output.clear();
  output.reserve(et.size()); // avoid frequent reallocations
  for (std::vector<int>::const_iterator it = et.begin(); it != et.end(); ++it) {
    if ((*it) <= energyLow)       output.push_back(0);
    else if ((*it) <= energyHigh) output.push_back(1);
    else                          output.push_back(2);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::etRange: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Truncate FIR results for LUT input */
namespace { // helper function
  template<class T>
  std::tuple<bool, int> getFirStartBit(unsigned int coolId, boost::any& C) {
    auto settings = boost::any_cast<T*>(C)->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0);
    return std::make_tuple(true, settings->firStartBit());
  }
} // anonymous namespace

void L1TriggerTowerTool::dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  unsigned int start = 0;
  if(!m_conditionsContainer.empty()) {
    bool available = false;
    if(m_isRun2)
      std::tie(available, start) = getFirStartBit<L1CaloPprConditionsContainerRun2>(channelId.id(), m_conditionsContainer);
    else
      std::tie(available, start) = getFirStartBit<L1CaloPprConditionsContainer>(channelId.id(), m_conditionsContainer);
    if(!available)ATH_MSG_WARNING( "::dropBits: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::dropBits: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::dropBits: firStartBit: " << start );
  
  dropBits(fir, start, output);
}

/** Truncate FIR results for LUT input */

void L1TriggerTowerTool::dropBits(const std::vector<int> &fir, unsigned int start, std::vector<int> &output)
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations

  /// mask to select appropriate 10 bit range
  unsigned int mask = (0x3ff<<start);

  /// overflow detection
  int max = 1<<(10+start);

  for (std::vector<int>::const_iterator it = fir.begin(); it != fir.end(); ++it) {
     if ((*it)>=max) output.push_back(0x3ff);
     else output.push_back(((*it)&mask)>>start);
  }
  if (m_debug) {
    ATH_MSG_VERBOSE( "::dropBits: output: ");
    printVec(output);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Return FIR filter parameters for a channel */
void L1TriggerTowerTool::firParams(const L1CaloCoolChannelId& channelId, std::vector<int> &firCoeffs)
{   
  /// Get coefficients from COOL DB
  firCoeffs.clear();
  if(!m_conditionsContainer.empty()) {
    const std::vector<short int>* hwCoeffs = nullptr;
    if(m_isRun2)
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), m_conditionsContainer);
    else
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainer>(channelId.id(), m_conditionsContainer);

    if(hwCoeffs) {
      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs->size()); // avoid frequent reallocations
      for (int i = hwCoeffs->size()-1; i >= 0; --i) firCoeffs.push_back((*hwCoeffs)[i]);

    } else ATH_MSG_WARNING( "::firParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::firParams: No Conditions Container retrieved" );

  if (m_debug) {
    ATH_MSG_VERBOSE( "::fir: FIR coefficients: ");
    printVec(firCoeffs);
   ATH_MSG_VERBOSE(" ");
  }
}

/** Return BCID parameters for a channel */
void L1TriggerTowerTool::bcidParams(const L1CaloCoolChannelId& channelId, int &energyLow, int &energyHigh, int &decisionSource, std::vector<unsigned int> &decisionConditions,
                                    unsigned int &peakFinderStrategy, int &satLow, int &satHigh, int &satLevel)
{
  energyLow          = 0;
  energyHigh         = 0;
  decisionSource     = 0;
  decisionConditions.clear();
  peakFinderStrategy = 0;
  satLevel           = 0;
  satLow             = 0;
  satHigh            = 0;
  
  if(!m_conditionsContainer.empty()) {
    using std::get;
    std::tuple<unsigned int, unsigned int, unsigned int> bcidDecision;
    std::tuple<bool, int, int> bcidEnergyRange;
    std::tuple<bool, int, int, int> saturation;
    if(m_isRun2) {
      using Cont = L1CaloPprConditionsContainerRun2;
      bcidDecision = getBcidDecision<Cont>(m_conditionsContainer);
      peakFinderStrategy = getStrategy<Cont>(m_conditionsContainer);
      decisionSource = getDecisionSource<Cont>(m_conditionsContainer);
      bcidEnergyRange = getBcidEnergyRange<Cont>(channelId.id(), m_conditionsContainer);
      saturation = getSaturation<Cont>(channelId.id(), m_conditionsContainer);
    } else {
      using Cont = L1CaloPprConditionsContainer;
      bcidDecision = getBcidDecision<Cont>(m_conditionsContainer);
      peakFinderStrategy = getStrategy<Cont>(m_conditionsContainer);
      decisionSource = getDecisionSource<Cont>(m_conditionsContainer);
      bcidEnergyRange = getBcidEnergyRange<Cont>(channelId.id(), m_conditionsContainer);
      saturation = getSaturation<Cont>(channelId.id(), m_conditionsContainer);
    }

    decisionConditions = { get<2>(bcidDecision), 
                           get<1>(bcidDecision),
                           get<0>(bcidDecision) }; // reverse order
    if(get<0>(bcidEnergyRange)) {
      std::tie(std::ignore, energyLow, energyHigh);
    } else ATH_MSG_WARNING( "::bcidParams: No BcidEnergyRange found" );

    if(get<0>(saturation)) {
      std::tie(std::ignore, satLevel, satLow, satHigh) = saturation;
    } else ATH_MSG_WARNING( "::bcidParams: No Saturation found" );
  } else ATH_MSG_WARNING( "::bcid:Params No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::bcidParams: satLevel: " << satLevel
                   << " satLow: "  << satLow << " satHigh: " << satHigh << endreq
                   << " energyLow: " << energyLow << " energyHigh: " << energyHigh << endreq
                   << " decisionSource: " << decisionSource << " peakFinderStrategy: "
                   << peakFinderStrategy );

}

/** Return LUT parameters for a channel */
void L1TriggerTowerTool::lutParams(const L1CaloCoolChannelId& channelId, int &startBit, int &slope, int &offset, int &cut, int &pedValue, float &pedMean, int &strategy, bool &disabled)
{
  startBit = 0;
  strategy = 0;
  offset   = 0;
  slope    = 0;
  cut      = 0;
  pedValue = 0;
  pedMean  = 0.;
  disabled = true;
  
  if(m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::lutParams: Run-2 data - behaviour undefined!");
  }
 
  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainer*>(m_conditionsContainer);

    const L1CaloPprConditions* settings = conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutStrategy();
      offset   = settings->lutOffset();
      slope    = settings->lutSlope();
      cut      = settings->lutNoiseCut();
      pedValue = settings->pedValue();
      pedMean  = settings->pedMean();
    } else ATH_MSG_WARNING( "::lutParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::lutParams: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::lutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean );
  unsigned int noiseCut = 0;
  disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
}

void L1TriggerTowerTool::cpLutParams(const L1CaloCoolChannelId& channelId, int& startBit, int& slope, int& offset, int& cut, int& pedValue, float& pedMean, int& strategy, bool& disabled)
{
  startBit = 0;
  strategy = 0;
  offset   = 0;
  slope    = 0;
  cut      = 0;
  pedValue = 0;
  pedMean  = 0.;
  disabled = true;
  
  if(!m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::cpLutParams: Run-1 data - behaviour undefined!");
  }

  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainerRun2*>(m_conditionsContainer);

    const L1CaloPprConditionsRun2* settings = conditionsContainer->pprConditions(channelId.id());
    if(settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutCpStrategy();
      offset   = settings->lutCpOffset();
      slope    = settings->lutCpSlope();
      cut      = settings->lutCpNoiseCut();
      pedValue = settings->pedValue();
      pedMean  = settings->pedMean();
    } else ATH_MSG_WARNING( "::cpLutParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::cpLutParams: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::cpLutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean );
  unsigned int noiseCut = 0;
  disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
}

void L1TriggerTowerTool::jepLutParams(const L1CaloCoolChannelId& channelId, int& startBit, int& slope, int& offset, int& cut, int& pedValue, float& pedMean, int& strategy, bool& disabled)
{
  startBit = 0;
  strategy = 0;
  offset   = 0;
  slope    = 0;
  cut      = 0;
  pedValue = 0;
  pedMean  = 0.;
  disabled = true;
  
  if(!m_isRun2) {
    // assert instead ?!
    ATH_MSG_WARNING("::jepLutParams: Run-1 data - behaviour undefined!");
  }

  if(!m_conditionsContainer.empty()) {
    auto conditionsContainer = boost::any_cast<L1CaloPprConditionsContainerRun2*>(m_conditionsContainer);

    const L1CaloPprConditionsRun2* settings = conditionsContainer->pprConditions(channelId.id());
    if(settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutJepStrategy();
      offset   = settings->lutJepOffset();
      slope    = settings->lutJepSlope();
      cut      = settings->lutJepNoiseCut();
      pedValue = settings->pedValue();
      pedMean  = settings->pedMean();
    } else ATH_MSG_WARNING( "::jepLutParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::jepLutParams: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::jepLutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean );
  unsigned int noiseCut = 0;
  disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
}

/** Return offline identifier for given tower coordinates */

Identifier L1TriggerTowerTool::identifier(double eta, double phi, int layer)
{
   Identifier id(0);
   if (m_l1CaloTTIdTools && m_lvl1Helper) {
     int pos_neg_z = m_l1CaloTTIdTools->pos_neg_z(eta);
     int region    = m_l1CaloTTIdTools->regionIndex(eta);
     int ieta      = m_l1CaloTTIdTools->etaIndex(eta);
     int iphi      = m_l1CaloTTIdTools->phiIndex(eta, phi);
   
     id = m_lvl1Helper->tower_id(pos_neg_z, layer, region, ieta, iphi);
   }
   return id;
} 

/** Return online identifier for given offline identifier */

HWIdentifier L1TriggerTowerTool::hwIdentifier(const Identifier& id)
{
   HWIdentifier hwId(0);
   if (m_ttSvc) {
     try   { hwId = m_ttSvc->createTTChannelID(id, false); }
     catch (CaloID_Exception) { hwId = HWIdentifier(0); }
   }
   return hwId;
}

/** Return online identifier for given tower coordinates */

HWIdentifier L1TriggerTowerTool::hwIdentifier(double eta, double phi, int layer)
{
   Identifier id = identifier(eta, phi, layer);
   return hwIdentifier(id);
}

/** Return Cool channel identifier for given tower coordinates */

L1CaloCoolChannelId L1TriggerTowerTool::channelID(double eta, double phi, int layer)
{
  // Use direct lookup table if possible
  const double absEta = fabs(eta);
  int index = 0;
  if (absEta < 2.5) {
    const int etaBin = 10.*absEta;
    const int phiBin = phi/(M_PI/32.);
    index = (etaBin<<6) + phiBin;
  } else if (absEta < 3.2) {
    const int etaBin = 5.*(absEta - 2.5);
    const int phiBin = phi/(M_PI/16.);
    index = 1600 + (etaBin<<5) + phiBin;
  } else {
    const int etaBin = (absEta - 3.2)/0.425;
    const int phiBin = phi/(M_PI/8.);
    index = 1728 + (etaBin<<4) + phiBin;
  }
  if (eta < 0.)  index += 1792;
  if (layer > 0) index += 3584;
  if (index >= s_maxTowers) return L1CaloCoolChannelId(0);
  if (m_idTable.empty()) {
    m_idTable.reserve(s_maxTowers);
    m_idTable.assign(s_maxTowers, 0);
  }
  if (m_idTable[index] == 0) {
    Identifier id = identifier(eta, phi, layer);
    L1CaloCoolChannelId coolID = channelID(id);
    m_idTable[index] = coolID.id();
  }
  return L1CaloCoolChannelId(m_idTable[index]);
}

/** Return Cool channel identifier for given offline identifier */

L1CaloCoolChannelId L1TriggerTowerTool::channelID(const Identifier& id)
{
  L1CaloCoolChannelId coolId(0);
  if (m_ttSvc) {
    try {
      HWIdentifier hwId = hwIdentifier(id);
      coolId = m_ttSvc->createL1CoolChannelId(hwId);
    }
    catch (CaloID_Exception) { coolId = L1CaloCoolChannelId(0); }
  }
  return coolId;
}

/** Return saturation override flag for given channel & et range */
namespace { // helper function
  template<class T>
  std::tuple<bool, bool, bool> getSatOverride(boost::any& C) {
    auto CC = boost::any_cast<T*>(C);
    return std::make_tuple(CC->satOverride1(), CC->satOverride2(), CC->satOverride3());
  }
} // anonymous namespace

bool L1TriggerTowerTool::satOverride(int range, const L1CaloCoolChannelId& /*channelId*/)
{
  bool override = false;
  if(!m_conditionsContainer.empty()) {
    std::tuple<bool, bool, bool> satOverride;
    if(m_isRun2)
      satOverride = getSatOverride<L1CaloPprConditionsContainerRun2>(m_conditionsContainer);
    else
      satOverride = getSatOverride<L1CaloPprConditionsContainer>(m_conditionsContainer);
    // NB Reverse order as for bcidDecision1/2/3
    if (range == 0) override = std::get<2>(satOverride);
    if (range == 1) override = std::get<1>(satOverride);
    if (range == 2) override = std::get<0>(satOverride);
  } else ATH_MSG_WARNING( "::satOverride: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::satOverride: range " << range
          << " has saturation override flag " << override );
  
  return override;
}

/** Check for disabled channel */

bool L1TriggerTowerTool::disabledChannel(const L1CaloCoolChannelId& channelId)
{
  unsigned int noiseCut = 0;
  return disabledChannel(channelId, noiseCut);
}

/** Check for disabled channel with noise cut */
bool L1TriggerTowerTool::disabledChannel(const L1CaloCoolChannelId& channelId, unsigned int& noiseCut)
{
  bool isDisabled = false;
  noiseCut = 0;
  if(!m_disabledChannelContainer.empty()) {
    const L1CaloPprDisabledChannel* disabledChan = nullptr;
    if(m_isRun2) disabledChan = boost::any_cast<L1CaloPprDisabledChannelContainerRun2*>(m_disabledChannelContainer)->pprDisabledChannel(channelId.id());
    else disabledChan = boost::any_cast<L1CaloPprDisabledChannelContainer*>(m_disabledChannelContainer)->pprDisabledChannel(channelId.id());

    if (disabledChan) {
      if (!disabledChan->disabledBits()) {
        ChanCalibErrorCode calibError(disabledChan->calibErrorCode());
        if (calibError.chanValid()) {
          ChanDeadErrorCode deadError(disabledChan->deadErrorCode());
          if (deadError.chanValid()) noiseCut = disabledChan->noiseCut();
      //else isDisabled = true;
        } //else isDisabled = true;
      } else isDisabled = true;
      
      ATH_MSG_VERBOSE( MSG::hex
              << "::disabledChannel: calibErrorCode: " << (disabledChan->calibErrorCode()).errorCode()
              << "  deadErrorCode: " << (disabledChan->deadErrorCode()).errorCode()
              << "  noiseCut: "      << disabledChan->noiseCut()
              << "  disabledBits: "  << disabledChan->disabledBits()
              << MSG::dec );
      
    } else {
      ATH_MSG_VERBOSE( "::disabledChannel: No L1CaloPprDisabledChannel found" );
    }
  } else {
    ATH_MSG_WARNING( "::disabledChannel: No DisabledChannel Container retrieved" );
  }
  if (isDisabled && m_debug) ATH_MSG_VERBOSE( "::disabledChannel: Channel is disabled" );

  return isDisabled;
}

/** Finer control of debug printing */

void L1TriggerTowerTool::setDebug(bool debug)
{
  m_debug = (debug && msgLvl(MSG::VERBOSE));
}

/** Calculate median eta of FCAL trigger tower from nominal eta and layer.
    Hopefully this will be a short-term work-around */

double L1TriggerTowerTool::FCalTTeta(double nominalEta, double /*phi*/, int layer)
{
  double eta = nominalEta;
  float abseta = fabs(eta);
  if (abseta<3.2) return eta; // If called for non-FCAL TT return input value
  if (layer == 0) {
    int sign = ((eta > 0) ? 1 : -1);
    if (abseta < 3.6)      eta = 3.15 * sign;
    else if (abseta < 4.0) eta = 3.33 * sign;
    else if (abseta < 4.5) eta = 3.72 * sign;
    else                   eta = 4.41 * sign;
  }
  else if (eta > 0) {
    if (abseta < 3.6)      eta = 3.36;
    else if (abseta < 4.0) eta = 3.45;
    else if (abseta < 4.5) eta = 4.17;
    else                   eta = 4.19;    
  }
  else {
    if (abseta < 3.6)      eta = -3.45;
    else if (abseta < 4.0) eta = -3.36;
    else if (abseta < 4.5) eta = -4.19;
    else                   eta = -4.17;    
  }
  return eta;
} 

/** Return median eta of trigger tower from L1CaloCoolChannelId. For FCAL towers
    this is the physical centre of the eta range - for other towers the normal
    tower eta will be returned */

double L1TriggerTowerTool::FCalTTeta(const L1CaloCoolChannelId& channelId)
{
  /// Get crate/module/submodule/channel from the CoolId
  unsigned int crate   = channelId.crate();
  unsigned int module  = channelId.module();
  unsigned int mcm     = channelId.subModule();
  unsigned int pin     = channelId.channel();
  int channel = pin*16 + mcm;

  /// use these with mapping tool to get eta
  double eta;
  double phi;
  int layer;
  if(!m_mappingTool->mapping(crate, module, channel, eta, phi, layer)) {
    ATH_MSG_WARNING("::FCalTTeta: could not map 0x" << std::hex << channelId.id() << std::dec);
  }

  /// for FCAL, convert nominal eta to physical
  return FCalTTeta(eta, phi, layer);
}

/** Print a vector to debug */
template <typename T>
void L1TriggerTowerTool::printVec(const std::vector<T>& vec)
{
  if (m_debug) {
    if (vec.empty()) ATH_MSG_VERBOSE( " empty ");
    else {
      for(auto v : vec) {
        ATH_MSG_VERBOSE( v << " ");
      }
    }
  }
}

StatusCode L1TriggerTowerTool::loadFTRefs()
{
  //method to load the FineTimeReferences Folder from COOL or an sqlite file (needs to be included in the job options)
  //implementation very similar to L1TriggerTowerTool::retrieveConditions
  //calling this method outside of fillHistograms() can result in Errors
  StatusCode sc;
  m_dbFineTimeRefsTowers = 0;
  
  if (m_l1CondSvc) {
    ATH_MSG_VERBOSE( "Retrieving FineTimeReferences Containers" );
    bool verbose = msgLevel() <= MSG::VERBOSE;


    sc = m_l1CondSvc->retrieve(m_dbFineTimeRefsTowers);
    if (sc.isFailure()) {
     ATH_MSG_WARNING( "No FineTimeReferences Folder found" );
     return sc;
    }
    ATH_MSG_VERBOSE( "Retrieved FineTimeReferences Container" );
    if (verbose) m_dbFineTimeRefsTowers->dump();

  } else {
    ATH_MSG_WARNING( "Could not retrieve FineTimeReferences, as Conditon Service not present" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

std::pair<double, double> L1TriggerTowerTool::refValues(const L1CaloCoolChannelId& channelId)
{
  //method returning the fine time reference and calibration value
  //the fineTimeReference folder has to be loaded first using the method L1TriggerTowerTool::loadFTRefs
  double reference = 0;
  double calib = 0;
  
  if (m_dbFineTimeRefsTowers) {
    const L1CaloPpmFineTimeRefs* ftref = m_dbFineTimeRefsTowers->ppmFineTimeRefs(channelId.id());
    if (ftref) {
      FineTimeErrorCode errorCode(ftref->errorCode());
//        if (errorCode.chanValid()) { //this should be changed at some point, at the moment the error code is ignored
      reference = ftref->refValue();
      calib = ftref->calibValue();
//        }
      
      ATH_MSG_VERBOSE( MSG::hex
              << "::refValues: errorCode: " << (ftref->errorCode()).errorCode()
             << MSG::dec << "  reference: " << ftref->refValue() << "  calib: "  << ftref->calibValue() );
  
    } else {
      ATH_MSG_VERBOSE( "::refValue: No FineTimeRefsTowers found" );
    }
  } else {
    ATH_MSG_VERBOSE( "::refValue: No FineTimeRefs Container retrieved" );
  }
  
  return std::make_pair(reference, calib);
}

void L1TriggerTowerTool::pedestalCorrection(std::vector<int>& firInOut, int firPed, int iElement, int layer, int bcid, float mu, std::vector<int_least16_t>& correctionOut) {
  unsigned nFIR = firInOut.size();
  correctionOut.assign(nFIR, 0u);

  if(!m_correctFir) return;

  // apply the pedestal correction
  for(unsigned i = 0; i != nFIR; ++i) {
    correctionOut[i] = (m_dynamicPedestalProvider->dynamicPedestal(iElement, layer, firPed, bcid + i - nFIR/2, mu) - firPed);
    firInOut[i] -= correctionOut[i];

    if(firInOut[i] < 0) firInOut[i] = 0;
  }

  if(m_debug) {
    ATH_MSG_VERBOSE( "::pedestalCorrection(BCID=" << bcid << ", mu = " << mu << "): ");
    printVec(correctionOut);
    ATH_MSG_VERBOSE(" ");
  }
}

} // end of namespace
