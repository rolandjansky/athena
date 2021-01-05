/*
Copyright! (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_SPCOUNTHYPOTOOL_H
#define TRIGT2MINBIAS_SPCOUNTHYPOTOOL_H

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

  Gaudi::Property<int> m_totNumPixSP{this, "totNumPixSP", -1, "Require Clusters  (of any size) Count > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_totNumPixCL_1{this, "totNumPixCL_1", -1, "Require Clusters Count made of clustes of size 1 > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_totNumPixCL_2{this, "totNumPixCL_2", -1, "Require Clusters Count made of clustes of size 2 > threshold (-1 disabled) in whole pixel"};
  Gaudi::Property<int> m_totNumPixCLmin3{this, "totNumPixCLmin3", -1, "Accept eventss with minimum 3 SP Count > threshold (-1 disabled) in pixel"};
  Gaudi::Property<int> m_pixClBarrel{this, "pixClBarrel", -1, "Require Clusters Count > threshold (-1 disabled) in pixel Barrel"};
  Gaudi::Property<int> m_pixClEndcapA{this, "pixClEndcapA", -1, "Require Clusters Count > threshold (-1 disabled) in pixel Endcap A side"};
  Gaudi::Property<int> m_pixClEndcapC{this, "pixClEndcapC", -1, "Require Clusters > threshold (-1 disabled) in pixel Endcap C side"};
  Gaudi::Property<int> m_totNumSctSP{this, "totNumSctSP", -1, "Require SP Count > threshold (-1 disabled) in whole SCT"};
  Gaudi::Property<int> m_SctSpBarrel{this, "SctSpBarrel", -1, "Require SP Count > threshold (-1 disabled) in SCT Barrel"};
  Gaudi::Property<int> m_SctSpEndcapA{this, "SctSpEndcapA", -1, "Require SP Count > threshold (-1 disabled) in SCT Endcap A side"};
  Gaudi::Property<int> m_SctSpEndcapC{this, "SctSpEndcapC", -1, "Require SP Count > threshold (-1 disabled) in SCT Endcap C side"};
  Gaudi::Property<bool> m_logicAnd{this, "TriggerTypeAnd", true, "And/Or Logic"};
};

#endif //> !TRIGT2MINBIAS_SPCOUNTHYPOTOOL_H
