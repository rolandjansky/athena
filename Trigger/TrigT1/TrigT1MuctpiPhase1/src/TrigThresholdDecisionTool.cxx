/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

#include "TrigT1MuctpiPhase1/TrigThresholdDecisionTool.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

namespace LVL1
{
  TrigThresholdDecisionTool::TrigThresholdDecisionTool(const std::string& type, 
						       const std::string& name, 
						       const IInterface* parent)
    :
    base_class(type, name, parent),
    m_l1menu(nullptr)
  {
  }
  
  TrigThresholdDecisionTool::~TrigThresholdDecisionTool()
  {
    
  }
  
  StatusCode TrigThresholdDecisionTool::initialize()
  {
    return StatusCode::SUCCESS;
  }
  
  StatusCode TrigThresholdDecisionTool::start()
  {
    CHECK( m_rpcTool.retrieve() );
    CHECK( m_tgcTool.retrieve() );
    ATH_CHECK( m_detStore->retrieve(m_l1menu) );     

    //front-load the TGC flag parsing and all possible 3-bit decisions for the menu
    const std::vector<std::shared_ptr<TrigConf::L1Threshold>>* thresholds = &m_l1menu->thresholds("MU");
    for (auto itr=thresholds->begin();itr!=thresholds->end();++itr)
    {
      std::shared_ptr<TrigConf::L1Threshold_MU> thr = std::static_pointer_cast<TrigConf::L1Threshold_MU>(*itr);
      std::string tgcFlags = thr->tgcFlags();

      //parse the tgc flags and buffer them
      parseTGCFlags(tgcFlags);


      //loop over all 3-bit flag combinations
      for (unsigned flags=0;flags<8;flags++)
      {
	bool F=flags&0b100;
	bool C=flags&0b010;
	bool H=flags&0b001;
	makeTGCDecision(tgcFlags, F, C, H);

      }
      
    }

    return StatusCode::SUCCESS;
  }
  
  std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > TrigThresholdDecisionTool::getThresholdDecisions(const unsigned& dataWord) const
  {
    //first figure out if we need to use the RPC or TGC tool for decoding the ROI
    LVL1::ITrigT1MuonRecRoiTool::MuonTriggerSystem system = m_rpcTool->getSystem(dataWord);
    const LVL1::ITrigT1MuonRecRoiTool* roiTool;
    if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel) roiTool = &(*m_rpcTool);
    else roiTool = &(*m_tgcTool);
    
    
    //the object that will be returned: pairs of thresholds and pass/fail decisions
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > threshold_decisions;
    
    //buffer the some information
    unsigned isub = roiTool->getBitMaskValue(&dataWord, roiTool->SubSysIDMask());
    LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem side = static_cast<LVL1MUONIF::Lvl1MuCTPIInputPhase1::MuonSubSystem>(isub);
    unsigned pt_threshold = roiTool->getBitMaskValue(&dataWord, roiTool->ThresholdMask());
    unsigned roi, sectorID;
    if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel) {
      roi = roiTool->getBitMaskValue(&dataWord, roiTool->BarrelRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->BarrelSectorIDMask());
    } else if (system == LVL1::ITrigT1MuonRecRoiTool::Endcap) {
      roi = roiTool->getBitMaskValue(&dataWord, roiTool->EndcapRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->ForwardSectorIDMask());
    } else { // Forward
      roi = roiTool->getBitMaskValue(&dataWord, roiTool->ForwardRoIMask());
      sectorID = roiTool->getBitMaskValue(&dataWord, roiTool->ForwardSectorIDMask());
    }

    //buffer (notional) TGC flags
    bool F=false, C=false, H=false;
    if (system != LVL1::ITrigT1MuonRecRoiTool::Barrel)
    {
      F = dataWord & roiTool->BW2Or3Mask();
      C = dataWord & roiTool->InnerCoinMask();
      H = dataWord & roiTool->GoodMFMask();
    }

    
    //loop over the thresholds
    const std::vector<std::shared_ptr<TrigConf::L1Threshold> >* thresholds = &m_l1menu->thresholds("MU");
    int nThresholds = thresholds->size();
    for (int ithresh=0;ithresh<nThresholds;ithresh++)
    {
      std::shared_ptr<TrigConf::L1Threshold_MU> thr = std::static_pointer_cast<TrigConf::L1Threshold_MU>((*thresholds)[ithresh]);
      threshold_decisions.push_back(std::make_pair(thr, false));
 
      bool passed=false;      
      if (system == LVL1::ITrigT1MuonRecRoiTool::Barrel)
      {
	//skip the threshold with regions not corresponding to ALL or barrel
	if (thr->region().find("ALL") == std::string::npos &&
	    thr->region().find("BA") == std::string::npos) continue;
	
	//veto this candidate from this multiplicity if it's part of the excluded ROI list
	if (isExcludedRPCROI(thr->rpcExclROIList(), roi, sectorID, side == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC())) continue;

	if (thr->idxBarrel()+1 >= pt_threshold) passed=true;
      }
      else // Endcap or Forward
      {
	if (system == LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem()) // Endcap
	{
	//skip the threshold with regions not corresponding to ALL or endcap
	  if (thr->region().find("ALL") == std::string::npos &&
	      thr->region().find("EC") == std::string::npos) continue;
	  if (thr->idxEndcap()+1 >= pt_threshold) passed=true;
	}
	else // Forward
	{
	//skip the threshold with regions not corresponding to ALL or forward
	  if (thr->region().find("ALL") == std::string::npos &&
	      thr->region().find("FW") == std::string::npos) continue;
	  if (thr->idxForward()+1 >= pt_threshold) passed=true;
	}
	
	bool pass = getTGCDecision(thr->tgcFlags(), F, C, H);
	if (!pass) continue;
      } // end Endcap or Forward
      
      threshold_decisions[ithresh].second = passed;
    } // N Thresholds
    
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

  bool TrigThresholdDecisionTool::isExcludedRPCROI(const std::string& rpcExclROIList, const unsigned& roi, const unsigned& sectorID, const bool& isSideC) const
  {
    if (rpcExclROIList != "")
    {
      const std::map<std::string, std::vector<unsigned int> >* exclList = &m_l1menu->thrExtraInfo().MU().exclusionList(rpcExclROIList);
      if (exclList->size() != 0)
      {
	//build the sector name of this ROI to compare against the exclusion list
	std::stringstream sectorName;
	sectorName<<"B";
	int sectorNumber=sectorID;
	if (isSideC) sectorNumber += 32;
	if (sectorNumber < 10) sectorName << "0";
	sectorName << sectorNumber;
	
	//do the comparison
	auto exclROIs = exclList->find(sectorName.str());
	if (exclROIs != exclList->end())
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

  bool TrigThresholdDecisionTool::getTGCDecision(const std::string& tgcFlags, const bool& F, const bool& C, const bool& H) const
  {
    //check if the word has been checked before for this string of flags (it should always, as we've buffered them in 'start')
    TGCFlagDecision decision(F,C,H);
    auto previous_decisions = m_tgcFlag_decisions.find(tgcFlags);
    if (previous_decisions == m_tgcFlag_decisions.end()) return false;
    auto previous_decision_itr = previous_decisions->second.find(decision);
    if (previous_decision_itr != previous_decisions->second.end()) return previous_decision_itr->pass;
    return false;
  }

  void TrigThresholdDecisionTool::makeTGCDecision(const std::string& tgcFlags, const bool& F, const bool& C, const bool& H)
  {
    //check if the word has been checked before for this string of flags
    TGCFlagDecision decision(F,C,H);
    auto previous_decisions = &m_tgcFlag_decisions[tgcFlags];
    auto previous_decision_itr = previous_decisions->find(decision);
    if (previous_decision_itr != previous_decisions->end()) return;
    else { // make the decision
      
      //check the quality based on the flags.
      //loop over outer layer of "ors" and 'or' the results
      bool passedFlags = false;
      const std::vector<std::vector<std::string> >* vec_flags = &m_parsed_tgcFlags[tgcFlags];
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

  void TrigThresholdDecisionTool::parseTGCFlags(const std::string& tgcFlags)
  {
    //parse the logic of the quality flag into a 2D vector, where outer layer contains the logic |'s and inner layer contains the logical &'s.
    //save the 2D vector in a map so we don't have to parse it each time we want to check the flags.
    if (m_parsed_tgcFlags.find(tgcFlags) == m_parsed_tgcFlags.end())
    {
      std::vector<std::string> vec_ors = parseString(tgcFlags, "|");
      std::vector<std::vector<std::string> > vec_flags;
      for (unsigned ior=0;ior<vec_ors.size();ior++)
      {
	vec_flags.push_back(parseString(vec_ors[ior],"&"));
      }
      m_parsed_tgcFlags[tgcFlags] = vec_flags;
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
}
