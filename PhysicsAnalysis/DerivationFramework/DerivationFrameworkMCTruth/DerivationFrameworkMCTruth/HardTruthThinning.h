/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardTruthThinning, (c) ATLAS Detector software
// Author: Frank Paige
// Thinning tool to use with CompactHardTruth and truth jets.
// Selects:
//   (1) Stable TruthEvent particles matching HardTruth barcodes.
//   (2) Constituents of truth jets.
//   (3) Selected particles (e.g., B's) and their decays.
//   (4) Stable particles within cone around hard leptons/photons.
// Treatment of Geant particles not yet implemented.
//
// Required parameter  Type                      MCGN2 key
// EventInfo           EventInfo                 "EventInfo"
// TruthParticles      TruthParticleContainer    "TruthParticle"
// TruthVertices       TruthVertexContainer key  "TruthVertex" 
// HardParticles       TruthParticleContainer    "TruthHardParticle"
//
// Parameters that turn on features:
// JetName        If not empty, save constituents with cuts
// KeepIds        If pdgId list not empty, save particles and decay chains
// IsolRadius     If positive, save stable particles in isolation cones
//
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_HARDTRUTHTHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_HARDTRUTHTHINNINGTOOL_H

#include <string>
#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"


namespace DerivationFramework {
    
  class HardTruthThinning : public extends<AthAlgTool, IThinningTool> {
  public:

    HardTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~HardTruthThinning();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode doThinning() const override;

    static int getDescendants(const xAOD::TruthParticle* p,
                       std::vector<const xAOD::TruthParticle*>& d) ;
    static void printxAODTruth(long long evnum,
                        const xAOD::TruthParticleContainer* truths) ;

  private:
    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};

    StringProperty m_streamName
      { this, "StreamName", "", "Name of the stream being thinned" };
    SG::ThinningHandleKey<xAOD::TruthParticleContainer> m_truthParticleName
      { this, "TruthParticles", "", "truth particle container name" };
    SG::ThinningHandleKey<xAOD::TruthVertexContainer> m_truthVertexName
      { this, "TruthVertices", "", "truth vertex container name" };

    std::string m_hardParticleName;

    // TruthJet name and parameters
    std::string m_jetName;
    float m_jetPtCut;
    float m_jetEtaCut;
    float m_jetConstPtCut;
    float m_jetPhotonPtCut;
    float m_isolR;
    float m_isolPtCut;

    // Counters
    mutable std::atomic<int> m_evtCount{};
    int m_maxCount;
    mutable std::atomic<int> m_errCount{};

    // Special particles to keep (with descendants)
    std::vector<int> m_keepIds;

  }; 
} //end namespace

#endif // DERIVATIONFRAMEWORK_HARDTRUTHTHINNING_H
