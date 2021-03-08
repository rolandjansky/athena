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

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h"

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace LVL1 {

  class TrigThresholdDecisionTool : public extends<AthAlgTool, ITrigThresholdDecisionTool>
  {
  public:

    TrigThresholdDecisionTool(const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent);
    
    virtual ~TrigThresholdDecisionTool();
    
    virtual StatusCode initialize() override;
    virtual StatusCode start() override;

    virtual
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > getThresholdDecisions(const unsigned& dataWord) const override;
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

    bool isExcludedRPCROI(const std::string& rpcExclROIList, const unsigned& roi, const unsigned& sectorID, const bool& isSideC) const;
    bool getTGCDecision(const std::string& tgcFlags, const bool& F, const bool& C, const bool& H) const;
    void makeTGCDecision(const std::string& tgcFlags, const bool& F, const bool& C, const bool& H);
    void parseTGCFlags(const std::string& tgcFlags);
    std::vector<std::string> parseString(std::string str, std::string sep) const;

    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_rpcTool{this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/LVL1__TrigT1RPCRecRoiTool", "Tool to get the eta/phi coordinates in the RPC"};
    ToolHandle<LVL1::ITrigT1MuonRecRoiTool> m_tgcTool{this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/LVL1__TrigT1TGCRecRoiTool", "Tool to get the eta/phi coordinates in the TGC"};
    ServiceHandle<StoreGateSvc> m_detStore { this, "DetectorStore", "StoreGateSvc/DetectorStore", "Detector store to get the menu" };

    const TrigConf::L1Menu* m_l1menu;

    //buffered parsed TGC flags
    std::map<std::string, std::vector<std::vector<std::string> > > m_parsed_tgcFlags;

    //buffered set of decisions for words that have been checked for each TGC flag
    std::map<std::string, std::set<TGCFlagDecision> > m_tgcFlag_decisions;
  };

}


#endif
