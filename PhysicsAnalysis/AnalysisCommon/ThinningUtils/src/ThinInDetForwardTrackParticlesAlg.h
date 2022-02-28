///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H
#define THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H 1

/**
 @class ThinInDetForwardTrackParticlesAlg
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

class ThinInDetForwardTrackParticlesAlg
: public ::AthAlgorithm
{
public:
    
    /// Constructor with parameters:
    ThinInDetForwardTrackParticlesAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor:
    virtual ~ThinInDetForwardTrackParticlesAlg();
    
    /// Athena algorithm's initalize hook
    virtual StatusCode  initialize();
    
    /// Athena algorithm's execute hook
    virtual StatusCode  execute();
    
    /// Athena algorithm's finalize hook
    virtual StatusCode  finalize();
    
private:

    StringProperty m_streamName
    { this, "StreamName", "", "Stream for which to thin" };

    SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_tracksKey
    { this, "TracksKey", "InDetForwardTrackParticles", "Tracks to thin" };

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonsKey
    { this, "MuonsKey", "Muons", "Muons to use for thinning" };
    
    /// Should the thinning run?
    BooleanProperty m_doThinning
    { this, "ThinInDetForwardTrackParticles", true, "Should the InDetForwardTrackParticles thinning be run?" };
   
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nTracksProcessed;
    unsigned long m_nTracksThinned;
    unsigned long m_nMuons = 0UL;
    unsigned long m_nSiFwdMuons = 0UL;
    unsigned long m_nSiFwdAssoc = 0UL;
};


#endif //> !THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H
