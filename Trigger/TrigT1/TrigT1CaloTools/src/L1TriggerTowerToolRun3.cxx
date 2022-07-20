/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//////////////////////////////////////////////////////////////////////
//  L1TriggerTowerToolRun3.cxx 
///////////////////////////////////////////////////////////////////////
#include "L1TriggerTowerToolRun3.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloToolInterfaces/IL1DynamicPedestalProvider.h"

#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <cstdint>
#include <tuple>
#include <boost/range/adaptor/reversed.hpp>
using namespace boost::adaptors;

namespace LVL1 
{

//================ Static Constants ============================================

const int L1TriggerTowerToolRun3::s_saturationValue;
const int L1TriggerTowerToolRun3::s_maxTowers;

//================ Constructor =================================================

L1TriggerTowerToolRun3::L1TriggerTowerToolRun3(const std::string& t,
                          const std::string& n,
                          const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_caloMgr(0),
  m_lvl1Helper(0),
  m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools", this),
  m_ttSvc("CaloTriggerTowerService/CaloTriggerTowerService", this),
  m_mappingTool("", this),
  m_correctFir(false),
  m_dynamicPedestalProvider("", this)
{
  declareInterface<IL1TriggerTowerToolRun3>(this);

  declareProperty( "BaselineCorrection", m_correctFir );
  declareProperty( "L1DynamicPedestalProvider", m_dynamicPedestalProvider );
}

//================ Destructor =================================================

L1TriggerTowerToolRun3::~L1TriggerTowerToolRun3() {}

//================ Initialisation =================================================

StatusCode L1TriggerTowerToolRun3::initialize()
{

  // Reading the Condition Containers Run 3 style  
  ATH_CHECK( m_runParametersContainer.initialize() );
  ATH_CHECK( m_pprConditionsContainerRun2.initialize() );
  ATH_CHECK( m_pprDisabledChannelContainer.initialize() );
  ATH_CHECK( m_ppmFineTimeRefsContainer.initialize() );

  ATH_CHECK(m_l1CaloTTIdTools.retrieve());

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

  if (!m_mappingTool.empty()) {
    ATH_CHECK( m_mappingTool.retrieve() );
  }

  // Incident Service:
  IIncidentSvc* incSvc = 0;
  if (service("IncidentSvc", incSvc).isFailure()) {
    ATH_MSG_WARNING( "Unable to retrieve pointer to IncidentSvc " );
  }
  else {
    incSvc->addListener(this, "BeginRun");
  }

  // Pedestal Correction
  if (m_correctFir) {
    ATH_CHECK(m_dynamicPedestalProvider.retrieve());
    ATH_MSG_INFO( "Retrieved L1DynamicPedestalProvider: " << m_dynamicPedestalProvider ); 
  }

  ATH_CHECK( m_eventInfoKey.initialize() );
  
  ATH_CHECK( m_L1MenuKey.initialize() );
  
  

  
  ATH_MSG_INFO( "Initialization completed" );
  
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1TriggerTowerToolRun3::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Reset mapping table at start of run ============================

void L1TriggerTowerToolRun3::handle(const Incident& inc)
{
  if (inc.type()=="BeginRun") {
    ATH_MSG_DEBUG( "Resetting mapping table at start of run" );
   
    m_idTable.clear();
  }
}



/** All-in-one routine - give it the ADC counts and TT identifier, and
    it returns the results */
void L1TriggerTowerToolRun3::process(const std::vector<int> &digits, double eta, double phi, int layer,
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
void L1TriggerTowerToolRun3::process(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId,
                                 std::vector<int> &et, std::vector<int> &bcidResults,
                                 std::vector<int> &bcidDecisions, bool useJepLut /* = true */)
{
  
  ATH_MSG_DEBUG( "::process: ==== Entered Process ====" );
  ATH_MSG_DEBUG( "::process: digits: " << digits);
  ATH_MSG_DEBUG( " channelID: " << MSG::hex << channelId.id() << MSG::dec );
 
 
  /// Initialise
  et.clear();
  bcidResults.clear();
  bcidDecisions.clear();

  ATH_MSG_DEBUG( "::process: ---- FIR filter ----" );
     
  /// emulate FIR filter
  std::vector<int> filter;
  fir(digits, channelId, filter);
  std::vector<int> lutInput;
  dropBits(filter, channelId, lutInput);

  ATH_MSG_DEBUG( "::process: ---- BCID algorithms ----" );

  /// emulate the two BCID algorithms
  bcid(filter, digits, channelId, bcidResults);

  ATH_MSG_DEBUG( "::process: ---- BCID decisions ----" );

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  bcidDecisionRange(lutInput, digits, channelId, decisionRange);
  bcidDecision(bcidResults, decisionRange, bcidDecisions);

  ATH_MSG_DEBUG( "::process: ---- LUT ET calculation ----" );

  /// LUT ET calculation
  std::vector<int> lutOutput;
  if(useJepLut) jepLut(lutInput, channelId, lutOutput);
  else cpLut(lutInput, channelId, lutOutput);
  

  ATH_MSG_DEBUG( "::process: ---- use ET range ----" );

  /// Use ET range to return appropriate ET value
  /// do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably
  applyEtRange(lutOutput, decisionRange, channelId, et);

  ATH_MSG_DEBUG( "::process: ==== Leaving Process ====" );
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
void L1TriggerTowerToolRun3::simulateChannel(const xAOD::TriggerTower& tt, std::vector<int>& outCpLut, std::vector<int>& outJepLut, std::vector<int>& bcidResults, std::vector<int>& bcidDecisions) const {

  //If we have 80 MHz readout, we need to extract the 40 MHz samples. The central 80 MHz sample is always a 40 MHz sample. We use the cool database (runParameters folder) to understand if we are in 80MHz readout
  
  
  SG::ReadCondHandle<L1CaloRunParametersContainer> runParameters( m_runParametersContainer);
  unsigned int readoutConfigID   = runParameters->runParameters(1)->readoutConfigID(); 
  ATH_MSG_DEBUG("RunParameters:: readoutConfigID " <<  readoutConfigID);
  
  std::vector<uint16_t> digits40;

  if(readoutConfigID == 5 or readoutConfigID == 6){
    
    
    ATH_MSG_DEBUG("80 MHz readout detected, emulating 40 MHz samples");
    
    int nSlices = tt.adc().size();

    if((nSlices%4)==3){
      for (int i=0 ; i < (nSlices-1)/2 ; i++ ){
	digits40.push_back(tt.adc().at(2*i+1));
      }
    }
    else if((nSlices%4)==1){
      for (int i=0 ; i <= (nSlices-1)/2 ; i++){
	digits40.push_back(tt.adc().at(2*i));
      }
    }


  }else{
    ATH_MSG_DEBUG("40 MHz readout detected");
    digits40 = tt.adc();
  }
  
  const auto& digits = convertVectorType<int>(digits40);

  L1CaloCoolChannelId channelId {tt.coolId()}; 

  ATH_MSG_DEBUG( "::simulateChannel: ==== Entered Process ====" );
  ATH_MSG_DEBUG( "::simulateChannel: digits: " << digits );
  ATH_MSG_DEBUG( "::simulateChannel: channelID: " << MSG::hex << channelId.id() << MSG::dec );
  
  
  /// Initialise
  outCpLut.clear();
  outJepLut.clear();
  bcidResults.clear();
  bcidDecisions.clear();
      
  /// emulate FIR filter
  ATH_MSG_DEBUG( "::simulateChannel: ---- FIR filter ----" );
  std::vector<int> filter;
  fir(digits, channelId, filter);
  

  /// apply pedestal correction
  ATH_MSG_DEBUG( "::simulateChannel: ---- pedestalCorrection ----" );
  // the correction is only available for each LUT slice in the read-out (not ADC/Filter slice)
  // therefore we can only apply it to the #LUT central filter slices
  const std::size_t nCorr = tt.correctionEnabled().size();
  const std::size_t filterOffset = filter.size()/2 - nCorr/2;
  for(std::size_t iCorr = 0; iCorr < nCorr; ++iCorr) {
    filter[filterOffset + iCorr] -= tt.correction()[iCorr] * tt.correctionEnabled()[iCorr];
  }
  
  ATH_MSG_DEBUG("::simulateChannel: filter: " << filter);


  std::vector<int> lutInput;
  dropBits(filter, channelId, lutInput);

  ATH_MSG_DEBUG( "::simulateChannel: ---- BCID algorithms ---- ");

  /// emulate the two BCID algorithms
  bcid(filter, digits, channelId, bcidResults);


  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  bcidDecisionRange(lutInput, digits, channelId, decisionRange);
  bcidDecision(bcidResults, decisionRange, bcidDecisions);
  
  ATH_MSG_DEBUG( "::simulateChannel: bcidDecisionRange "  <<  decisionRange);
  ATH_MSG_DEBUG( "::simulateChannel: bcidDecisions "  <<  bcidDecisions);



  ATH_MSG_DEBUG( "::simulateChannel: ---- LUT ET calculation ----" );

  /// LUT ET calculation
  std::vector<int> cpLutOutput, jepLutOutput;
  cpLut(lutInput, channelId, cpLutOutput);
  jepLut(lutInput, channelId, jepLutOutput);

  ATH_MSG_DEBUG( "::simulateChannel: cpLut "  <<  cpLutOutput);
  ATH_MSG_DEBUG( "::simulateChannel: jepLut "  <<  jepLutOutput);

  ATH_MSG_DEBUG( "::simulateChannel: ---- use ET range ----" );

  /// Use ET range to return appropriate ET value
  /// do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably
  applyEtRange(cpLutOutput, decisionRange, channelId, outCpLut);
  applyEtRange(jepLutOutput, decisionRange, channelId, outJepLut);
  
  ATH_MSG_DEBUG( "::simulateChannel: cpLut applyETRange "  <<  outCpLut);
  ATH_MSG_DEBUG( "::simulateChannel: jepLut applyETRange "  <<  outJepLut);

  
  ATH_MSG_DEBUG( "::simulateChannel: ==== Leaving Process ====" );
}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vector of predicted BCID result words */
    
void L1TriggerTowerToolRun3::bcid(const std::vector<int> &filter, const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const
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

  ATH_MSG_DEBUG( "::bcid: bcidResults: " << output);

}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vector of predicted BCID result words */
    
void L1TriggerTowerToolRun3::bcid(const std::vector<int> &filter, const std::vector<int> &digits,
                                          unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &output) const 
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

  ATH_MSG_DEBUG( "::bcid: bcidResults: " << output);
}

/** Evaluate both peak-finder and saturated BCID algorithms and return
    vectors of predicted BCID result and decision words */
    
void L1TriggerTowerToolRun3::bcid(const std::vector<int> &filter, const std::vector<int> &lutInput, const std::vector<int> &digits, int energyLow, int energyHigh, int decisionSource, std::vector<unsigned int> &decisionConditions,
                              unsigned int strategy, int satLow, int satHigh, int satLevel, std::vector<int> &result, std::vector<int> &decision) const 
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

  ATH_MSG_DEBUG( "::bcid: bcidResults: " << result);
  
 

  /// evaluate BCID decisions
  std::vector<int> decisionRange;
  if (!(decisionSource&0x1)) etRange(digits, energyLow, energyHigh, decisionRange);
  else                     etRange(lutInput, energyLow, energyHigh, decisionRange);
  bcidDecision(result, decisionRange, decisionConditions, decision);
  ATH_MSG_DEBUG( "::bcid: bcidDecisions: " << decision);

}

namespace { // helper function
  template<class T>
  const std::vector<short int>* getFirCoefficients(unsigned int coolId,  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    auto settings = pprConditionsRun2->pprConditions(coolId);
    if(!settings) return nullptr;
    return &(settings->firCoefficients());
  }
} // anonymous namespace

/** This FIR simulation produces a vector of same length as digit vector,
    with peak positions corresponding. However, first 2 and last 2 FIR
    sums will be incomplete, and so are zeroed here */
void L1TriggerTowerToolRun3::fir(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const
{   
  
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);

  /// Get coefficients from COOL DB
  std::vector<int> firCoeffs;
  if(!m_pprConditionsContainerRun2.empty()) {
    const std::vector<short int>* hwCoeffs;
    hwCoeffs =   getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(),  pprConditionsRun2);
    if(hwCoeffs) {
      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs->size()); // avoid frequent reallocations
      for (auto &i : reverse(*hwCoeffs)) {
	firCoeffs.push_back(i);	
	  }
    } else ATH_MSG_WARNING( "::fir: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::fir: No Conditions Container retrieved" );

  ATH_MSG_DEBUG( "::fir: FIR coefficients: " << firCoeffs);
    

  fir(digits, firCoeffs, output);
}

/** This FIR simulation produces a vector of same length as digit vector, with
    peak positions corresponding. However, since there are less FIR coefficients
    than digits the first and last few sums will be incomplete, and so are
    zeroed here */
void L1TriggerTowerToolRun3::fir(const std::vector<int> &digits, const std::vector<int> &firCoeffs, std::vector<int> &output) const
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
  
  ATH_MSG_DEBUG( "::fir: output: " << output);

}

namespace {
  template<typename T>
  unsigned int getStrategy( SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    return pprConditionsRun2->peakFinderCond();
  }
}

/** Peak finder BCID */
void L1TriggerTowerToolRun3::peakBcid(const std::vector<int> &fir, const L1CaloCoolChannelId& /*channelId*/, std::vector<int> &output) const 
{
  unsigned int strategy = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  

  if(!m_pprConditionsContainerRun2.empty()) {
    strategy = getStrategy<L1CaloPprConditionsContainerRun2>( pprConditionsRun2);
  } else ATH_MSG_WARNING( "::peakBcid: No Conditions Container retrieved" );

  ATH_MSG_DEBUG( "::peakBcid: peak-finder strategy: " << strategy );
  
  peakBcid(fir, strategy, output);
}

/** Peak finder BCID */
void L1TriggerTowerToolRun3::peakBcid(const std::vector<int> &fir, unsigned int strategy, std::vector<int> &output) const
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
  
  ATH_MSG_DEBUG( "::peakBcid: output: " << output);

  
}

namespace { // helper function
  template<class T>
  std::tuple<bool, int, int, int> getSaturation(unsigned int coolId,  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    auto settings = pprConditionsRun2->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0, 0, 0);
    return std::make_tuple(true, settings->satBcidLevel(), settings->satBcidThreshLow(), 
                           settings->satBcidThreshHigh());
  }
} // anonymous namespace

/** Saturated pulse BCID */
void L1TriggerTowerToolRun3::satBcid(const std::vector<int> &digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{  
  int satLevel = 0;
  int satLow   = 0;
  int satHigh  = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if (!m_pprConditionsContainerRun2.empty()) {
    bool available = false;
    std::tie(available, satLevel, satLow, satHigh) = getSaturation<L1CaloPprConditionsContainerRun2>(channelId.id(),  pprConditionsRun2);
    if(!available) ATH_MSG_WARNING( "::satBcid: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::satBcid: No Conditions Container retrieved" );

  ATH_MSG_DEBUG( "::satBcid: satLevel: " << satLevel
      << " satLow: "  << satLow
      << " satHigh: " << satHigh );

  satBcid(digits, satLow, satHigh, satLevel, output);
}

/** Saturated pulse BCID */

void L1TriggerTowerToolRun3::satBcid(const std::vector<int> &digits, int satLow, int satHigh, int satLevel, std::vector<int> &output) const
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
  
  ATH_MSG_DEBUG( "::satBcid: output: " << output);

}

/** Evaluate BCID decision range */
namespace {
  template<typename T>
  unsigned int getDecisionSource( SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    return pprConditionsRun2->decisionSource();
  }
}

void L1TriggerTowerToolRun3::bcidDecisionRange(const std::vector<int>& lutInput, const std::vector<int>& digits, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{
  int decisionSource = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if (!m_pprConditionsContainerRun2.empty()) {
    decisionSource = getDecisionSource<L1CaloPprConditionsContainerRun2>(pprConditionsRun2);
    
  } else ATH_MSG_WARNING( "::bcidDecisionRange: No Conditions Container retrieved" );

  if (!(decisionSource&0x1)) etRange(digits, channelId, output);
  else                     etRange(lutInput, channelId, output);
  
  ATH_MSG_DEBUG( "::bcidDecisionRange: decisionSource: " << decisionSource);
  ATH_MSG_DEBUG( " output: " << output);


}

/** Evaluate BCID decision based on BCID word, ET range and channel ID */
namespace { // helper function
  template<class T>
  std::tuple<unsigned int, unsigned int, unsigned int> getBcidDecision( SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    return std::make_tuple(pprConditionsRun2->bcidDecision1(), pprConditionsRun2->bcidDecision2(), pprConditionsRun2->bcidDecision3());
  }
} // anonymous namespace
void L1TriggerTowerToolRun3::bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range , std::vector<int> &output) const 
{
  unsigned int decision1 = 0;
  unsigned int decision2 = 0;
  unsigned int decision3 = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if(!m_pprConditionsContainerRun2.empty()) {
    std::tie(decision1, decision2, decision3) = getBcidDecision<L1CaloPprConditionsContainerRun2>(pprConditionsRun2);
  } else ATH_MSG_WARNING( "::bcidDecision: No Conditions Container retrieved" );

  // Reverse the order! (see elog 97082 9/06/10)
  std::vector<unsigned int> mask = { decision3, decision2, decision1 };
  
  ATH_MSG_DEBUG( "::bcidDecision: masks: " << MSG::hex
         << decision3 << " " << decision2 << " " << decision1 << MSG::dec );

  bcidDecision(bcidResults, range, mask, output);
}

/** Evaluate BCID decision based on BCID word, ET range and decision mask */

void L1TriggerTowerToolRun3::bcidDecision(const std::vector<int> &bcidResults, const std::vector<int> &range, const std::vector<unsigned int> &mask, std::vector<int> &output) const 
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

  ATH_MSG_DEBUG( "::bcidDecision: output: " << output);


}

// TODO implement scale
void L1TriggerTowerToolRun3::cpLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const
{   
  int startBit = 0;
  int strategy = 0;
  int offset   = 0;
  double offsetReal = 0;
  int slope    = 0;
  int cut      = 0;
  unsigned short scale = 0;
  double pedMean = 0;
  int ped      = 0;
  int hwCoeffSum = 0;
  const std::vector<short int>* hwCoeffs;


  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);

  if(!m_pprConditionsContainerRun2.empty()) {
    auto settings = pprConditionsRun2->pprConditions(channelId.id());
    if (settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutCpStrategy();
      slope    = settings->lutCpSlope();
      cut      = settings->lutCpNoiseCut();
      scale    = settings->lutCpScale();
      ped      = settings->pedValue();
      pedMean  = settings->pedMean();

      hwCoeffs =  getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2);
    
      for( auto &coeffs :  *hwCoeffs) { 
	hwCoeffSum += coeffs;
      }
      if (strategy == 0){
        offsetReal = pedMean * hwCoeffSum / pow(2.,startBit);
      }
      else{
        offsetReal = pedMean * hwCoeffSum * slope / pow(2.,startBit) - slope/2.;
      }
      offset = static_cast<unsigned short>( offsetReal < 0. ? 0 : offsetReal + 0.5 );

      ATH_MSG_DEBUG( "::cpLut: Offset: offset/strategy/pedMean/firCoeffSum/startBit/slope: "
		       << offset << " " << strategy << " " << " " << pedMean << " " << hwCoeffSum << " " << startBit << " " << slope );
      
    } else ATH_MSG_WARNING( "::cpLut: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::cpLut: No Conditions Container retrieved" );

  ATH_MSG_DEBUG( "::cpLut: LUT strategy/offset/slope/cut/ped: "
          << strategy << " " << offset << " " << slope << " " << cut << " " << ped << " " );

  unsigned int noiseCut = 0;
  bool disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
  if(strategy == 2) {
    // take the global scale into account - translate strategy to 1 for Run-1 compatible treatment
    lut(fir, scale*slope, scale*offset, scale*cut, 1, disabled, output);
  } else if(strategy == 1 || strategy == 0){
    lut(fir, slope, offset, cut, strategy, disabled, output);
  } else {
        ATH_MSG_WARNING(" ::cpLut: Unknown stragegy: " << strategy);
	output.push_back(0); //avoid crashing with Unknown stragegy
  }
}

void L1TriggerTowerToolRun3::jepLut(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{   
  int startBit = 0;
  int strategy   = 0;
  int offset     = 0;
  double offsetReal = 0;
  int slope      = 0;
  int cut        = 0;
  unsigned short scale_db   = 0;
  unsigned short scale_menu = 0;
  int ped        = 0;
  double pedMean = 0;
  int hwCoeffSum = 0;
  const std::vector<short int>* hwCoeffs;
  short par1     = 0;
  short par2     = 0;
  short par3     = 0;
  short par4     = 0;

  if(!isRun2()) {
    // assert instead ?!
    ATH_MSG_WARNING("::jepLut: Run-1 data - behaviour undefined!");
  }
  
  
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  const EventContext& ctx = Gaudi::Hive::currentContext();

  if(! m_pprConditionsContainerRun2.empty()) {
    const auto settings = pprConditionsRun2->pprConditions(channelId.id());
    if (settings) {
      startBit =  settings->firStartBit();
      strategy   = settings->lutJepStrategy();
      slope      = settings->lutJepSlope();
      cut        = settings->lutJepNoiseCut();
      ped        = settings->pedValue();
      pedMean    = settings->pedMean();
      scale_db   = settings->lutJepScale();

      auto l1Menu = getL1Menu(ctx);
      scale_menu = l1Menu->thrExtraInfo().JET().jetScale(); // Retrieve scale param from menu
      
      if (strategy == 3) {
	par1  = settings->lutJepPar1();
	par2  = settings->lutJepPar2();
	par3  = settings->lutJepPar3();
	par4  = settings->lutJepPar4();
      }
      
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2);
      
      for( auto &coeffs :  *hwCoeffs) { 
	 hwCoeffSum += coeffs;
      }
      if (strategy == 0){
        offsetReal = pedMean * hwCoeffSum / pow(2.,startBit);
      }
      else{
        offsetReal = pedMean * hwCoeffSum * slope / pow(2.,startBit) - slope/2.;
      }
      offset = static_cast<unsigned short>( offsetReal < 0. ? 0 : offsetReal + 0.5 );

      ATH_MSG_VERBOSE( "::jepLut: Offset: offset/strategy/pedMean/firCoeffSum/startBit/slope: "
		       << offset << " " << strategy << " " << " " << pedMean << " " << hwCoeffSum << " " << startBit << " " << slope );

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
    lut(fir, scale_menu*slope, scale_menu*offset, scale_menu*cut, 1, disabled, output);
  }else if(strategy == 1 || strategy == 0) {
    lut(fir, slope, offset, cut, strategy, disabled, output);
  } else { 
    ATH_MSG_WARNING(" ::jepLut: Unknown stragegy: " << strategy);
    output.push_back(0);
  }
}

/** LUT simulation: pedestal subtraction, energy calibration and threshold */
void L1TriggerTowerToolRun3::lut(const std::vector<int> &fir, int slope, int offset, int cut, int strategy, bool disabled, std::vector<int> &output) const
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations
  
 
  for( auto it :  fir) { 
    int out = 0;
    if (!disabled) {
      if (strategy == 0 && it >= offset+cut) { // Original scheme
        out = ((it-offset)*slope + 2048)>>12;
      } else if (strategy == 1 && it*slope >= offset+cut) { // New scheme
        out = (it*slope - offset + 2048)>>12;
      }
      if (out < 0)                 out = 0;
      if (out > s_saturationValue) out = s_saturationValue;
    }
    output.push_back(out);
  }

  ATH_MSG_DEBUG( "::lut: output: " << output);

}

void L1TriggerTowerToolRun3::nonLinearLut(const std::vector<int> &fir, int slope, int offset, int cut, int scale, short par1, short par2, short par3, short par4, bool disabled, std::vector<int> &output) const
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations

  
  for( auto it :  fir) { 
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
      if (it * slope < offset + nll_noise * cut) {
        output.push_back(0);
        continue;
      }
      // actual calculation
      out = int((((int)(2048 + nll_slope * (it * slope - offset)))>>12) + nll_offset + nll_ampl * std::exp(nll_expo * (it * slope - offset)));

      if(out > s_saturationValue) out = s_saturationValue;
      if(out < 0)                 out = 0;
    }
    output.push_back(out);
  }
  ATH_MSG_DEBUG( "::nonLinearLut: output: " << output);

}
/** Use ET range to return appropriate ET value
    Do not test BCID here, since no guarantee enough ADC samples to evaluate it reliably */

void L1TriggerTowerToolRun3::applyEtRange(const std::vector<int>& lut, const std::vector<int>& range, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{
  bool disabled = disabledChannel(channelId);
  std::vector<int>::const_iterator itlut   = lut.begin();
  std::vector<int>::const_iterator itrange = range.begin();
  while ( itlut != lut.end() && itrange != range.end() ) {
    if (!disabled && satOverride((*itrange))) output.push_back(s_saturationValue);
    else                                                 output.push_back(*itlut);
    ++itlut;
    ++itrange;
  }
  
  ATH_MSG_DEBUG( "::applyEtRange: output: " << output);
 
}

/** Identify BCID decision range */
namespace { // helper function
  template<class T>
  std::tuple<bool, int, int> getBcidEnergyRange(unsigned int coolId, SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    auto settings = pprConditionsRun2->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0, 0);
    return std::make_tuple(true, settings->bcidEnergyRangeLow(), settings->bcidEnergyRangeHigh());
  }
} 

// anonymous namespace

void L1TriggerTowerToolRun3::etRange(const std::vector<int> &et, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{
  int energyLow  = 0;
  int energyHigh = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if (!m_pprConditionsContainerRun2.empty()) {
    bool available = false;
    std::tie(available, energyLow, energyHigh) = getBcidEnergyRange<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2);
    if(!available) ATH_MSG_WARNING("::etRange: No L1CaloPprConditions found");
  } else ATH_MSG_WARNING("::etRange: No Conditions Container retrieved");

  ATH_MSG_VERBOSE( "::etRange: energyLow: " << energyLow
                          << " energyHigh: "          << energyHigh);

  etRange(et, energyLow, energyHigh, output);
}

/** Identify BCID decision range */

void L1TriggerTowerToolRun3::etRange(const std::vector<int> &et, int energyLow, int energyHigh, std::vector<int> &output) const 
{
  output.clear();
  output.reserve(et.size()); // avoid frequent reallocations
  for( auto it :  et) { 
    if (it <= energyLow)       output.push_back(0);
    else if (it <= energyHigh) output.push_back(1);
    else                          output.push_back(2);
  }
  
  ATH_MSG_DEBUG( "::etRange: output: " << output);
  

}

/** Truncate FIR results for LUT input */
namespace { // helper function
  template<class T>
  std::tuple<bool, int> getFirStartBit(unsigned int coolId, SG::ReadCondHandle<L1CaloPprConditionsContainerRun2> pprConditionsRun2) {
    auto settings = pprConditionsRun2->pprConditions(coolId);
    if(!settings) return std::make_tuple(false, 0);
    return std::make_tuple(true, settings->firStartBit());
  }
} // anonymous namespace

void L1TriggerTowerToolRun3::dropBits(const std::vector<int> &fir, const L1CaloCoolChannelId& channelId, std::vector<int> &output) const 
{
  unsigned int start = 0;
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if(!m_pprConditionsContainerRun2.empty()) {
    bool available = false;
    std::tie(available, start) = getFirStartBit<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2);
    if(!available)ATH_MSG_WARNING( "::dropBits: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::dropBits: No Conditions Container retrieved" );

  ATH_MSG_DEBUG( "::dropBits: firStartBit: " << start );
  
  dropBits(fir, start, output);
}

/** Truncate FIR results for LUT input */

void L1TriggerTowerToolRun3::dropBits(const std::vector<int> &fir, unsigned int start, std::vector<int> &output) const
{
  output.clear();
  output.reserve(fir.size()); // avoid frequent reallocations

  /// mask to select appropriate 10 bit range
  unsigned int mask = (0x3ff<<start);

  /// overflow detection
  int max = 1<<(10+start);

  for( auto it :  fir) { 
    if (it>=max) output.push_back(0x3ff);
    else output.push_back((it&mask)>>start);
  }

  ATH_MSG_DEBUG( "::dropBits: output: " << output);
}

/** Return FIR filter parameters for a channel */
void L1TriggerTowerToolRun3::firParams(const L1CaloCoolChannelId& channelId, std::vector<int> &firCoeffs) const 
{   
  /// Get coefficients from COOL DB
  firCoeffs.clear();
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  if(!m_pprConditionsContainerRun2.empty()) {
    const std::vector<short int>* hwCoeffs = nullptr;
    hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2 );
    
    if(hwCoeffs) {
      /** Ordering of coeffs in hw makes sense for hw implementation, but is
          not most natural for processing vectors in software. So reverse order
          here before using */
      firCoeffs.reserve(hwCoeffs->size()); // avoid frequent reallocations
      for (int i = hwCoeffs->size()-1; i >= 0; --i) firCoeffs.push_back((*hwCoeffs)[i]);

    } else ATH_MSG_WARNING( "::firParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::firParams: No Conditions Container retrieved" );


  ATH_MSG_DEBUG( "::fir: FIR coefficients: " << firCoeffs);
  
}

/** Return BCID parameters for a channel */
void L1TriggerTowerToolRun3::bcidParams(const L1CaloCoolChannelId& channelId, int &energyLow, int &energyHigh, int &decisionSource, std::vector<unsigned int> &decisionConditions,
                                    unsigned int &peakFinderStrategy, int &satLow, int &satHigh, int &satLevel) const
{
  energyLow          = 0;
  energyHigh         = 0;
  decisionSource     = 0;
  decisionConditions.clear();
  peakFinderStrategy = 0;
  satLevel           = 0;
  satLow             = 0;
  satHigh            = 0;
  
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);

  if(!m_pprConditionsContainerRun2.empty()) {
    using std::get;
    std::tuple<unsigned int, unsigned int, unsigned int> bcidDecision;
    std::tuple<bool, int, int> bcidEnergyRange;
    std::tuple<bool, int, int, int> saturation;
    
    using Cont = L1CaloPprConditionsContainerRun2;
    bcidDecision = getBcidDecision<Cont>(pprConditionsRun2);
    peakFinderStrategy = getStrategy<Cont>(pprConditionsRun2);
    decisionSource = getDecisionSource<Cont>(pprConditionsRun2);
    bcidEnergyRange = getBcidEnergyRange<Cont>(channelId.id(), pprConditionsRun2);
    saturation = getSaturation<Cont>(channelId.id(), pprConditionsRun2);
    
    
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

  ATH_MSG_DEBUG( "::bcidParams: satLevel: " << satLevel
                   << " satLow: "  << satLow << " satHigh: " << satHigh << endmsg
                   << " energyLow: " << energyLow << " energyHigh: " << energyHigh << endmsg
                   << " decisionSource: " << decisionSource << " peakFinderStrategy: "
                   << peakFinderStrategy );

}

void L1TriggerTowerToolRun3::cpLutParams(const L1CaloCoolChannelId& channelId, int& startBit, int& slope, int& offset, int& cut, int& pedValue, float& pedMean, int& strategy, bool& disabled)
{
  startBit = 0;
  strategy = 0;
  offset   = 0;
  double offsetReal = 0;
  slope    = 0;
  cut      = 0;
  pedValue = 0;
  pedMean  = 0.;
  disabled = true;
  int hwCoeffSum = 0;
  const std::vector<short int>* hwCoeffs;
  
  if(!isRun2()) {
    // assert instead ?!
    ATH_MSG_WARNING("::cpLutParams: Run-1 data - behaviour undefined!");
  }

  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  if(!m_pprConditionsContainerRun2.empty()) {
    const auto settings = pprConditionsRun2->pprConditions(channelId.id());
    if(settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutCpStrategy();
      slope    = settings->lutCpSlope();
      cut      =  settings->lutCpNoiseCut();
      pedValue =  settings->pedValue();
      pedMean  =  settings->pedMean();
      
      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(), pprConditionsRun2);
      
      for( auto &coeffs :  *hwCoeffs) { 
	hwCoeffSum += coeffs;
      }
      if (strategy == 0){
	offsetReal = pedMean * hwCoeffSum / pow(2.,startBit);
      }
      else{
	offsetReal = pedMean * hwCoeffSum * slope / pow(2.,startBit) - slope/2.;
      }
      offset = static_cast<unsigned short>( offsetReal < 0. ? 0 : offsetReal + 0.5 );
      
      ATH_MSG_VERBOSE( "::jepLutParams: Offset: offset/strategy/pedMean/firCoeffSum/startBit/slope: "
		     << offset << " " << strategy << " " << " " << pedMean << " " << hwCoeffSum << " " << startBit << " " << slope );

    } else ATH_MSG_WARNING( "::cpLutParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::cpLutParams: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::cpLutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean );
  unsigned int noiseCut = 0;
  disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
}

void L1TriggerTowerToolRun3::jepLutParams(const L1CaloCoolChannelId& channelId, int& startBit, int& slope, int& offset, int& cut, int& pedValue, float& pedMean, int& strategy, bool& disabled)
{
  startBit = 0;
  strategy = 0;
  offset   = 0;
  double offsetReal = 0;
  slope    = 0;
  cut      = 0;
  pedValue = 0;
  pedMean  = 0.;
  disabled = true;
  int hwCoeffSum = 0;
  const std::vector<short int>* hwCoeffs;
  
  if(!isRun2()) {
    // assert instead ?!
    ATH_MSG_WARNING("::jepLutParams: Run-1 data - behaviour undefined!");
  }
  
  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  if(!m_pprConditionsContainerRun2.empty()) {
    const auto settings = pprConditionsRun2->pprConditions(channelId.id());
    if(settings) {
      startBit = settings->firStartBit();
      strategy = settings->lutJepStrategy();
      slope    = settings->lutJepSlope();
      cut      = settings->lutJepNoiseCut();
      pedValue = settings->pedValue();
      pedMean  = settings->pedMean();

      hwCoeffs = getFirCoefficients<L1CaloPprConditionsContainerRun2>(channelId.id(),pprConditionsRun2);
    
      for( auto &coeffs :  *hwCoeffs) { 
	hwCoeffSum += coeffs;
      }
      if (strategy == 0){
	offsetReal = pedMean * hwCoeffSum / pow(2.,startBit);
      }
      else{
	offsetReal = pedMean * hwCoeffSum * slope / pow(2.,startBit) - slope/2.;
      }
      offset = static_cast<unsigned short>( offsetReal < 0. ? 0 : offsetReal + 0.5 );
      
      ATH_MSG_VERBOSE( "::jepLutParams: Offset: offset/strategy/pedMean/firCoeffSum/startBit/slope: "
		       << offset << " " << strategy << " " << " " << pedMean << " " << hwCoeffSum << " " << startBit << " " << slope );
      
    } else ATH_MSG_WARNING( "::jepLutParams: No L1CaloPprConditions found" );
  } else ATH_MSG_WARNING( "::jepLutParams: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::jepLutParams: LUT startBit/strategy/offset/slope/cut/pedValue/pedMean: "
          << startBit << " " << strategy << " " << offset << " " << slope << " " << cut << " " << pedValue << " " << pedMean );
  unsigned int noiseCut = 0;
  disabled = disabledChannel(channelId, noiseCut);
  if (noiseCut > 0) cut = noiseCut;
}

/** Return offline identifier for given tower coordinates */

Identifier L1TriggerTowerToolRun3::identifier(double eta, double phi, int layer)
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

HWIdentifier L1TriggerTowerToolRun3::hwIdentifier(const Identifier& id)
{
   HWIdentifier hwId(0);
   if (m_ttSvc) {
     try   { hwId = m_ttSvc->createTTChannelID(id, false); }
     catch (const CaloID_Exception&) { hwId = HWIdentifier(0); }
   }
   return hwId;
}

/** Return online identifier for given tower coordinates */

HWIdentifier L1TriggerTowerToolRun3::hwIdentifier(double eta, double phi, int layer)
{
   Identifier id = identifier(eta, phi, layer);
   return hwIdentifier(id);
}

/** Return Cool channel identifier for given tower coordinates */

L1CaloCoolChannelId L1TriggerTowerToolRun3::channelID(double eta, double phi, int layer)
{
  // Use direct lookup table if possible
  const double absEta = std::abs(eta);
  int index = 0;
  if (absEta < 2.5) {
    const int etaBin = 10.*absEta;
    const int phiBin = phi*(32/M_PI);
    index = (etaBin<<6) + phiBin;
  } else if (absEta < 3.2) {
    const int etaBin = 5.*(absEta - 2.5);
    const int phiBin = phi*(16./M_PI);
    index = 1600 + (etaBin<<5) + phiBin;
  } else {
    const int etaBin = (absEta - 3.2)*(1./0.425);
    const int phiBin = phi*(8./M_PI);
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

L1CaloCoolChannelId L1TriggerTowerToolRun3::channelID(const Identifier& id)
{
  L1CaloCoolChannelId coolId(0);
  if (m_ttSvc) {
    try {
      HWIdentifier hwId = hwIdentifier(id);
      coolId = m_ttSvc->createL1CoolChannelId(hwId);
    }
    catch (const CaloID_Exception&) { coolId = L1CaloCoolChannelId(0); }
  }
  return coolId;
}

/** Return saturation override flag for given channel & et range */
namespace { // helper function
  template<class T>
  std::tuple<bool, bool, bool> getSatOverride(SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2) {
    return std::make_tuple(pprConditionsRun2->satOverride1(), pprConditionsRun2->satOverride2(), pprConditionsRun2->satOverride3());
  }
} // anonymous namespace

bool L1TriggerTowerToolRun3::satOverride(int range) const 
{

  SG::ReadCondHandle<L1CaloPprConditionsContainerRun2>  pprConditionsRun2( m_pprConditionsContainerRun2);
  
  bool override = false;
  if(!m_pprConditionsContainerRun2.empty()) {
    std::tuple<bool, bool, bool> satOverride;
    satOverride = getSatOverride<L1CaloPprConditionsContainerRun2>(pprConditionsRun2);
    // NB Reverse order as for bcidDecision1/2/3
    if (range == 0) override = std::get<2>(satOverride);
    else if (range == 1) override = std::get<1>(satOverride);
    else if (range == 2) override = std::get<0>(satOverride);
  } else ATH_MSG_WARNING( "::satOverride: No Conditions Container retrieved" );

  ATH_MSG_VERBOSE( "::satOverride: range " << range
          << " has saturation override flag " << override );
  
  return override;
}

/** Check for disabled channel */

bool L1TriggerTowerToolRun3::disabledChannel(const L1CaloCoolChannelId& channelId) const
{
  unsigned int noiseCut = 0;
  return disabledChannel(channelId, noiseCut);
}

/** Check for disabled channel with noise cut */
bool L1TriggerTowerToolRun3::disabledChannel(const L1CaloCoolChannelId& channelId, unsigned int& noiseCut) const 
{

  SG::ReadCondHandle<L1CaloPprDisabledChannelContainerRun2>  pprDisabledChannel(m_pprDisabledChannelContainer);
  bool isDisabled = false;
  noiseCut = 0;
  if(!m_pprDisabledChannelContainer.empty()) {
    
    const auto disabledChan =  pprDisabledChannel->pprDisabledChannel(channelId.id());
       
    if (disabledChan) {
      if (!disabledChan->disabledBits()) {
        ChanCalibErrorCode calibError(disabledChan->calibErrorCode());
	if (calibError.chanValid()) {
          ChanDeadErrorCode deadError(disabledChan->deadErrorCode());
          if (deadError.chanValid()) noiseCut = disabledChan->noiseCut();
      //else isDisabled = true;
        } //else isDisabled = true;
      } else isDisabled = true;
      
      ATH_MSG_DEBUG( MSG::hex
              << "::disabledChannel: calibErrorCode: " << (disabledChan->calibErrorCode()).errorCode()
              << "  deadErrorCode: " << (disabledChan->deadErrorCode()).errorCode()
              << "  noiseCut: "      << disabledChan->noiseCut()
              << "  disabledBits: "  << disabledChan->disabledBits()
              << MSG::dec );
      
    } else {
      ATH_MSG_DEBUG( "::disabledChannel: No L1CaloPprDisabledChannel found" );
    }
  } else {
    ATH_MSG_WARNING( "::disabledChannel: No DisabledChannel Container retrieved" );
  }
  if (isDisabled) ATH_MSG_DEBUG( "::disabledChannel: Channel is disabled" );

  return isDisabled;
}


/** Calculate median eta of FCAL trigger tower from nominal eta and layer.
    Hopefully this will be a short-term work-around */

double L1TriggerTowerToolRun3::FCalTTeta(double nominalEta, double /*phi*/, int layer)
{
  double eta = nominalEta;
  float abseta = std::abs(eta);
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

double L1TriggerTowerToolRun3::FCalTTeta(const L1CaloCoolChannelId& channelId)
{
  if ( !m_mappingTool.isValid() ) {
    throw GaudiException("No mapping tool configured",
                         "L1TriggerTowerToolRun3::FCalTTeta", StatusCode::FAILURE);
  }

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



std::pair<double, double> L1TriggerTowerToolRun3::refValues(const L1CaloCoolChannelId& channelId)
{
  //method returning the fine time reference and calibration value
  //the fineTimeReference folder has to be loaded first using the method L1TriggerTowerToolRun3::loadFTRefs
  double reference = 0;
  double calib = 0;
  SG::ReadCondHandle<L1CaloPpmFineTimeRefsContainer> ppmFineTimeRefs( m_ppmFineTimeRefsContainer);
  
  if (!m_ppmFineTimeRefsContainer.empty()) {
    const L1CaloPpmFineTimeRefs* ftref = ppmFineTimeRefs->ppmFineTimeRefs(channelId.id());
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

void L1TriggerTowerToolRun3::pedestalCorrection(std::vector<int>& firInOut, int firPed, int iElement, int layer, int bcid, float mu, std::vector<int_least16_t>& correctionOut) {
  unsigned nFIR = firInOut.size();
  correctionOut.assign(nFIR, 0u);

  if(!m_correctFir) return;

  // apply the pedestal correction
  for(unsigned i = 0; i != nFIR; ++i) {
    correctionOut[i] = (m_dynamicPedestalProvider->dynamicPedestal(iElement, layer, firPed, bcid + i - nFIR/2, mu) - firPed);
    firInOut[i] -= correctionOut[i];

    if(firInOut[i] < 0) firInOut[i] = 0;
  }

  ATH_MSG_DEBUG( "::pedestalCorrection(BCID=" << bcid << ", mu = " << mu << "): " << correctionOut);

} 

bool L1TriggerTowerToolRun3::isRun2() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (ctx.eventID().run_number() >= 253377) return true;

  SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
  if (eventInfo->eventType (xAOD::EventInfo::IS_SIMULATION)) return true;
  return false;
}



const TrigConf::L1Menu* L1TriggerTowerToolRun3::getL1Menu(const EventContext& ctx) const {
  const TrigConf::L1Menu* menu = nullptr;

  StatusCode sc =   m_configSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve trigger configuration service" );
  } 
  
  if (detStore()->contains<TrigConf::L1Menu>(m_L1MenuKey.key())) {
    SG::ReadHandle<TrigConf::L1Menu>  l1MenuHandle = SG::makeHandle( m_L1MenuKey, ctx );
    if( l1MenuHandle.isValid() ){
      menu=l1MenuHandle.cptr();
    }
  } else {
    menu = &(m_configSvc->l1Menu(ctx)); 
  }

  return menu;
}



} // end of namespace
 



