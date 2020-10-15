/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H
#define TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H

#include "TrigMuonMonitorAlgorithm.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/*
This is a class for monitoring L2MuonSA.
 */
class L2OverlapRemoverMonMT : public TrigMuonMonitorAlgorithm{

 public:
  L2OverlapRemoverMonMT(const std::string& name, ISvcLocator* pSvcLocator );

 protected:
  virtual StatusCode fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const override;

 private:

  bool isOverlap(const std::string &chain, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL1, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL2,
                 std::tuple<float,float,float> (*trigPosForMatchFunc)(const xAOD::L2StandAloneMuon*)) const;
  StatusCode chooseBestMuon(const std::string &chain, std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont, std::vector<unsigned int> muResult) const;
  float calcinvMass(double m1, double pt1, double eta1, double phi1, double m2, double pt2, double eta2, double phi2) const;
  static inline std::tuple<float,float,float> L2SAPosForMatchFunc(const xAOD::L2StandAloneMuon *trig);

  // these threshold is same as the L2OverlapRemover in TrigMufastHypoTool
  Gaudi::Property< float > m_dRThresBB {this, "DRThresBB", 0.05, "DR threshold in barel and barel region"};
  Gaudi::Property< float > m_massThresBB {this, "MassThresBB", 0.20, "mass threshold in barel and barel region"};
  Gaudi::Property< float > m_dRThresBE {this, "DRThresBE", 0.05, "DR threshold in barel and barel region"};
  Gaudi::Property< float > m_massThresBE {this, "MassThresBE", 0.20, "mass threshold in barel and endcap region"};
  Gaudi::Property< std::vector<float> > m_etaBinsEC {this, "EtaBinsEC", {0, 1.9, 2.1, 9.9}, "eta bins of DR and mass thresholds in endcap and barel region"};
  Gaudi::Property< std::vector<float> > m_dRThresEC {this, "DRThresEC", {0.06, 0.05, 0.05}, "DR threshold in endcap and barel region"};
  Gaudi::Property< std::vector<float> > m_massThresEC {this, "MassThresEC", {0.20, 0.15, 0.10}, "mass threshold in endcap and endcap region"};

};

#endif //TRIGMUONMONITORINGMT_L2OVERLAPREMOVERMONMT_H
