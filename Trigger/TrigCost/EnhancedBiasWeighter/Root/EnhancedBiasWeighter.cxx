// EnhancedBiasWeighter includes
#include "EnhancedBiasWeighter/EnhancedBiasWeighter.h"

#include <memory>
#include "PathResolver/PathResolver.h"

#include <TXMLEngine.h> 
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>

namespace {
  template <typename T>
    T stringToNum(const std::string& i) {
      T ret;
      std::istringstream( i ) >> ret;
      return ret;
    }
}

/**
 * @class EnhancedBiasWeighter
 * Tool to calculate data-driven rates using EnhancedBias datasets. 
 */
EnhancedBiasWeighter::EnhancedBiasWeighter( const std::string& name )
  : asg::AsgTool( name ),
  m_deadtime(1.),
  m_pairedBunches(0),
  m_mcModifiedCrossSection(0.),
  m_lumiAverageNum(0),
  m_muAverageNum(0),
  m_averageDenom(0)
{
  declareProperty( "RunNumber", m_runNumber = 0);
  declareProperty( "CalculateWeightingData", m_calculateWeightingData = true, "If true, read from COOL, CONDBR2 and XMLs. If false, read directly from decorated TRIG1 dAOD.");
  declareProperty( "EnforceEBGRL", m_enforceEBGRL = true, "Each Enhanced Bias run has a 'good run list' style veto on some LB. If this flag is true, events in these LB get weight 0");
  declareProperty( "IsMC", m_isMC = false, "MC mode? If so we need a cross section and filter efficiency");
  declareProperty( "MCCrossSection", m_mcCrossSection = 0, "If running over MC. The process cross section in nb (AMI gives thins in nb)");
  declareProperty( "MCFilterEfficiency", m_mcFilterEfficiency = 0, "If running over MC. The process filter efficiency (0.0-1.0)");
  declareProperty( "MCKFactor", m_mcKFactor = 1., "If running over MC. Higher-order corrections fudge factor to the cross section");
  declareProperty( "InelasticCrossSection", m_inelasticCrossSection = 8e-26, "Inelastic cross section in units cm^2. Default 80 mb at 13 TeV.");
  declareProperty( "UseBunchCrossingTool", m_useBunchCrossingTool = true, "BunchCrossing tool requires CONDBR2 access. Can be disabled here if this is a problem.");
}

/**
 * Check if we're MC or data, and if data check if we're to load the weighting info or read it from the input file
 */
StatusCode EnhancedBiasWeighter::initialize() 
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  if (m_calculateWeightingData == true) {

    if (m_isMC) {

      if (m_mcCrossSection == 0 || m_mcFilterEfficiency == 0) {
        ATH_MSG_FATAL("For MC rates, a cross section and filter efficiency must be supplied.");
        return StatusCode::FAILURE;
      }
      m_bcTool.setTypeAndName("Trig::MCBunchCrossingTool/BunchCrossingTool");
      m_deadtime = 1.; // No deadtime for MC
      m_pairedBunches = FULL_RING; // Assume full-ring
      const float mcCrossSectionInSqCm = 1e-33 * m_mcCrossSection; // Convert nb -> cm^2 
      m_mcModifiedCrossSection = mcCrossSectionInSqCm * m_mcFilterEfficiency * m_mcKFactor;
      ATH_MSG_INFO ("Running over MC with xsec:" << m_mcCrossSection << " nb, filter efficiency:" << m_mcFilterEfficiency << ", k-factor:" << m_mcKFactor);

    } else { // isData

      if (m_runNumber == 0) {
        ATH_MSG_FATAL("calculateWeightingData is TRUE, but the RunNumber property has not been set. This must be set such that we can read in the correct data.");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO ("calculateWeightingData is TRUE. This job will read in EnhancedBias weighting data from CVMFS and COOL.");
      ATH_CHECK( loadWeights() );
      ATH_CHECK( loadLumi() );
      m_bcTool.setTypeAndName("Trig::LHCBunchCrossingTool/BunchCrossingTool");

    } // end isData

    if (m_useBunchCrossingTool) ATH_CHECK( m_bcTool.retrieve() );

  } else {
    ATH_MSG_INFO ("calculateWeightingData is FALSE. This job must be running over an EnhancedBias TRIG1 dAOD which has already been decorated with weighting data.");
  }

  return StatusCode::SUCCESS;
}

/**
 * Nothing to do
 */
StatusCode EnhancedBiasWeighter::finalize() 
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


/**
 * Read into memory from XML event weights for this EnhancedBias run.
 */
StatusCode EnhancedBiasWeighter::loadWeights()
{
  // Construct name
  std::stringstream fileNameDev, fileName;
  fileName    << "TrigCostRootAnalysis/EnhancedBiasWeights_" << m_runNumber << ".xml";
  fileNameDev << "dev/TrigCostRootAnalysis/EnhancedBiasWeights_" << m_runNumber << ".xml";

  std::string weightingFile = PathResolverFindCalibFile( fileName.str() );  // Check standard area
  if (weightingFile == "") {
    weightingFile = PathResolverFindCalibFile( fileNameDev.str() ); // Else check DEV area
  }

  if (weightingFile == "") {
    ATH_MSG_WARNING ("Could not retrieve " << fileName.str() << " or " << fileNameDev.str() << ", cannot perform enhanced bias weighting.");
    return StatusCode::FAILURE;
  }

  std::unique_ptr<TXMLEngine> xml(new TXMLEngine() );
  XMLDocPointer_t xmlDoc = xml->ParseFile( weightingFile.c_str() );

  if (xmlDoc == nullptr) {
    ATH_MSG_WARNING ("Could not parse " << fileName.str() << ", cannot perform enhanced bias weighting.");
    return StatusCode::FAILURE;
  }

  // Navigate XML
  const XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
  assert( xml->GetNodeName(mainNode) == std::string("run") );
  const XMLNodePointer_t weightsNode = xml->GetChild( mainNode );
  const XMLNodePointer_t eventsNode  = xml->GetNext( weightsNode );

  XMLNodePointer_t weightNode = xml->GetChild( weightsNode );
  XMLNodePointer_t eventNode  = xml->GetChild( eventsNode );

  while ( weightNode != 0 ) { // Loop over all weight elements
    assert( xml->GetNodeName(weightNode) == std::string("weight") );

    const int32_t id   = std::atoi(xml->GetAttr(weightNode, "id") );
    const double weight = std::atof(xml->GetAttr(weightNode, "value") );
    int32_t unbiased = 0;
    if ( xml->HasAttr(weightNode, "unbiased") == true ) {
      unbiased = std::atoi(xml->GetAttr(weightNode, "unbiased"));
    }

    m_idToWeightMap[id] = weight;
    m_idToUnbiasedMap[id] = unbiased;

    weightNode = xml->GetNext(weightNode);
  }

  while ( eventNode != 0 ) { // Loop over all event elements
    assert( xml->GetNodeName(eventNode) == std::string("e") ); //Event
    const uint64_t eventNumber  = std::strtoll(xml->GetAttr(eventNode, "n"), nullptr, 10); //Number
    const int32_t eventWeightID = std::stoi(xml->GetAttr(eventNode, "w") ); //Weight ID

    m_eventNumberToIdMap[eventNumber] = eventWeightID;

    eventNode = xml->GetNext(eventNode);
  }

  ATH_MSG_INFO ("Loaded " << m_eventNumberToIdMap.size() << " event weights for run " << m_runNumber);
  return StatusCode::SUCCESS;
}

/**
 * Read into memory this EnhancedBias run's XML
 */
StatusCode EnhancedBiasWeighter::loadLumi()
{
  // Fetch LB time from COOL for this run
  if (m_readLumiBlock.updateLumiBlocks(m_runNumber, msg()) == false) {
    ATH_MSG_FATAL("Unable to load this runs luminosity values from COOL.");
    return StatusCode::FAILURE;
  }

  // Read in number of events to expect 
  // Construct name
  std::stringstream fileNameDev, fileName;
  fileName    << "TrigCostRootAnalysis/enhanced_bias_run_" << m_runNumber << ".xml";
  fileNameDev << "dev/TrigCostRootAnalysis/enhanced_bias_run_" << m_runNumber << ".xml";

  std::string runFile = PathResolverFindCalibFile( fileName.str() );  // Check standard area
  if (runFile == "") {
    runFile = PathResolverFindCalibFile( fileNameDev.str() ); // Else check DEV area
  }

  if (runFile == "") {
    ATH_MSG_FATAL ("Could not retrieve " << fileName.str() << ", cannot perform enhanced bias weighting.");
    return StatusCode::FAILURE;
  }

  std::unique_ptr<TXMLEngine> xml(new TXMLEngine());
  const XMLDocPointer_t xmlDoc = xml->ParseFile( runFile.c_str() );

  if (xmlDoc == nullptr) {
    ATH_MSG_FATAL ("Could not parse " << fileName.str() << ", cannot perform enhanced bias weighting.");
    return StatusCode::FAILURE;
  }

  const XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
  assert( xml->GetNodeName(mainNode) == std::string("trigger") );
  XMLNodePointer_t listNode = xml->GetChild( mainNode );

  while ( listNode != 0 ) { // Loop over all menu elements
    const std::string listName = xml->GetNodeName(listNode);

    if (listName == "lb_list") {
      
      XMLNodePointer_t node = xml->GetChild( listNode );
      while( node != 0) {
        assert( xml->GetNodeName(node) == std::string("lb") );
        const uint32_t lb      = std::atoi( xml->GetAttr(node, "id") );
        const double   lumi    = std::atof( xml->GetAttr(node, "lumi") );
        const uint32_t nEvents = std::atoi( xml->GetNodeContent(node) );
        const std::string flag = xml->HasAttr(node, "flag") ? xml->GetAttr(node, "flag") : "";  

        m_eventsPerLB[lb] = nEvents;
        m_goodLB[lb] = (flag == "bad" ? 0 : 1); 
        m_lumiPerLB[lb] = lumi < 1e10 ? 1e30 * lumi : lumi;

        node = xml->GetNext(node); 
      }

    } else if (listName == "lumivalues") {

      XMLNodePointer_t node = xml->GetChild( listNode );
      while( node != 0) { 
        if ( xml->GetNodeName(node) == std::string("deadtime") ) {
          m_deadtime = 1. + std::atof( xml->GetNodeContent(node) );
        }
        node = xml->GetNext(node);
      }
      
    } else if (listName == "bunchgroups") {

      XMLNodePointer_t node = xml->GetChild( listNode );
      while( node != 0) { 
        if ( xml->GetNodeName(node) == std::string("bunchgroup") && xml->HasAttr(node, "name") &&
            (std::string(xml->GetAttr(node, "name")) == "Paired" || std::string(xml->GetAttr(node, "name")) == "Filled")) {
          m_pairedBunches = std::atoi( xml->GetNodeContent(node) );
        }
        node = xml->GetNext(node);
      }
      
    }

    listNode = xml->GetNext(listNode);
  }

  ATH_MSG_INFO ("Loaded " << m_eventsPerLB.size() << " EnhancedBias lumi block's info for run " << m_runNumber);
  return StatusCode::SUCCESS;
}

/**
 * To make most use of the XML parsing available already in this class
 * @param prescaleXML File name of a prescales file as generated by the RuleBook
 * @return Set of L1 or HLT chain names mapped to their desired prescale
 */
std::unordered_map<std::string, ChainDetail> EnhancedBiasWeighter::parsePrescaleXML(const std::string& prescaleXML) const {
  std::unordered_map<std::string, ChainDetail> result;

  std::string xmlFile = PathResolverFindDataFile( prescaleXML ); 
  if (xmlFile == "") {
    ATH_MSG_ERROR ("Could not retrieve " << prescaleXML << ", place it somewhere PathResolver can find it (such as the current directory).");
    return result;
  }

  std::unique_ptr<TXMLEngine> xml(new TXMLEngine());
  const XMLDocPointer_t xmlDoc = xml->ParseFile( xmlFile.c_str() );

  if (xmlDoc == nullptr) {
    ATH_MSG_WARNING ("Could not parse " << prescaleXML << ", please check that it is valid XML.");
    return result;
  }

  // Get access to main node
  XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
  assert( xml->GetNodeName(mainNode) == std::string("trigger") );
  XMLNodePointer_t listNode = xml->GetChild( mainNode );


  while ( listNode != nullptr) { // Loop over all menu elements
    const std::string listName = xml->GetNodeName(listNode);

    // if (_listName == "PredictionLumi") {
    //   Float_t predictionLumi = stringToFloat( xml->GetNodeContent(listNode) );
    // }
    if (listName != "level") { // Find the "level" item
      listNode = xml->GetNext(listNode);
      continue;
    }

    XMLNodePointer_t sigNode = xml->GetChild( listNode );
    while( sigNode != nullptr) {
      if (xml->GetNodeName(sigNode) != std::string("signature")) { // Find the "signature" items
        sigNode = xml->GetNext(sigNode);
        continue;
      }

      XMLNodePointer_t sigDetailsNode = xml->GetChild( sigNode );
      std::string chainName;
      while( sigDetailsNode != nullptr) {

        if (xml->GetNodeContent(sigDetailsNode) == nullptr) {
          sigDetailsNode = xml->GetNext(sigDetailsNode);
          continue;
        }

        const std::string detail = xml->GetNodeName(sigDetailsNode);
        if (detail == "sig_name") {
          chainName = xml->GetNodeContent(sigDetailsNode);
          result[chainName] = ChainDetail();
        } else if (detail == "sig_counter") {
          result[chainName].m_counter = std::stoi( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "prescale" || detail == "chain_prescale") { // This is an alternate name
          result[chainName].m_prescale = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "lower_chain_name") {
          // Later processing here does not expect any spaces, so remove them now. Pure comma separated list
          std::string lower = xml->GetNodeContent(sigDetailsNode);
          while (lower.find(" ") != std::string::npos) lower.replace( lower.find(" "), 1, "");
          result[chainName].m_lowerName = lower;
        } else if (detail == "evts_passed") {
          result[chainName].m_eventsPassed = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "evts_passed_weighted") {
          result[chainName].m_eventsPassedWeighted = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "rate") {
          result[chainName].m_rate = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "rate_err") {
          result[chainName].m_rateErr = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "passthrough") {
          result[chainName].m_passthroughPrescale = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "rerun_prescale") {
          result[chainName].m_rerunPrescale = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "express_prescale") {
          result[chainName].m_expressPrescale = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "efficiency") {
          result[chainName].m_efficiency = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "efficiency_err") {
          result[chainName].m_efficiencyErr = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "prescaled_efficiency") {
          result[chainName].m_prescaledEfficiency = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "prescaled_efficiency_error") {
          result[chainName].m_prescaledEfficiencyErr = std::stod( xml->GetNodeContent(sigDetailsNode) );
        } else if (detail == "comment") {
          result[chainName].m_comment = xml->GetNodeContent(sigDetailsNode);
        }

        sigDetailsNode = xml->GetNext(sigDetailsNode);
      }
      sigNode = xml->GetNext(sigNode);
    }
    listNode = xml->GetNext(listNode);
  }

  return result;
}

/**
 * @return Internal mapping of an event number to a weight ID.
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
int32_t EnhancedBiasWeighter::getEventEBID(const xAOD::EventInfo* eventInfo) const
{
  const uint64_t eventNumber = eventInfo->eventNumber();

  const auto mapIterator = m_eventNumberToIdMap.find(eventNumber);
  if (mapIterator != m_eventNumberToIdMap.end()) {
    return mapIterator->second;
  }
  // Unfortunately, earlier weighting XMLs have 32 bit signed event number. Hence we also have to try this option
  const int32_t eventNumber32 = static_cast<int32_t>(eventNumber);
  const auto mapIterator32 = m_eventNumberToIdMap.find(eventNumber32);
  if (mapIterator32 != m_eventNumberToIdMap.end()) {
    return mapIterator32->second;
  } else {
    ATH_MSG_ERROR( "Couldn't find enhanced bias info for event " << eventNumber);
    return -1;
  } 
}

/**
 * @return The effective number of events this one event represents from online, given EnhancedBias online prescales.
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 * For MC, this is the generator weight
 */
double EnhancedBiasWeighter::getEBWeight(const xAOD::EventInfo* eventInfo) const
{

  if (m_enforceEBGRL && !isGoodLB(eventInfo)) {
    return 0;
  }

  ATH_CHECK( trackAverages(eventInfo) );

  if (m_calculateWeightingData) {

    if (m_isMC) {

      const std::vector<float> weights = eventInfo->mcEventWeights();
      if (weights.size() > 0) return weights[0];
      return 1.;

    } else { // isData

      int32_t ebID = getEventEBID(eventInfo);
      const auto mapIterator = m_idToWeightMap.find(ebID);
      if (mapIterator == m_idToWeightMap.end() ) {
        ATH_MSG_ERROR( "Couldn't find enhanced bias weight for event with ID " << ebID);
        return 0;
      }
      return mapIterator->second;

    } // isData

  } else {

    return eventInfo->auxdata<double>("EnhancedBiasWeight");

  }
}

/**
 * Internal function to keep track of the mean instantaneous lumi & mean pileup of the EB/MC sample being processed.
 */
StatusCode EnhancedBiasWeighter::trackAverages(const xAOD::EventInfo* eventInfo) const 
{
  m_muAverageNum   += std::ceil( eventInfo->actualInteractionsPerCrossing() );
  m_lumiAverageNum += getLBLumi(eventInfo);
  ++m_averageDenom;
  return StatusCode::SUCCESS;
}

/**
 * @return The amount of online walltime contributed by this event based on the LB length and the number of events in the LB
 * For MC, this is fixed by the sample cross section, the inelastic cross section and the mu of the current event.
 * Retrieved from COOL and CVMFS XML or fetched from TRIG1 dAOD
 */
double EnhancedBiasWeighter::getEBLiveTime(const xAOD::EventInfo* eventInfo) const
{


  if (m_calculateWeightingData) {

      if (m_isMC) {

        // Probability that a single pp interaction yields this MC process (ratio of xsec)
        const double probOfProcess = m_mcModifiedCrossSection / m_inelasticCrossSection; 
        // Probability that a single bunch-crossing yeild this MC process. Binomial statistics (1 - Prob(exactly 0 interactions, given mu interactions)).
        const double probOfBunchCrossing = 1. - std::pow( 1. - probOfProcess, std::ceil(eventInfo->actualInteractionsPerCrossing()) );
        const double bunchCrossingRate = m_pairedBunches * LHC_FREQUENCY;
        // How much wall-time does this event represent? This is the reciprocal of the rate, which is the % per crossing scaled by the crossing rate.
        ATH_MSG_DEBUG("MC livetime debug: probOfProcess:" << probOfProcess << " probOfBunchCrossing:" << probOfBunchCrossing << " bunchCrossingRate:" << bunchCrossingRate << " time:" << (1. / (probOfBunchCrossing * bunchCrossingRate)));
        return 1. / (probOfBunchCrossing * bunchCrossingRate);

      } else {

        uint32_t lumiBlock = eventInfo->lumiBlock();

        // Check the cache
        const auto inCacheIterator = m_eventLivetime.find( lumiBlock );
        if (inCacheIterator != m_eventLivetime.end()) return inCacheIterator->second;

        // Else calculate
        const auto mapIterator = m_eventsPerLB.find(lumiBlock);
        if (mapIterator == m_eventsPerLB.end() ) {
          ATH_MSG_ERROR( "Couldn't find LB info for LB: " << lumiBlock );
          return 0;
        } 
        const int32_t eventsInThisLB = mapIterator->second;
        const double lbLength = m_readLumiBlock.getLumiBlockLength(lumiBlock, msg());
        // This event is one in eventsInThisLB, so has an effective temporal contribution of:
        double eventLivetime = 0;
        if (eventsInThisLB > 0 && fabs(lbLength) > 1e-10) eventLivetime = (1. / static_cast<double>(eventsInThisLB)) * lbLength;
        // Cache this (mutable)
        m_eventLivetime[lumiBlock] = eventLivetime;
        return eventLivetime;

      } // isData

  } else {

    return eventInfo->auxdata<double>("EnhancedBiasLivetime");

  }
}

/**
 * @return If the current event was selected online by a fully unbiased trigger 
 * for MC we cannot play such games and return True so that every trigger is queried.
 * the user is expected to know that a j0 trigger running on a ttbar MC is going to underestimate
 * the rate of the j0 trigger running on MinBias.
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
bool EnhancedBiasWeighter::isUnbiasedEvent(const xAOD::EventInfo* eventInfo) const
{
  if (m_calculateWeightingData) {
  
    if (m_isMC) {

      return true;

    } else { //isData

      int32_t ebID = getEventEBID(eventInfo);
      const auto mapIterator = m_idToUnbiasedMap.find(ebID);
      if (mapIterator == m_idToUnbiasedMap.end() ) {
        ATH_MSG_ERROR("Couldn't find isUnbiased information for event with ID " << ebID);
        return false;
      }
      return mapIterator->second;
    
    } // isData
  
  } else {

    return static_cast<bool>( eventInfo->auxdata<char>("IsUnbiasedEventFlag") );

  }
}

/**
 * @return If the current lumi block should be used for rates or vetoed due to online deadtime 
 * Not a concern for MC
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
bool EnhancedBiasWeighter::isGoodLB(const xAOD::EventInfo* eventInfo) const
{
  if (m_calculateWeightingData) {

    if (m_isMC) {
      
      return true;

    } else { // isData

      uint32_t lumiBlock = eventInfo->lumiBlock();

      const auto mapIterator = m_goodLB.find(lumiBlock);
      if (mapIterator == m_goodLB.end() ) {
        ATH_MSG_ERROR( "Couldn't find LB good/bad info for LB: " << lumiBlock );
        return false;
      }
      return static_cast<bool>(mapIterator->second);

    } // isData

  } else {

    return static_cast<bool>( eventInfo->auxdata<char>("IsGoodLBFlag") );

  }
}


/**
 * @return the instantaneous luminosity for the current lumi block in units of cm-2s-1. 
 * For MC we fix this value to something sensible throughout the processing.
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
double EnhancedBiasWeighter::getLBLumi(const xAOD::EventInfo* eventInfo) const
{
  if (m_calculateWeightingData) {

    if (m_isMC) {

      const double mu = std::ceil( eventInfo->actualInteractionsPerCrossing() );
      return (mu * LHC_FREQUENCY * m_pairedBunches) / m_inelasticCrossSection;

    } else { // isData

      uint32_t lumiBlock = eventInfo->lumiBlock();
      const auto mapIterator = m_lumiPerLB.find(lumiBlock);
      if (mapIterator == m_lumiPerLB.end() ) {
        ATH_MSG_ERROR( "Couldn't find lumi info for LB: " << lumiBlock );
        return 0.;
      }
      return mapIterator->second;

    } // isData

  } else {

    return eventInfo->auxdata<double>("LBLumi");

  }
}

/**
 * @return the fractional deadtime in the event. Currently a fixed number, in future may be reported as a fn. of LB.
 * There is no deadtime in MC, so for MC this will always return 1. 
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
double EnhancedBiasWeighter::getDeadtime() const
{
  return m_deadtime;
}

/**
 * @return The number of colliding bunches in the Enhaned Bias run being used.
 * For MC this will return the number of bunches which corresponds to a full-ring.
 * Retrieved from CVMFS XML or fetched from TRIG1 dAOD
 */
uint32_t EnhancedBiasWeighter::getPairedBunches() const
{
  return m_pairedBunches;
}

/**
 * @return How far into the current train this BCID is.
 * Retrieved from the database using the BunchCrossingTool or fetched from TRIG1 dAOD
 */
uint32_t EnhancedBiasWeighter::getDistanceIntoTrain(const xAOD::EventInfo* eventInfo) const
{
  if (m_calculateWeightingData) {

    if (!m_useBunchCrossingTool) return 0;
    return m_bcTool->distanceFromFront( eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings );

  } else {

    return eventInfo->auxdata<uint32_t>("BCIDDistanceFromFront");

  }
}

/**
 * @return The mean luminosity of all events
 */
double EnhancedBiasWeighter::getAverageLumi() const
{
  if (m_averageDenom == 0) return 0;
  return m_lumiAverageNum / (double)m_averageDenom;
}

double EnhancedBiasWeighter::getAverageMu() const 
{
  if (m_averageDenom == 0) return 0;
  return m_muAverageNum / (double)m_averageDenom;
}

/**
 * Decorate the AOD with EnhancedBias weighting quantities such that no CVMFS or DB access is required
 * on subsequent passes through the dAOD to perform rates.
 */
StatusCode EnhancedBiasWeighter::addBranches() const
{

  if (m_calculateWeightingData == false) {
    ATH_MSG_FATAL( "Cannot decorate AOD with EnhancedBias data if loadEnhancedBiasData is FALSE ");
    return StatusCode::FAILURE;
  }

  // Set up the decorator
  SG::AuxElement::Decorator< double >   decoratorEBWeight("EnhancedBiasWeight"); 
  SG::AuxElement::Decorator< double >   decoratorEBLivetime("EnhancedBiasLivetime"); 
  SG::AuxElement::Decorator< double >   decoratorLBLumi("LBLumi"); 
  SG::AuxElement::Decorator< double >   decoratorDeadtime("Deadtime"); 
  SG::AuxElement::Decorator< uint32_t > decoratorBCIDDistanceFromFront("BCIDDistanceFromFront"); 
  SG::AuxElement::Decorator< char >     decoratorUnbiasedFlag("IsUnbiasedEventFlag");
  SG::AuxElement::Decorator< char >     decoratorGoodLBFlag("IsGoodLBFlag");  

  const xAOD::EventInfo* eventInfo(nullptr);
  ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );

  decoratorEBWeight(*eventInfo) = getEBWeight(eventInfo);
  decoratorEBLivetime(*eventInfo) = getEBLiveTime(eventInfo);
  decoratorLBLumi(*eventInfo) = getLBLumi(eventInfo);
  decoratorUnbiasedFlag(*eventInfo) = isUnbiasedEvent(eventInfo);
  decoratorGoodLBFlag(*eventInfo) = isGoodLB(eventInfo);
  decoratorDeadtime(*eventInfo) = m_deadtime;
  decoratorBCIDDistanceFromFront(*eventInfo) = getDistanceIntoTrain(eventInfo);

  return StatusCode::SUCCESS;
}
