/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRIGJETEJSHYPOTOOL_H
#define TRIGJETEJSHYPOTOOL_H
/********************************************************************
 *
 * NAME:     TrigJetEJsHypoTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *  * @class TrigJetEJsHypoTool
 *  * @brief HypoTool for 
 *  * @details HypoTool needed to decide on exotic jet selections
 *  * @author Aaron Kilgallon
 *  * @author Antonio Boveia
 *  * @author Caterina Doglioni
 *  * @author Teng Jian Khoo
 *  * @author Tim Martin
 *  * @author Rafal Bielski
 *          
 *
 ********************************************************************/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/ITrigJetHypoToolHelper.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"


class TrigJetEJsHypoTool: public AthAlgTool{

  public:

    TrigJetEJsHypoTool(const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~TrigJetEJsHypoTool();
    StatusCode initialize();
    StatusCode finalize();

    // typedef for passing jet - decision object assocation
    using JetDecision = std::pair<const xAOD::Jet*,TrigCompositeUtils::Decision*>;

    struct JetInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::Jet_v1* jet;
      const xAOD::TrackParticleContainer* AllTracks;
      const xAOD::Vertex_v1 *PV;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
    };

    StatusCode decide( std::vector<JetInfo>& decisions )  const;

    const HLT::Identifier& getId() const; 

  private:

    // Identifier is used to keep track of which tool made which decision.
    // The information is stored in the event store.
    HLT::Identifier m_decisionId;  

    ToolHandle<ITrigJetHypoToolHelper> m_helper {
      this, "helper_tool", {}, "Jet hypo helper AlgTool"};

    Gaudi::Property<bool>
      m_visitDebug {this, "visit_debug", false, "debug flag"};

    Gaudi::Property<std::string> m_chainName {
    this, "chain_name", {}, "chain name"};

    Gaudi::Property< double > m_ptf    {this, "PTF",       1.0, ""};
    Gaudi::Property< double > m_dr     {this, "dR",        1.0, ""};
    Gaudi::Property< int > m_trackless {this, "Trackless", 1.0, ""};

    // Track eta cuts for exotics and trackless hypos
    double m_trackEtaCut_Exotics        = 2.4;
    double m_trackEtaCut_Trackless      = 2.4;
    // Track pT cuts for exotics hypos
    double m_trackPtCut_Exotics         = 1.0*Gaudi::Units::GeV;
    double m_trackPtCut_Trackless       = 2.0*Gaudi::Units::GeV;
    // Jet pT cut for promptTrackFrac definition (studies only
    // done down to a 50 GeV cut)
    double m_jetPtCut_Exotics           = 50.0*Gaudi::Units::GeV;
    double m_jetPtCut_Trackless         = 50.0*Gaudi::Units::GeV;
    // Eta cut for exotics jet selections (may need to be reduced
    // to 1.4 for large-R jets to have complete overlapy with 
    // the tracker)
    double m_jetEtaCut_Exotics          = 2.0;
    double m_jetEtaCut_Trackless        = 2.0;
    // Track d0 cuts based on a multiple of background distribution
    // 2.5 was found to be best signal sensitivity for R-Hadrons
    double m_PTFSigmaCut                = 2.5;
    // Cut on number of tracks associated to jet in the soft-bomb
    // trigger case (likely to be changed based on physics studies)
    int    m_matchedTracksCut_Trackless = 0;
    // |z0 - PV_z| cut on tracks
    double m_PV_z                       = 10.0;

};
#endif
