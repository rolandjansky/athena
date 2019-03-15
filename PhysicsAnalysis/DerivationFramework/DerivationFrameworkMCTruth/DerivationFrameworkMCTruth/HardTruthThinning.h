/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HardTruthThinning, (c) ATLAS Detector software
// Author: Frank Paige
// ThinningSvc tool to use with CompactHardTruth and truth jets.
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
class IThinningSvc;

namespace DerivationFramework {
    
  class HardTruthThinning : public AthAlgTool, public IThinningTool {
  public:

    HardTruthThinning(const std::string& t, const std::string& n, const IInterface* p);
    ~HardTruthThinning();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode doThinning() const;
    int getDescendants(const xAOD::TruthParticle* p,
                       std::vector<const xAOD::TruthParticle*>& d) const;
    void printxAODTruth(long long evnum,
                        const xAOD::TruthParticleContainer* truths) const;

  private:

    // handle to the thinning service
    ServiceHandle<IThinningSvc> m_thinningSvc;

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
    // TruthParticle container names
    std::string m_truthParticleName;
    std::string m_truthVertexName;
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
    mutable int m_evtCount;
    int m_maxCount;
    mutable int m_errCount;

    // Special particles to keep (with descendants)
    std::vector<int> m_keepIds;

  }; 
} //end namespace

#endif // DERIVATIONFRAMEWORK_HARDTRUTHTHINNING_H
