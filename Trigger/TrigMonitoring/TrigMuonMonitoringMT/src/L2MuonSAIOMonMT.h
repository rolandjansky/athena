/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2MUONSAIOMONMT_H
#define TRIGMUONMONITORINGMT_L2MUONSAIOMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/*
This is a class for monitoring L2MuonSAIO.
 */
class L2MuonSAIOMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2MuonSAIOMonMT(const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize() override;

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;

 private:
  Gaudi::Property<std::vector<float> > m_monitored_chains_plateau {this, "Plateaus", {}, "List of plateaus of measured trigger"};
  std::map<std::string, double> m_plateaus {};

};

#endif //TRIGMUONMONITORINGMT_L2MUONSAIOMONMT_H
