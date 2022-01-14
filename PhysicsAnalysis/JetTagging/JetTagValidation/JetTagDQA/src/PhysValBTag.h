///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// PhysValBTag.h
// Header file for class PhysValBTag
// Author: E.Schopf<elisabeth.schopf@cern.ch>
// Updates: J.Hoefer <judith.hoefer@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef JETTAGDQA_PHYSVALBTag_H
#define JETTAGDQA_PHYSVALBTag_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrkValHistUtils/PlotBase.h"
#include "BTaggingValidationPlots.h"

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenter.h"

// Root includes
#include "TH1.h"

// Forward declaration

namespace JetTagDQA {

  class PhysValBTag
    : public ManagedMonitorToolBase
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:
    double m_PV_x = -999.;
    double m_PV_y = -999.;
    double m_PV_z = -999.;
    // Copy constructor:

    /// Constructor with parameters:
    PhysValBTag( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

    /// Destructor:
    virtual ~PhysValBTag();

    // Athena algtool's Hooks
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();


    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    std::map<const xAOD::TrackParticle*, int> getTrackTruthAssociations(const xAOD::BTagging* btag) const;

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:

    /// Default constructor:
    PhysValBTag();

    ToolHandle<InDet::IInDetTrackTruthOriginTool> m_trackTruthOriginTool{this, "trackTruthOriginTool", "InDet::InDetTrackTruthOriginTool"};
    FlavorTagDiscriminants::BTagMuonAugmenter m_muonAugmenter;

    // isData flag
    bool m_isData;
    bool m_doTrackTruth = true;

    // Containers
    std::string m_jetName1;
    std::string m_jetName2;
    std::string m_jetName3;

    std::string m_trackName;
    std::string m_vertexName;

    std::map<std::string, JetTagDQA::BTaggingValidationPlots> m_btagplots;
    
    // histogram definitions
    // the first one is a vector because I can only pass vectors from the joboptions to the algs (and no maps)
    Gaudi::Property< std::vector< std::vector< std::string > > > m_HistogramDefinitionsVector {this, "HistogramDefinitions", {}, "Map with histogram definitions"};
    // have a useful map nevertheless
    std::map< std::string, std::vector< std::string > > m_HistogramDefinitionsMap;

    float m_jetPtCut = -1;
    bool m_onZprime = false;
    float m_jetPtCutTtbar;
    float m_jetPtCutZprime;
    float m_JVTCutAntiKt4EMTopoJets;
    float m_JVTCutLargerEtaAntiKt4EMTopoJets;
    float m_JVTCutAntiKt4EMPFlowJets;
    float m_truthMatchProbabilityCut;

    JetTagDQA::BTaggingValidationPlots m_antiKt4EMTopoPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt4EMPFlowJetsPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKtVR30Rmax4Rmin02PV0TrackJetsPlots;

    int m_nevents;

    StatusCode book(PlotBase& plots);
  };

  // I/O operators
  //////////////////////

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
}

#endif //> !JETTAGDQA_PHYSVALBTag_H
