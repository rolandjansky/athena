/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT2DJTRIG_DISP_HYPOTOOL_H
#define TRIGT2DJTRIG_DISP_HYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODJet/Jet.h"
#include "AthContainers/AuxElement.h"
#include "AthenaMonitoringKernel/Monitored.h"

class DisplacedJetDispHypoTool : virtual public ::AthAlgTool
{
public:

  DisplacedJetDispHypoTool( const std::string& type,
    const std::string& name,
    const IInterface* parent );

    virtual ~DisplacedJetDispHypoTool() = default;
    virtual StatusCode initialize() override;

    struct Info {
      TrigCompositeUtils::Decision* decision;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
      const xAOD::Jet* jet;
      const xAOD::TrackParticleContainer* lrt_tracks;
      const xAOD::Vertex* primary_vertex;
    };

    StatusCode decide( Info& info ) const;

  private:

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_min_trk_pt{this, "min_trk_pt",{0}, "Minimum pt for a track [GeV]"};
    Gaudi::Property<float> m_d0cut{this, "trk_d0cut",{4}, "D0 cut applied to split prompt/displaced"};

    Gaudi::Property<float> m_z0stcut{this, "max_z0st",{3.0}, "Maximum value for z0*sin(theta) [mm]"};
    Gaudi::Property<float> m_d0sigcut{this, "d0sigcut",{25.0}, "Maximum value for z0*sin(theta) [mm]"};


    Gaudi::Property<int> m_maxprompt_h{this, "max_prompt_trk_h",{4}, "Max number of prompt tracks in a jet"};
    Gaudi::Property<int> m_mindisp_h{this, "min_disp_trk_h",{4}, "Min number of disp tracks in a jet"};
    Gaudi::Property<float> m_nother_frac_h{this, "nother_frac_h",{0.5}, "nother frac"};

    Gaudi::Property<int> m_maxprompt_l{this, "max_prompt_trk_l",{4}, "Max number of prompt tracks in a jet"};
    Gaudi::Property<int> m_mindisp_l{this, "min_disp_trk_l",{4}, "Min number of disp tracks in a jet"};
    Gaudi::Property<float> m_nother_frac_l{this, "nother_frac_l",{0.5}, "nother frac"};
    
    Gaudi::Property<std::string> m_cutname{this, "cut_name",{""}, "Name of cuts, used for decoration names"};

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
