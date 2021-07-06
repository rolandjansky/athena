/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGTHRESHOLDDECISIONTOOL_H
#define ITRIGTHRESHOLDDECISIONTOOL_H

#include "TrigConfData/L1Threshold.h"

#include "GaudiKernel/IAlgTool.h"

namespace LVL1 {

  class ITrigThresholdDecisionTool: public virtual IAlgTool {
  public:
    
    virtual ~ITrigThresholdDecisionTool() {}

    DeclareInterfaceID( ITrigThresholdDecisionTool, 1, 0);

    virtual std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
    getThresholdDecisions(uint32_t dataWord,
                          const EventContext& eventContext) const = 0;
    virtual std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >
    getThresholdDecisions(uint32_t dataWord,
                          const std::vector<std::shared_ptr<TrigConf::L1Threshold>>& menuThresholds,
                          const TrigConf::L1ThrExtraInfoBase& menuExtraInfo) const = 0;
    virtual std::pair<std::string, double> getMinThresholdNameAndValue(const std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >& decisions,
								       const double& eta = 0) const = 0;

  }; // end of ITrigThresholdDecisionTool

} // namespace LVL1

#endif
