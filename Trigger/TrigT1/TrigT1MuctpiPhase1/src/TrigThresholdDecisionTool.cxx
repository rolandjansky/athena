/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

#include "TrigThresholdDecisionTool.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

namespace LVL1
{
  namespace MURoIThresholdsToolParams {
    const char ContainerName[] = "LVL1MuonRoIs";
    const char ThresholdType[] = "MU";
  }

  TrigThresholdDecisionTool::TrigThresholdDecisionTool(const std::string& type, 
						       const std::string& name, 
						       const IInterface* parent)
    : base_class(type, name, parent) {}
  
  StatusCode TrigThresholdDecisionTool::initialize()
  {
    ATH_MSG_DEBUG( "========================================" );
    ATH_MSG_DEBUG( "Initialize for TrigThresholdDecisionTool"  );
    ATH_MSG_DEBUG( "========================================" );

    ATH_CHECK(RoIThresholdsTool::initialize());
    ATH_CHECK( m_rpcTool.retrieve() );
    ATH_CHECK( m_tgcTool.retrieve() );
    return StatusCode::SUCCESS;
  }
  
  StatusCode TrigThresholdDecisionTool::start()
  {
    ATH_MSG_DEBUG( "==========================================" );
    ATH_MSG_DEBUG( "Start for Phase1 TrigThresholdDecisionTool"  );
    ATH_MSG_DEBUG( "==========================================" );


    //front-load the TGC flag parsing and all possible 3-bit decisions for the menu
    SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey);
    ATH_CHECK(l1Menu.isValid());
    std::optional<ThrVecRef> menuThresholds = getMenuThresholds(*l1Menu);
    ATH_CHECK(menuThresholds.has_value());

    for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds.value().get()) {
      std::shared_ptr<TrigConf::L1Threshold_MU> thr = std::static_pointer_cast<TrigConf::L1Threshold_MU>(thrBase);

      //parse the tgc flags and buffer them
      std::string tgcFlags = getShapedFlags( thr->tgcFlags() );
      parseFlags(tgcFlags);

      //loop over all 3-bit flag combinations
      for (unsigned flags=0;flags<8;flags++)
      {
	bool F=flags&0b100;
	bool C=flags&0b010;
	bool H=flags&0b001;
	makeTGCDecision(tgcFlags, F, C, H);
      }

      //parse the rpc flags and buffer them
      std::string rpcFlags = getShapedFlags( thr->rpcFlags() );
      parseFlags(rpcFlags);

      //loop over all 2-bit flag combinations
      for (unsigned flags=0;flags<2;flags++)
      {
	bool M=flags&0b1;
	makeRPCDecision(rpcFlags, M);
      }
    }

    return StatusCode::SUCCESS;
  }
  
  uint64_t TrigThresholdDecisionTool::getPattern(const xAOD::MuonRoI& roi,
                                                 const ThrVec& menuThresholds,
                                                 const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const {
    return getPattern(roi.roiWord(), menuThresholds, menuExtraInfo);
  }

  uint64_t TrigThresholdDecisionTool::getPattern(uint32_t dataWord,
                                                 const ThrVec& menuThresholds,
                                                 const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const {
    uint64_t thresholdsPattern = 0;

    //first figure out if we need to use the RPC or TGC tool for decoding the ROI
    LVL1::ITrigT1MuonRecRoiTool::MuonTriggerSystem system = m_rpcTool->getSystem(dataWord);
    const LVL1::ITrigT1MuonRecRoiTool* roiTool;
    if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel) roiTool = &(*m_rpcTool);
    else roiTool = &(*m_tgcTool);

    //buffer the some information
    unsigned isub = roiTool->getBitMaskValue(&dataWord, roiTool->SubSysIDMask());
    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem side = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem>(isub);
    unsigned ptword = roiTool->getBitMaskValue(&dataWord, roiTool->ThresholdMask());
    unsigned roi, sectorID;
    if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel) {
      roi      = roiTool->getBitMaskValue(&dataWord, roiTool->BarrelRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->BarrelSectorIDMask());
    } else if (system == LVL1::ITrigT1MuonRecRoiTool::Endcap) {
      roi      = roiTool->getBitMaskValue(&dataWord, roiTool->EndcapRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->EndcapSectorIDMask());
    } else { // Forward
      roi      = roiTool->getBitMaskValue(&dataWord, roiTool->ForwardRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->ForwardSectorIDMask());
    }
    const TrigConf::L1ThrExtraInfo_MU& muThrExtraInfo = dynamic_cast<const TrigConf::L1ThrExtraInfo_MU&>(menuExtraInfo);

    //buffer (notional) TGC/RPC flags
    bool F=false, C=false, H=false, M=false;
    if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel)
    {
      M  = dataWord & roiTool->OverflowPerRoIMask();
    }
    else
    {
      F = dataWord & roiTool->BW2Or3Mask();
      C = dataWord & roiTool->InnerCoinMask();
      H = dataWord & roiTool->GoodMFMask();
    }

    //loop over the thresholds
    for (const std::shared_ptr<TrigConf::L1Threshold>& thrBase : menuThresholds) {
      std::shared_ptr<TrigConf::L1Threshold_MU> thr = std::static_pointer_cast<TrigConf::L1Threshold_MU>(thrBase);

      bool passed{false};
      if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel) {
        //skip the threshold with regions not corresponding to ALL or barrel
        if (thr->region().find("ALL") == std::string::npos &&
            thr->region().find("BA") == std::string::npos) continue;

        //veto this candidate from this multiplicity if it's part of the excluded ROI list
        const bool isSideC = (side == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC());
        if (isExcludedRPCROI(muThrExtraInfo, thr->rpcExclROIList(), roi, sectorID, isSideC)) continue;

        if (ptword >= thr->idxBarrel()) {
          // mark this threshold as passed
          passed = true;
        }

        passed &= getRPCDecision(getShapedFlags(thr->rpcFlags()), M);
      }
      else { // Endcap or Forward
        if (system == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem()) { // Endcap
          //skip the threshold with regions not corresponding to ALL or endcap
          if (thr->region().find("ALL") == std::string::npos &&
              thr->region().find("EC") == std::string::npos) continue;

          if (ptword >= thr->idxEndcap()) {
            // mark this threshold as passed
            passed = true;
          }
        }
        else { // Forward
          //skip the threshold with regions not corresponding to ALL or forward
          if (thr->region().find("ALL") == std::string::npos &&
              thr->region().find("FW") == std::string::npos) continue;

          if (ptword >= thr->idxForward()) {
            // mark this threshold as passed
            passed = true;
          }
        }

        passed &= getTGCDecision(getShapedFlags(thr->tgcFlags()), F, C, H);
      } // end Endcap or Forward

      if (passed) {
        // set the corresponding bit in the pattern
        thresholdsPattern |= (1 << thr->mapping());
      }

    } // loop over thresholds

    return thresholdsPattern;
  }

  std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
  TrigThresholdDecisionTool::getThresholdDecisions(uint32_t dataWord,
                                                   const EventContext& eventContext) const {
    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, eventContext);
    std::optional<ThrVecRef> menuThresholds = getMenuThresholds(*l1Menu);
    std::optional<ExtraInfoRef> menuExtraInfo = getMenuThresholdExtraInfo(*l1Menu);
    // Call the other overload
    return getThresholdDecisions(dataWord, menuThresholds.value().get(), menuExtraInfo.value().get());
  }

  std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
  TrigThresholdDecisionTool::getThresholdDecisions(uint32_t dataWord,
                                                   const ThrVec& menuThresholds,
                                                   const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const {

    const uint64_t pattern = getPattern(dataWord, menuThresholds, menuExtraInfo);

    //the object that will be returned: pairs of thresholds and pass/fail decisions
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > threshold_decisions;
    threshold_decisions.resize(menuThresholds.size());
    for (const std::shared_ptr<TrigConf::L1Threshold>& thr : menuThresholds) {
      const bool decision = pattern & (1 << thr->mapping());
      threshold_decisions[thr->mapping()] = std::make_pair(thr, decision);
    }
    return threshold_decisions;
  }

  std::pair<std::string, double> TrigThresholdDecisionTool::getMinThresholdNameAndValue(const std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >& decisions, const double& eta) const
  {
    //find the lowest pt threshold passed
    std::string thrName="";
    double thrVal=0;
    for (unsigned idec=0;idec<decisions.size();++idec) {
      if (!decisions[idec].second) continue;
      std::shared_ptr<TrigConf::L1Threshold> thr = decisions[idec].first;
      if (thr->thrValue(eta) < thrVal)
      {
	thrVal = thr->thrValue(eta);
	thrName = thr->name();
      }
    }
    return std::make_pair(thrName, thrVal);
  }

  bool TrigThresholdDecisionTool::isExcludedRPCROI(const TrigConf::L1ThrExtraInfo_MU& menuExtraInfo,
                                                   const std::string& rpcExclROIList,
                                                   const unsigned roi,
                                                   const unsigned sectorID,
                                                   const bool isSideC) const {
    if (rpcExclROIList != "")
    {
      const std::map<std::string, std::vector<unsigned int> >& exclList = menuExtraInfo.exclusionList(rpcExclROIList);
      if (exclList.size() != 0)
      {
	//build the sector name of this ROI to compare against the exclusion list
	std::stringstream sectorName;
	sectorName<<"B";
	int sectorNumber=sectorID;
	if (isSideC) sectorNumber += 32;
	if (sectorNumber < 10) sectorName << "0";
	sectorName << sectorNumber;
	
	//do the comparison
	auto exclROIs = exclList.find(sectorName.str());
	if (exclROIs != exclList.end())
	{
	  for (auto roi_itr=exclROIs->second.begin();roi_itr!=exclROIs->second.end();roi_itr++)
	  {
	    if (*roi_itr == roi) return true;
	  }
	}
      }
    } // rpcExclList != ""

    return false;
  }

  bool TrigThresholdDecisionTool::getTGCDecision(const std::string& tgcFlags, const bool F, const bool C, const bool H) const
  {
    //check if the word has been checked before for this string of flags (it should always, as we've buffered them in 'start')
    TGCFlagDecision decision(F,C,H);
    auto previous_decisions = m_tgcFlag_decisions.find(tgcFlags);
    if (previous_decisions == m_tgcFlag_decisions.end()) return false;

    auto previous_decision_itr = previous_decisions->second.find(decision);
    if (previous_decision_itr != previous_decisions->second.end()) return previous_decision_itr->pass;
    return false;
  }

  void TrigThresholdDecisionTool::makeTGCDecision(const std::string& tgcFlags, const bool F, const bool C, const bool H)
  {
    //check if the word has been checked before for this string of flags
    TGCFlagDecision decision(F,C,H);
    auto previous_decisions = &m_tgcFlag_decisions[tgcFlags];
    auto previous_decision_itr = previous_decisions->find(decision);
    if (previous_decision_itr != previous_decisions->end()) return;
    else if (tgcFlags == "") {
      decision.pass=true;
      previous_decisions->insert(decision);
    }
    else { // make the decision
      
      //check the quality based on the flags.
      //loop over outer layer of "ors" and 'or' the results
      bool passedFlags = false;
      const std::vector<std::vector<std::string> >* vec_flags = &m_parsed_flags[tgcFlags];
      for (auto or_itr = vec_flags->begin();or_itr!=vec_flags->end();or_itr++)
      {
	//loop over the inner layer of "ands" and 'and' the results
	bool passedAnd = true;
	for (auto and_itr = or_itr->begin();and_itr!=or_itr->end();and_itr++)
	{
	  if (*and_itr == "F") passedAnd = passedAnd && F;
	  else if (*and_itr == "C") passedAnd = passedAnd && C;
	  else if (*and_itr == "H") passedAnd = passedAnd && H;
	}
	passedFlags = passedFlags || passedAnd;
      }
      //buffer the decision
      decision.pass = passedFlags;
      previous_decisions->insert(decision);
    }	  
  }

  bool TrigThresholdDecisionTool::getRPCDecision(const std::string& rpcFlags, const bool M) const
  {
    //check if the word has been checked before for this string of flags (it should always, as we've buffered them in 'start')
    RPCFlagDecision decision(M);
    auto previous_decisions = m_rpcFlag_decisions.find(rpcFlags);
    if (previous_decisions == m_rpcFlag_decisions.end()) return false;

    auto previous_decision_itr = previous_decisions->second.find(decision);
    if (previous_decision_itr != previous_decisions->second.end()) return previous_decision_itr->pass;
    return false;
  }

  void TrigThresholdDecisionTool::makeRPCDecision(const std::string& rpcFlags, const bool M)
  {
    //check if the word has been checked before for this string of flags
    RPCFlagDecision decision(M);
    auto previous_decisions = &m_rpcFlag_decisions[rpcFlags];
    auto previous_decision_itr = previous_decisions->find(decision);
    if (previous_decision_itr != previous_decisions->end()) return;
    else if (rpcFlags == "") {
      decision.pass=true;
      previous_decisions->insert(decision);
    }
    else { // make the decision
      
      //check the quality based on the flags.
      //loop over outer layer of "ors" and 'or' the results
      bool passedFlags = false;
      const std::vector<std::vector<std::string> >* vec_flags = &m_parsed_flags[rpcFlags];
      for (auto or_itr = vec_flags->begin();or_itr!=vec_flags->end();or_itr++)
      {
	//loop over the inner layer of "ands" and 'and' the results
	bool passedAnd = true;
	for (auto and_itr = or_itr->begin();and_itr!=or_itr->end();and_itr++)
	{
	  if (*and_itr == "M") passedAnd = passedAnd && M;
	}
	passedFlags = passedFlags || passedAnd;
      }
      //buffer the decision
      decision.pass = passedFlags;
      previous_decisions->insert(decision);
    }	  
  }

  void TrigThresholdDecisionTool::parseFlags(const std::string& flags)
  {
    //parse the logic of the quality flag into a 2D vector, where outer layer contains the logic |'s and inner layer contains the logical &'s.
    //save the 2D vector in a map so we don't have to parse it each time we want to check the flags.
    if (m_parsed_flags.find(flags) == m_parsed_flags.end())
    {
      std::vector<std::string> vec_ors = parseString(flags, "|");
      std::vector<std::vector<std::string> > vec_flags;
      for (unsigned ior=0;ior<vec_ors.size();ior++)
      {
	vec_flags.push_back(parseString(vec_ors[ior],"&"));
      }
      m_parsed_flags[flags] = vec_flags;
    }
  }

  std::vector<std::string> TrigThresholdDecisionTool::parseString(std::string str, std::string sep) const
  {
    std::vector<std::string> parsed;
    int pos = 0;
    bool first = true;
    if (str.size() == 0) return parsed;
    if (str.find(sep) == std::string::npos)
    {
      parsed.push_back(str);
      return parsed;
    }
    while (true)
    {
      int newPos = str.find(sep, pos);
      if (str.find(sep, pos) == std::string::npos)
      {
	if (!first) parsed.push_back(str.substr(pos, newPos-pos));
	break;
      }
      std::string sub = str.substr(pos, newPos-pos);
      parsed.push_back(sub);
      pos = newPos+1;
      first = false;
    }
    return parsed;
  }
  std::string TrigThresholdDecisionTool::getShapedFlags(const std::string& flags) const
  {
    std::string shapedFlags = flags;
    shapedFlags.erase(std::remove_if(shapedFlags.begin(),shapedFlags.end(),::isspace),shapedFlags.end()); // remove spaces
    std::vector<std::string> vec_ors = parseString(shapedFlags,"|");
    std::set<std::string> set_ors;
    for(const auto& ors : vec_ors){
      std::vector<std::string> vec_ands = parseString(ors,"&");
      std::set<std::string> set_ands;
      for(const auto& ands : vec_ands){
	set_ands.insert(ands);
      }
      std::string aa = "";
      for(const auto& ands : set_ands){
	aa += ands;
	aa += "&";
      }
      std::string bb = aa.substr(0,aa.size()-1); // remove the last "&"
      set_ors.insert(bb);
    }
    std::string aa = "";
    for(const auto& ors : set_ors){
      aa += ors;
      aa += "|";
    }
    std::string bb = aa.substr(0,aa.size()-1); // remove the last "|"
    return bb;
  }
}
