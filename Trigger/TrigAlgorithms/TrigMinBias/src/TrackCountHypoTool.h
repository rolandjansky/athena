/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGMINBIAS_TRACKCOUNTHYPOTOOL_H
#define TRIGMINBIAS_TRACKCOUNTHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthCheckedComponent.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

class TrackCountHypoTool : virtual public AthCheckedComponent<::AthAlgTool>
{
public:

  TrackCountHypoTool(const std::string& type,
    const std::string& name,
    const IInterface* parent);

  struct TrkCountsInfo {
    TrigCompositeUtils::Decision* decision;
    const xAOD::TrigComposite* counts;
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
  };

  StatusCode decide(TrkCountsInfo& decisions) const;

private:

  HLT::Identifier m_decisionId;
  Gaudi::Property<float> m_minPt{ this, "minPt", 100, "Accept events with momentum higher than this limit in MeV" };
  Gaudi::Property<float> m_maxZ0{ this, "maxZ0", 401, "Accept events with absolute value of track z at beamline position than this limit in mm" };
  Gaudi::Property<float> m_maxVertexZ{ this, "maxVertexZ", 803, "Accept events with absolute value of track to vertex z distance than this limit in mm" };
  Gaudi::Property<float> m_minNtrks{ this, "minNtrks", 1, "Accept events with minimum (including that value) of this number of tracks, -1 means this cut is disabled" };
  Gaudi::Property<float> m_maxNtrks{ this, "maxNtrks", -1, "Accept events with maximum (excluding that value) of this number of tracks, -1 means this cut is disabled" };
  const int m_exclusivityThreshold = 15;
  Gaudi::Property<bool> m_exclusive{ this, "exclusive", false, "Apply exclusivity selection (less than Exclusivity cut low pt tracks & tracks within the range)" };
  Gaudi::Property<bool> m_acceptAll{ this, "acceptAll", false, "Accept all events" };
};

#endif //> !TRIGMINBIAS_TRACKCOUNTHYPOTOOL_H
