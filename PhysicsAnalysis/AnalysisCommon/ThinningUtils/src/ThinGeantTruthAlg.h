///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_ThinGeantTruthAlg_H
#define THINNINGUTILS_ThinGeantTruthAlg_H 1

/**
 @class ThinGeantTruthAlg
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ThinningHandleKey.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#

class ThinGeantTruthAlg
  : public ::AthAlgorithm
{
public:
  
    /// Constructor with parameters:
    ThinGeantTruthAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor:
    virtual ~ThinGeantTruthAlg();
    
    /// Athena algorithm's initalize hook
    virtual StatusCode  initialize();
    
    /// Athena algorithm's execute hook
    virtual StatusCode  execute();
    
    /// Athena algorithm's finalize hook
    virtual StatusCode  finalize();
    
    /// Inline method
    void ancestors(const xAOD::TruthParticle*,
                   std::vector<bool> &,
                   std::unordered_set<int> & );
    void descendants(const xAOD::TruthParticle*,
                     std::vector<bool> &,
                     std::unordered_set<int> & );   
    bool isStatus1BSMParticle(const xAOD::TruthParticle*) const;

 
private:

    /// Should the thinning run?
    bool m_doThinning;
   
    /// Geant barcode
    int m_geantOffset; 
 
    /// Geant-decayed longer lived particles
    std::vector<int> m_longlived;

    StringProperty m_streamName
    { this, "StreamName", "", "Stream for which thinning is to be done." };

    SG::ThinningHandleKey<xAOD::TruthParticleContainer>  m_truthParticlesKey {this, 
        "TruthParticlesKey", 
        "TruthParticles", 
        "Name of the input Truth Particle container"};


    SG::ThinningHandleKey<xAOD::TruthVertexContainer> m_truthVerticesKey{this, 
        "TruthVerticesKey", 
        "TruthVertices", 
        "Name of the input Truth Vertices container"};


    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsKey {this, 
        "ElectronsKey", 
        "Electrons", 
        "Name of the input electron container"};

    SG::ReadHandleKey<xAOD::PhotonContainer> m_photonsKey {this, 
        "PhotonsKey", 
        "Photons", 
        "Name of the input photon container"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonsKey {this, 
        "MuonsKey", 
        "Muons", 
        "Name of the input muon container"};

    SG::ReadHandleKey<xAOD::TruthParticleContainer> m_egammaTruthKey {this, 
        "EGammaTruthKey", 
        "egammaTruthParticles", 
        "Name of the input egammaTruth container"};
    
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nParticlesProcessed;
    unsigned long m_nVerticesProcessed;
    unsigned long m_nParticlesThinned;
    unsigned long m_nVerticesThinned;

};


#endif //> !THINNINGUTILS_ThinGeantTruthAlg_H
