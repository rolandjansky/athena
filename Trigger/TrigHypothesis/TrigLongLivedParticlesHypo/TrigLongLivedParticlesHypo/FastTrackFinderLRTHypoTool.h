/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
#define TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

class FastTrackFinderLRTHypoTool : virtual public ::AthAlgTool
{
public:

  FastTrackFinderLRTHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual ~FastTrackFinderLRTHypoTool() = default;
    virtual StatusCode initialize() override;

    struct TrkCountsInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigComposite* counts;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    StatusCode decide( TrkCountsInfo& decisions ) const;

  private:

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_min_pt{this, "min_pt",{0}, "Accept events with momentum higher than this limit"};
    Gaudi::Property<float> m_min_d0{this, "min_d0",{0}, "Accept events with d0 above this limit"};
    Gaudi::Property<float> m_max_d0{this, "max_d0",{900}, "Accept events with d0 below this limit"};
    Gaudi::Property<float> m_max_xi2{this, "max_xi2",{10}, "Accept events with Xi2 of the track less than this limit"};
    Gaudi::Property<float> m_required_ntrks{this, "required_ntrks", 1, "Accept events with minimum of this number of tracks"};
    Gaudi::Property<float> m_max_required_ntrks{this, "max_required_ntrks", -1, "Accept events with maximum of this number of tracks"};
    Gaudi::Property<bool> m_acceptAll{this, "acceptAll", false, "Accept all events"};
    Gaudi::Property<bool> m_cutMinAndMax{this, "cutMinAndMax", false, "take events within the given higher and lower limt/cut"};
    Gaudi::Property<bool> m_logicAnd{this, "TriggerTypeAnd", true, "And/Or Logic"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
