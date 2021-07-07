// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_TRIGTHRESHOLDECISIONTOOL_H
#define TRIGT1MUCTPIPHASE1_TRIGTHRESHOLDECISIONTOOL_H

/*
  Tool to help perform decision about trigger threshold given ROI data
*/

#include "TrigConfData/L1ThrExtraInfo.h"
#include "TrigConfData/L1Threshold.h"
#include "TrigConfData/L1Menu.h"

#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Interfaces/ITrigThresholdDecisionTool.h"

#include "L1Decoder/IRoIThresholdsTool.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace LVL1 {

  namespace MURoIThresholdsToolParams {
    extern const char ContainerName[];
    extern const char ThresholdType[];
    using BaseClass = RoIThresholdsTool<xAOD::MuonRoI, xAOD::MuonRoIContainer, ContainerName, ThresholdType>;
  }

  class TrigThresholdDecisionTool : public extends<MURoIThresholdsToolParams::BaseClass, ITrigThresholdDecisionTool>
  {
  public:

    TrigThresholdDecisionTool(const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent);
    
    virtual StatusCode initialize() override;
    virtual StatusCode start() override;

    virtual uint64_t getPattern(const xAOD::MuonRoI& roi,
                                const ThrVec& menuThresholds,
                                const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;

    virtual uint64_t getPattern(uint32_t dataWord,
                                const ThrVec& menuThresholds,
                                const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const;

    virtual
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
    getThresholdDecisions(uint32_t dataWord,
                          const EventContext& eventContext) const override;
    virtual
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
    getThresholdDecisions(uint32_t dataWord,
                          const ThrVec& menuThresholds,
                          const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const override;

    virtual
    std::pair<std::string, double> getMinThresholdNameAndValue(const std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >& decisions,
							       const double& eta = 0) const override;


  protected:

    struct TGCFlagDecision
    {
      TGCFlagDecision(const bool& F, const bool& C, const bool& H) {word = F + 2*C + 4*H;pass=false;}

      //implement < operator so we can store this type in a set
      bool operator<(const TGCFlagDecision& rhs) const {return word < rhs.word;}

      unsigned word;
      bool pass;
    };

    bool isExcludedRPCROI(const TrigConf::L1ThrExtraInfo_MU& menuExtraInfo,
                          const std::string& rpcExclROIList,
                          unsigned roi,
                          unsigned sectorID,
                          bool isSideC) const;
    bool getTGCDecision(const std::string& tgcFlags, bool F, bool C, bool H) const;
    void makeTGCDecision(const std::string& tgcFlags, bool F, bool C, bool H);
    void parseTGCFlags(const std::string& tgcFlags);
    std::vector<std::string> parseString(std::string str, std::string sep) const;
    std::string getShapedTGCFlags(const std::string& tgcFlags) const;

    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_rpcTool{this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/LVL1__TrigT1RPCRecRoiTool", "Tool to get the eta/phi coordinates in the RPC"};
    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_tgcTool{this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/LVL1__TrigT1TGCRecRoiTool", "Tool to get the eta/phi coordinates in the TGC"};

    //buffered parsed TGC flags
    std::map<std::string, std::vector<std::vector<std::string> > > m_parsed_tgcFlags;

    //buffered set of decisions for words that have been checked for each TGC flag
    std::map<std::string, std::set<TGCFlagDecision> > m_tgcFlag_decisions;
  };

}


#endif
