/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
#define TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H

#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"

class TrackCountHypoTool : virtual public ::AthAlgTool
{
public:

  TrackCountHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual ~TrackCountHypoTool();
    virtual StatusCode initialize() override;

    struct TrkCountsInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigComposite* counts;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    StatusCode decide( TrkCountsInfo& decisions ) const;

  private:
    bool applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite* composit) const;

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_min_pt{this, "min_pt",0.1, "Accept events with momentum higher than this limit"};
    Gaudi::Property<float> m_required_ntrks{this, "required_ntrks", 1, "Accept events with minimum of this number of tracks"};
    Gaudi::Property<float> m_max_required_ntrks{this, "max_required_ntrks", -1, "Accept events with maximum of this number of tracks"};
    Gaudi::Property<bool> m_acceptAll{this, "acceptAll", false, "Accept all events"};
    Gaudi::Property<bool> m_cutMinAndMax{this, "cutMinAndMax", false, "take events within the given higher and lower limt/cut"};
    Gaudi::Property<bool> m_logicAnd{this, "TriggerTypeAnd", true, "And/Or Logic"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
