/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2DJTRIG_PROMPT_HYPOTOOL_H
#define TRIGT2DJTRIG_PROMPT_HYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODJet/Jet.h"
#include "AthenaMonitoringKernel/Monitored.h"

class DisplacedJetPromptHypoTool : virtual public ::AthAlgTool
{
public:

  DisplacedJetPromptHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual ~DisplacedJetPromptHypoTool() = default;
    virtual StatusCode initialize() override;

    struct Info {
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      std::map<const xAOD::Jet_v1*, TrigCompositeUtils::Decision*> jet_decisions;
      const xAOD::JetContainer* jets;
      std::map<const xAOD::Jet_v1*, std::vector<const xAOD::TrackParticle_v1*>> jets_to_tracks;
      const xAOD::Vertex* primary_vertex;
    };

    StatusCode decide( Info& info ) const;

  private:

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_min_jet_pt{this, "min_jet_pt",{0}, "Minimum jet pt [GeV]"};
    Gaudi::Property<float> m_min_trk_pt{this, "min_trk_pt",{0}, "Minimum pt for a track [GeV]"};
    Gaudi::Property<float> m_d0cut{this, "trk_d0cut",{4}, "D0 cut applied to split prompt/displaced"};
    Gaudi::Property<float> m_z0stcut{this, "max_z0st",{3.0}, "Maximum value for z0*sin(theta) [mm]"};
    Gaudi::Property<float> m_d0sigcut{this, "d0sigcut",{25.0}, "Maximum value for z0*sin(theta) [mm]"};
    Gaudi::Property<int> m_maxprompt{this, "max_prompt_trk",{4}, "Max number of prompt tracks in a jet"};
    Gaudi::Property<unsigned int> m_rankcut{this, "max_jet_rank",{4}, "Maximum jet rank which will be processed"};
    Gaudi::Property<int> m_minnjets{this, "min_njets",{2}, "Minimum number of jets left for it to be worth processing the event"};
    Gaudi::Property<std::string> m_cutname{this, "cut_name",{""}, "Name of cuts, used for decoration names"};

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
