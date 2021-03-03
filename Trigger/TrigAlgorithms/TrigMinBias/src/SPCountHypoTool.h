/*
Copyright! (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_SPCOUNTHYPOTOOL_H
#define TRIGMINBIAS_SPCOUNTHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

class SPCountHypoTool : virtual public ::AthAlgTool
{
public:
  SPCountHypoTool(const std::string &type,
                  const std::string &name,
                  const IInterface *parent);

  virtual StatusCode initialize() override;

  struct SPCountsInfo
  {
    TrigCompositeUtils::Decision *decision;
    const xAOD::TrigComposite *counts;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  StatusCode decide(SPCountsInfo &decisions) const;

private:
  bool applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite *composit) const;
  HLT::Identifier m_decisionId;

  Gaudi::Property<int> m_pixCL{this, "pixCL", -1, "Require Clusters  (of any size) Count > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_pixCL_1{this, "pixCL_1", -1, "Require Clusters Count made of clustes of size 1 > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_pixCL_2{this, "pixCL_2", -1, "Require Clusters Count made of clustes of size 2 > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_pixCLmin3{this, "pixCLmin3", -1, "Accept eventss with minimum 3 SP Count > threshold (-1 disabled) in pixel"};
  Gaudi::Property<int> m_pixCLBarrel{this, "pixCLBarrel", -1, "Require Clusters Count > threshold (-1 disabled) in pixel Barrel"};
  Gaudi::Property<int> m_pixCLEndcapA{this, "pixCLEndcapA", -1, "Require Clusters Count > threshold (-1 disabled) in pixel Endcap A side"};
  Gaudi::Property<int> m_pixCLEndcapC{this, "pixCLEndcapC", -1, "Require Clusters > threshold (-1 disabled) in pixel Endcap C side"};
  Gaudi::Property<int> m_sctSP{this, "sctSP", -1, "Require SP Count > threshold (-1 disabled) in whole SCT"};
  Gaudi::Property<int> m_sctSPBarrel{this, "sctSPBarrel", -1, "Require SP Count > threshold (-1 disabled) in SCT Barrel"};
  Gaudi::Property<int> m_sctSPEndcapA{this, "sctSPEndcapA", -1, "Require SP Count > threshold (-1 disabled) in SCT Endcap A side"};
  Gaudi::Property<int> m_sctSPEndcapC{this, "sctSPEndcapC", -1, "Require SP Count > threshold (-1 disabled) in SCT Endcap C side"};
  Gaudi::Property<bool> m_logicAnd{this, "TriggerTypeAnd", true, "And/Or Logic"};
};

#endif //> !TRIGMINBIAS_SPCOUNTHYPOTOOL_H
