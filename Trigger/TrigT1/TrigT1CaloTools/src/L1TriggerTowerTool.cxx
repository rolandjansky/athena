//////////////////////////////////////////////////////////////////////
//  L1TriggerTowerTool.cxx (c) Alan Watson
///////////////////////////////////////////////////////////////////////

#include "GaudiKernel/IIncidentSvc.h"

#include "TrigT1CaloTools/L1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibConditions/FineTimeErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannel.h"
#include "TrigT1CaloCalibConditions/L1CaloPpmFineTimeRefs.h"
#include "TrigT1CaloToolInterfaces/IL1DynamicPedestalProvider.h"

#include <cstdint>

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
  AlgTool(t,n,p),
  m_log(msgSvc(),n),
  m_caloMgr(0),
  m_lvl1Helper(0),
//   m_l1CaloTTIdTools(0),
  m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
  m_ttSvc(0),
  m_mappingTool("LVL1::PpmCoolOrBuiltinMappingTool/PpmCoolOrBuiltinMappingTool"),
  m_l1CondSvc(0),
  m_conditionsContainer(0),
  m_DisabledChannelContainer(0),
  m_dbFineTimeRefsTowers(0),
  m_correctFir(false)
{
  declareInterface<IL1TriggerTowerTool>(this);

  declareProperty( "BaselineCorrection", m_correctFir );
  declareProperty( "L1DynamicPedestalProvider", m_dynamicPedestalProvider );
}

//================ Destructor =================================================

L1TriggerTowerTool::~L1TriggerTowerTool()
{
}

//================ Initialisation =================================================

StatusCode L1TriggerTowerTool::initialize()
{
  m_log.setLevel(outputLevel());
  m_debug = outputLevel() <= MSG::VERBOSE; // May want to make this VERBOSE!
  
  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) {
    m_log << MSG::ERROR << "Problem initializing AlgTool " <<  endreq;
    return sc;
  }
    
  ///get a pointer to DetectorStore services
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
      m_log << MSG::ERROR << "Cannot access DetectorStore" << endreq;
      return sc;
  }

  /// and the L1CaloCondSvc
  ///  - note: may use tools without DB, so don't abort if fail here
  sc = service("L1CaloCondSvc", m_l1CondSvc);
  if (sc.isFailure()) {
    m_log << MSG::WARNING << "Could not retrieve L1CaloCondSvc" << endreq;
  }

  /// Retrieve tools for computing identifiers
  sc = m_l1CaloTTIdTools.retrieve();
  if (sc.isFailure()) {
    m_log << MSG::WARNING << "Cannot get L1CaloTTIdTools !"<< endreq;
  }
  
  IToolSvc* toolSvc;
  StatusCode scTools = service( "ToolSvc",toolSvc );
//   IAlgTool *algtool;

  if (scTools.isSuccess()) {
//      sc = toolSvc->retrieveTool("L1CaloTTIdTools", algtool);
//      if (sc!=StatusCode::SUCCESS) {
//        m_log << MSG::WARNING << " Cannot get L1CaloTTIdTools !" << endreq;
//      }
//      m_l1CaloTTIdTools = dynamic_cast<L1CaloTTIdTools*> (algtool);

    sc = toolSvc->retrieveTool("CaloTriggerTowerService", m_ttSvc);
    if (sc.isFailure()) {
      m_log << MSG::WARNING << "Could not retrieve CaloTriggerTowerService Tool" << endreq;
      //return StatusCode::FAILURE;
    }
  } else {
    m_log << MSG::WARNING << "Unable to retrieve ToolSvc" << endreq;
  }
  
  StatusCode scID = m_detStore->retrieve(m_caloMgr);
  if (scID.isFailure()) {
    m_log << MSG::WARNING << "Cannot retrieve m_caloMgr" << endreq;
  } else {
    m_lvl1Helper = m_caloMgr->getLVL1_ID();
  }
 
  sc = m_mappingTool.retrieve(); 
  if (sc.isFailure()) { 
    m_log << MSG::WARNING << "Failed to retrieve tool " << m_mappingTool << endreq; 
  } else {
    m_log << MSG::INFO << "Retrieved tool " << m_mappingTool << endreq; 
  }

  // Incident Service:
  IIncidentSvc* incSvc = 0;
  sc = service("IncidentSvc", incSvc);
  if (sc.isFailure()) {
     m_log << MSG::WARNING << "Unable to retrieve pointer to IncidentSvc " << endreq;
     //return StatusCode::FAILURE;
  }

  // Pedestal Correction
  if (m_correctFir) {
      sc = m_dynamicPedestalProvider.retrieve();
      if (sc.isFailure()) { 
          m_log << MSG::WARNING << "Failed to retrieve L1DynamicPedestalProvider: " << m_dynamicPedestalProvider << endreq; 
          return StatusCode::FAILURE;
      } else {
          m_log << MSG::INFO << "Retrieved L1DynamicPedestalProvider: " << m_dynamicPedestalProvider << endreq; 
      }
  }

  //start listening to "BeginRun"
  if (incSvc) incSvc->addListener(this, "BeginRun");
  
  m_log << MSG::INFO << "Initialization completed" << endreq;
  
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1TriggerTowerTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//================ Reset mapping table at start of run ============================

void L1TriggerTowerTool::handle(const Incident& inc)
{
  if (inc.type()=="BeginRun") {
    if (m_debug) m_log << MSG::DEBUG << "Resetting mapping table at start of run" << endreq;
   
    m_idTable.clear();
  }
  return;
}

//================= Now the actual user calls ===================================

/** Retrieve pointers to the L1Calo conditions containers */

StatusCode L1TriggerTowerTool::retrieveConditions()
{
  StatusCode sc;

  m_conditionsContainer = 0;
  m_DisabledChannelContainer = 0;

  if (m_l1CondSvc) {
    if (m_debug) m_log << MSG::VERBOSE << "Retrieving Conditions Containers" << endreq;
    bool verbose = outputLevel() <= MSG::VERBOSE;

    sc = m_l1CondSvc->retrieve(m_conditionsContainer);
    if (sc.isFailure()) {
      m_log << MSG::WARNING << "Could not retrieve ConditionsContainer" << endreq;
      return sc;
    }
    if (m_debug) m_log << MSG::VERBOSE << "Retrieved ConditionsContainer" << endreq;
    if (verbose) m_conditionsContainer->dump();

    sc = m_l1CondSvc->retrieve(m_DisabledChannelContainer);
    if (sc.isFailure()) {
      m_log << MSG::WARNING << "Could not retrieve DisabledChannelContainer" << endreq;
      return sc;
    }
    if (m_debug) m_log << MSG::VERBOSE << "Retrieved DisabledChannelContainer" << endreq;
    if (verbose) m_DisabledChannelContainer->dump();

  } else {
    m_log << MSG::WARNING << "Could not retrieve Conditions Containers" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** All-in-one routine - give it the ADC counts, coordinate and layer, and
    it returns the results */
    
void L1TriggerTowerTool::process(const std::vector<int> &digits, double eta, double phi, int layer,
             std::vector<int> &et, std::vector<int> &bcidResults,
             std::vector<int> &bcidDecisions)
{
  /// Find channelID for this tower
  L1CaloCoolChannelId id = channelID(eta, phi, layer);

  /// then process the tower
  process(digits, id, et, bcidResults, bcidDecisions);
}

/** All-in-one routine - give it the ADC counts and TT identifier, and
    it returns the results */
    
void L1TriggerTowerTool::process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
             std::vector<int> &et, std::vector<int> &bcidResults,
             std::vector<int> &bcidDecisions)
{
  if (m_debug) {
    m_log << MSG::VERBOSE << "::process: ==== Entered Process ====" << endreq;
    m_log << MSG::VERBOSE << "::process: digits: ";
    printVec(digits);
    m_log << MSG::VERBOSE << " channelID: " << MSG::hex << channelId.id() << MSG::dec << endreq;
  }
  
  /// Initialise
  et.clear();
  bcidResults.clear();
  bcidDecisions.clear();

  if (m_debug) m_log << MSG::VERBOSE << "::process: ---- FIR filter ----" << endreq;
      
  /// emulate FIR filter
  std::vector<int> filter;
  fir(digits, channelId, filter);
  std::vector<int> lutInput;
  dropBits(filter, channelId, lutInput);

  if (m_debug) m_log << MSG::VERBOSE << "::process: ---- BCID algorithms ----" << endreq;

  /// emulate the two BCID algorithms
  bcid(filter, digits, channelId, bcidResults);

  if (m_debug) m_log << MSG::VERBOSE << "::process: ---- BCID decisions ----" << endreq;

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  bcidDecisionRange(lutInput, digits, channelId, decisionRange);
  bcidDecision(bcidResults, decisionRange, channelId, bcidDecisions);

  if (m_debug) m_log << MSG::VERBOSE << "::process: ---- LUT ET calculation ----" << endreq;

  /// LUT ET calculation
  std::vector<int> lutOutput;
  lut(lutInput, channelId, lutOutput);

  if (m_debug) m_log << MSG::VERBOSE << "::process: ---- use ET range ----" << endreq;

  /// Use ET range to return appropriate ET value
  /// do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably
  applyEtRange(lutOutput, decisionRange, channelId, et);

  if (m_debug) m_log << MSG::VERBOSE << "::process: ==== Leaving Process ====" << endreq;
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
   m_log << MSG::VERBOSE << "::bcid: bcidResults: ";
   printVec(output);
   m_log << MSG::VERBOSE << endreq;
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
   m_log << MSG::VERBOSE << "::bcid: bcidResults: ";
   printVec(output);
   m_log << MSG::VERBOSE << endreq;
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
   m_log << MSG::VERBOSE << "::bcid: bcidResults: ";
   printVec(result);
   m_log << MSG::VERBOSE << endreq;
  }

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  if (!decisionSource&0x1) etRange(digits, energyLow, energyHigh, decisionRange);
  else                     etRange(lutInput, energyLow, energyHigh, decisionRange);
  bcidDecision(result, decisionRange, decisionConditions, decision);
  if (m_debug) {
   m_log << MSG::VERBOSE << "::bcid: bcidDecisions: ";
   printVec(decision);
   m_log << MSG::VERBOSE << endreq;
  }
}

/** This FIR simulation produces a vector of same length as digit vector,
    with peak positions corresponding. However, first 2 and last 2 FIR
    sums will be incomplete, and so are zeroed here */

void L1TriggerTowerTool::fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{   
  /// Get coefficients from COOL DB
  std::vector<int> firCoeffs;
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      const std::vector<short int>& hwCoeffs(settings->firCoefficients());

      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs.size()); // avoid frequent reallocations
      for (int i = hwCoeffs.size()-1; i >= 0; --i) firCoeffs.push_back(hwCoeffs[i]);

    } else if (m_debug) m_log << MSG::VERBOSE << "::fir: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::fir: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::fir: FIR coefficients: ";
    printVec(firCoeffs);
    m_log << MSG::VERBOSE << endreq;
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
    m_log << MSG::VERBOSE << "::fir: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}
/** Peak finder BCID */
void L1TriggerTowerTool::peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& /*channelId*/, std::vector<int> &output)
{
  unsigned int strategy = 0;
  if (m_conditionsContainer) {
    strategy = m_conditionsContainer->peakFinderCond();
  } else if (m_debug) m_log << MSG::VERBOSE << "::peakBcid: No Conditions Container retrieved" << endreq;

  if (m_debug) m_log << MSG::VERBOSE << "::peakBcid: peak-finder strategy: " << strategy << endreq;
  
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
    m_log << MSG::VERBOSE << "::peakBcid: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Saturated pulse BCID */

void L1TriggerTowerTool::satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{  
  int satLevel = 0;
  int satLow   = 0;
  int satHigh  = 0;
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      satLevel = settings->satBcidLevel();
      satLow   = settings->satBcidThreshLow();
      satHigh  = settings->satBcidThreshHigh();
    } else if (m_debug) m_log << MSG::VERBOSE << "::satBcid: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::satBcid: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::satBcid: satLevel: " << satLevel
      << " satLow: "  << satLow
      << " satHigh: " << satHigh << endreq;
  }

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
    m_log << MSG::VERBOSE << "::satBcid: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Evaluate BCID decision range */

void L1TriggerTowerTool::bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  int decisionSource = 0;
  if (m_conditionsContainer) {
    decisionSource = m_conditionsContainer->decisionSource();
  } else if (m_debug) m_log << MSG::VERBOSE << "::bcidDecisionRange: No Conditions Container retrieved" << endreq;

  if (!decisionSource&0x1) etRange(digits, channelId, output);
  else                     etRange(lutInput, channelId, output);
  if (m_debug) {
    m_log << MSG::VERBOSE << "::bcidDecisionRange: decisionSource: " << decisionSource;
    m_log << MSG::VERBOSE << " output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Evaluate BCID decision based on BCID word, ET range and channel ID */

void L1TriggerTowerTool::bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const L1CaloCoolChannelId& /*channelId*/, std::vector<int> &output)
{
  unsigned int decision1 = 0;
  unsigned int decision2 = 0;
  unsigned int decision3 = 0;
  if (m_conditionsContainer) {
    decision1 = m_conditionsContainer->bcidDecision1();
    decision2 = m_conditionsContainer->bcidDecision2();
    decision3 = m_conditionsContainer->bcidDecision3();
  } else if (m_debug) m_log << MSG::VERBOSE << "::bcidDecision: No Conditions Container retrieved" << endreq;

  // Reverse the order! (see elog 97082 9/06/10)
  std::vector<unsigned int> mask = { decision3, decision2, decision1 };
  if (m_debug) {
   m_log << MSG::VERBOSE << "::bcidDecision: masks: " << MSG::hex
         << decision3 << " " << decision2 << " " << decision1 << MSG::dec << endreq;
  }

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
   m_log << MSG::VERBOSE << "::bcidDecision: output: ";
   printVec(output);
   m_log << MSG::VERBOSE << endreq;
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
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      strategy = settings->lutStrategy();
      offset   = settings->lutOffset();
      slope    = settings->lutSlope();
      cut      = settings->lutNoiseCut();
      ped      = settings->pedValue();
    } else if (m_debug) m_log << MSG::VERBOSE << "::lut: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::lut: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::lut: LUT strategy/offset/slope/cut/ped: "
          << strategy << " " << offset << " " << slope << " " << cut << " " << ped << " " << endreq;
  }
  unsigned int noiseCut = 0;
  bool disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;

  lut(fir, slope, offset, cut, ped, strategy, disabled, output);
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
    m_log << MSG::VERBOSE << "::lut: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
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
    m_log << MSG::VERBOSE << "::applyEtRange: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Identify BCID decision range */

void L1TriggerTowerTool::etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  int energyLow  = 0;
  int energyHigh = 0;
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      energyLow  = settings->bcidEnergyRangeLow();
      energyHigh = settings->bcidEnergyRangeHigh();
    } else if (m_debug) m_log << MSG::VERBOSE << "::etRange: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::etRange: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::etRange: energyLow: " << energyLow
                          << " energyHigh: "          << energyHigh
                          << endreq;
  }

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
    m_log << MSG::VERBOSE << "::etRange: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Truncate FIR results for LUT input */

void L1TriggerTowerTool::dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output)
{
  unsigned int start = 0;
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) start = settings->firStartBit();
    else if (m_debug) m_log << MSG::VERBOSE << "::dropBits: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::dropBits: No Conditions Container retrieved" << endreq;

  if (m_debug) m_log << MSG::VERBOSE << "::dropBits: firStartBit: " << start << endreq;
  
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
    m_log << MSG::VERBOSE << "::dropBits: output: ";
    printVec(output);
    m_log << MSG::VERBOSE << endreq;
  }
}

/** Return FIR filter parameters for a channel */

void L1TriggerTowerTool::firParams(const L1CaloCoolChannelId& channelId, std::vector<int> &firCoeffs)
{   
  /// Get coefficients from COOL DB
  firCoeffs.clear();
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      const std::vector<short int>& hwCoeffs(settings->firCoefficients());

      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs.size()); // avoid frequent reallocations
      for (int i = hwCoeffs.size()-1; i >= 0; --i) firCoeffs.push_back(hwCoeffs[i]);

    } else if (m_debug) m_log << MSG::VERBOSE << "::firParams: No L1CaloPprConditions found" << endreq;
  } else if (m_debug) m_log << MSG::VERBOSE << "::firParams: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::fir: FIR coefficients: ";
    printVec(firCoeffs);
    m_log << MSG::VERBOSE << endreq;
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
  
  if (m_conditionsContainer) {
    unsigned int decision1 = m_conditionsContainer->bcidDecision1();
    unsigned int decision2 = m_conditionsContainer->bcidDecision2();
    unsigned int decision3 = m_conditionsContainer->bcidDecision3();
    decisionConditions = { decision3, decision2, decision1 }; // reverse order
    peakFinderStrategy = m_conditionsContainer->peakFinderCond();
    decisionSource = m_conditionsContainer->decisionSource();
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      energyLow  = settings->bcidEnergyRangeLow();
      energyHigh = settings->bcidEnergyRangeHigh();
      satLevel = settings->satBcidLevel();
      satLow   = settings->satBcidThreshLow();
      satHigh  = settings->satBcidThreshHigh();

    } else m_log << MSG::WARNING << "::bcidParams: No L1CaloPprConditions found" << endreq;
  } else m_log << MSG::WARNING << "::bcid:Params No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::bcidParams: satLevel: " << satLevel
                        << " satLow: "  << satLow << " satHigh: " << satHigh << endreq
                        << " energyLow: " << energyLow << " energyHigh: " << energyHigh << endreq
                        << " decisionSource: " << decisionSource << " peakFinderStrategy: "
                        << peakFinderStrategy << endreq;
  }
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
  
  if (m_conditionsContainer) {
    const L1CaloPprConditions* settings = m_conditionsContainer->pprConditions(channelId.id());
    if (settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutStrategy();
      offset   = settings->lutOffset();
      slope    = settings->lutSlope();
      cut      = settings->lutNoiseCut();
      pedValue = settings->pedValue();
      pedMean  = settings->pedMean();
    } else m_log << MSG::WARNING << "::lutParams: No L1CaloPprConditions found" << endreq;
  } else m_log << MSG::WARNING << "::lutParams: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::lutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean << endreq;
  }
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
     try   { hwId = m_ttSvc->createTTChannelID(id); }
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

bool L1TriggerTowerTool::satOverride(int range, const L1CaloCoolChannelId& /*channelId*/)
{
  bool override = false;
  if (m_conditionsContainer) {
    // NB Reverse order as for bcidDecision1/2/3
    if (range == 0) override = m_conditionsContainer->satOverride3();
    if (range == 1) override = m_conditionsContainer->satOverride2();
    if (range == 2) override = m_conditionsContainer->satOverride1();
  } else if (m_debug) m_log << MSG::VERBOSE << "::satOverride: No Conditions Container retrieved" << endreq;

  if (m_debug) {
    m_log << MSG::VERBOSE << "::satOverride: range " << range
          << " has saturation override flag " << override << endreq;
  }
  
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
  if (m_DisabledChannelContainer) {
    const L1CaloPprDisabledChannel* disabledChan = m_DisabledChannelContainer->pprDisabledChannel(channelId.id());
    if (disabledChan) {
      if (!disabledChan->disabledBits()) {
        ChanCalibErrorCode calibError(disabledChan->calibErrorCode());
        if (calibError.chanValid()) {
          ChanDeadErrorCode deadError(disabledChan->deadErrorCode());
          if (deadError.chanValid()) noiseCut = disabledChan->noiseCut();
      //else isDisabled = true;
        } //else isDisabled = true;
      } else isDisabled = true;
      if (m_debug) {
        m_log << MSG::VERBOSE << MSG::hex
              << "::disabledChannel: calibErrorCode: " << (disabledChan->calibErrorCode()).errorCode()
              << "  deadErrorCode: " << (disabledChan->deadErrorCode()).errorCode()
              << "  noiseCut: "      << disabledChan->noiseCut()
              << "  disabledBits: "  << disabledChan->disabledBits()
              << MSG::dec << endreq;
      }
    } else {
      if (m_debug) m_log << MSG::VERBOSE << "::disabledChannel: No L1CaloPprDisabledChannel found" << endreq;
    }
  } else {
    if (m_debug) m_log << MSG::VERBOSE << "::disabledChannel: No DisabledChannel Container retrieved" << endreq;
  }
  if (isDisabled && m_debug) m_log << MSG::VERBOSE << "::disabledChannel: Channel is disabled" << endreq;

  return isDisabled;
}

/** Finer control of debug printing */

void L1TriggerTowerTool::setDebug(bool debug)
{
  m_debug = (debug && outputLevel() <= MSG::VERBOSE);
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
  m_mappingTool->mapping(crate, module, channel, eta, phi, layer);

  /// for FCAL, convert nominal eta to physical
  return FCalTTeta(eta, phi, layer);
}

/** Print a vector to debug */
template <typename T>
void L1TriggerTowerTool::printVec(const std::vector<T>& vec)
{
  if (m_debug) {
    if (vec.empty()) m_log << MSG::VERBOSE << " empty ";
    else {
      for(auto v : vec) {
        m_log << MSG::VERBOSE << v << " ";
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
    if (m_debug) m_log << MSG::VERBOSE << "Retrieving FineTimeReferences Containers" << endreq;
    bool verbose = outputLevel() <= MSG::VERBOSE;


    sc = m_l1CondSvc->retrieve(m_dbFineTimeRefsTowers);
    if (sc.isFailure()) {
     m_log << MSG::WARNING << "No FineTimeReferences Folder found" << endreq;
     return sc;
    }
    if (m_debug) m_log << MSG::VERBOSE << "Retrieved FineTimeReferences Container" << endreq;
    if (verbose) m_dbFineTimeRefsTowers->dump();

  } else {
    m_log << MSG::WARNING << "Could not retrieve FineTimeReferences, as Conditon Service not present" << endreq;
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
      if (m_debug) {
        m_log << MSG::VERBOSE << MSG::hex
              << "::refValues: errorCode: " << (ftref->errorCode()).errorCode()
        << MSG::dec
        << "  reference: " << ftref->refValue()
        << "  calib: "  << ftref->calibValue()
        << endreq;
      }  
    } else {
      if (m_debug) m_log << MSG::VERBOSE << "::refValue: No FineTimeRefsTowers found" << endreq;
    }
  } else {
    if (m_debug) m_log << MSG::VERBOSE << "::refValue: No FineTimeRefs Container retrieved" << endreq;
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
    m_log << MSG::VERBOSE << "::pedestalCorrection(BCID=" << bcid << ", mu = " << mu << "): ";
    printVec(correctionOut);
    m_log << MSG::VERBOSE << endreq;
  }
}

} // end of namespace
