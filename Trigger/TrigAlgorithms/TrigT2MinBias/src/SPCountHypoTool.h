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
  SPCountHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual StatusCode initialize() override;

    struct SPCountsInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigComposite* counts;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    StatusCode decide( SPCountsInfo& decisions ) const;

  private:
    bool applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite* composit) const;
    HLT::Identifier m_decisionId;

    Gaudi::Property<int> m_totNumPixSP{this, "totNumPixSP", -1, "Accept events with SP Count higher than this threshold in pixel"};
    Gaudi::Property<int> m_totNumPixCL_1{this, "totNumPixCL_1", -1, "Accept events with 1 SP Count higher than this threshold in pixel"};
    Gaudi::Property<int> m_totNumPixCL_2{this, "totNumPixCL_2", -1, "Accept events with 2 SP Count higher than this threshold in pixel"};
    Gaudi::Property<int> m_totNumPixCLmin3{this, "totNumPixCLmin3", -1, "Accept eventss with minimum 3 SP Count higher than this threshold in pixel"};
    Gaudi::Property<int> m_pixClBarrel{this, "pixClBarrel", -1, "Accept events with SP Count higher than this threshold in pixel Barrel"};
    Gaudi::Property<int> m_pixClEndcapA{this, "pixClEndcapA", -1, "Accept events with SP Count higher than this threshold in pixel EndcapA"};
    Gaudi::Property<int> m_pixClEndcapC{this, "pixClEndcapC", -1, "Accept events with SP Count higher than this threshold in pixel EndcapC"};
    Gaudi::Property<int> m_totNumSctSP{this, "totNumSctSP", -1, "Accept events with SP Count higher than this threshold in SCT"};
    Gaudi::Property<int> m_SctSpBarrel{this, "SctSpBarrel", -1, "Accept events with SP Count higher than this threshold in SCT Barrel"};
    Gaudi::Property<int> m_SctSpEndcapA{this, "SctSpEndcapA", -1, "Accept events with SP Count higher than this threshold in SCT EndcapA"};
    Gaudi::Property<int> m_SctSpEndcapC{this, "SctSpEndcapC", -1, "Accept events with SP Count higher than this threshold in SCT EndcapC"};
    Gaudi::Property<bool> m_logicAnd{this, "TriggerTypeAnd", true, "And/Or Logic"};

  };

  #endif //> !TRIGT2MINBIAS_SPCOUNTHYPOTOOL_H
