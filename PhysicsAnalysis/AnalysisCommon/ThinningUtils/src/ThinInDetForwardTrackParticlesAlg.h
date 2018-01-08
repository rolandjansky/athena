///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IThinningSvc.h"

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

    /// Pointer to IThinningSvc
    ServiceHandle<IThinningSvc> m_thinningSvc;
    
    /// Should the thinning run?
    bool m_doThinning;
   
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nTracksProcessed;
    unsigned long m_nTracksThinned;
    unsigned long m_nMuons;
    unsigned long m_nSiFwdMuons;
    unsigned long m_nSiFwdAssoc;
};


#endif //> !THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H
