/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "DisplacedJetBeamspotInfo.h"

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
      const xAOD::TrigComposite* counts;
      xAOD::TrigComposite* info;
      DisplacedJetBeamspotInfo& beamspot;
    };

    StatusCode decide( Info& info ) const;

  private:

    HLT::Identifier m_decisionId;
    Gaudi::Property<float> m_min_trk_pt{this, "min_trk_pt",{0}, "Minimum pt for a track [GeV]"};
    Gaudi::Property<float> m_d0cut{this, "trk_d0cut",{4}, "D0 cut applied to split prompt/displaced"};

    Gaudi::Property<float> m_z0stcut{this, "max_z0st",{3.0}, "Maximum value for z0*sin(theta) [mm]"};
    Gaudi::Property<float> m_d0sigcut{this, "d0sigcut",{25.0}, "Maximum value for z0*sin(theta) [mm]"};


    Gaudi::Property<int> m_maxprompt_c2{this, "max_prompt_trk_c2",{4}, "Max number of prompt tracks in a jet"};
    Gaudi::Property<int> m_mindisp_c2{this, "min_disp_trk_c2",{4}, "Min number of disp tracks in a jet"};
    Gaudi::Property<float> m_nother_frac_c2{this, "nother_frac_c2",{0.5}, "nother frac"};

    Gaudi::Property<int> m_maxprompt_c1{this, "max_prompt_trk_c1",{4}, "Max number of prompt tracks in a jet"};
    Gaudi::Property<int> m_mindisp_c1{this, "min_disp_trk_c1",{4}, "Min number of disp tracks in a jet"};
    Gaudi::Property<float> m_nother_frac_c1{this, "nother_frac_c1",{0.5}, "nother frac"};
    
    Gaudi::Property<std::string> m_cutname{this, "cut_name",{""}, "Name of cuts, used for decoration names"};

    Gaudi::Property<bool> m_usebeamspot{this, "use_beamspot",{true}, "Use beamspot info"};

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
  };

  #endif //> !TRIGT2MINBIAS_TRACKCOUNTHYPOTOOL_H
