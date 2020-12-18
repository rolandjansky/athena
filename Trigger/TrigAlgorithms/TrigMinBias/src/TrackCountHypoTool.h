/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
#define TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

class TrackCountHypoTool : virtual public ::AthAlgTool
{
public:

  TrackCountHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual ~TrackCountHypoTool() = default;
    virtual StatusCode initialize() override;

    struct TrkCountsInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigComposite* counts;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    };

    StatusCode decide( TrkCountsInfo& decisions ) const;

  private:

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_minPt{this, "minPt",0.1, "Accept events with momentum higher than this limit in MeV"};
    Gaudi::Property<float> m_maxZ0{this, "maxZ0",100, "Accept events with absolute value of vertex lower than this limit in mm"};
    Gaudi::Property<float> m_minNtrks{this, "minNtrks", 1, "Accept events with minimum of this number of tracks, -1 means this cut is disabled"};
    Gaudi::Property<float> m_maxNtrks{this, "maxNtrks", -1, "Accept events with maximum of this number of tracks, -1 means this cut is disabled"};
    Gaudi::Property<bool> m_acceptAll{this, "acceptAll", false, "Accept all events"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
