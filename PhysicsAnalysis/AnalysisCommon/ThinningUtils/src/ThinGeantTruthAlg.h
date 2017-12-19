///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTruth/TruthParticleContainer.h"

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

    /// Pointer to IThinningSvc
    ServiceHandle<IThinningSvc> m_thinningSvc;
    
    /// Should the thinning run?
    bool m_doThinning;
   
    /// Geant barcode
    int m_geantOffset; 
 
    /// Geant-decayed longer lived particles
    std::vector<int> m_longlived;

    /// Names of the containers to thin
    std::string m_truthParticlesKey;
    std::string m_truthVerticesKey;
    std::string m_muonsKey;
    std::string m_electronsKey;
    std::string m_photonsKey;   
    std::string m_egammaTruthKey;
 
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nParticlesProcessed;
    unsigned long m_nVerticesProcessed;
    unsigned long m_nParticlesThinned;
    unsigned long m_nVerticesThinned;

};


#endif //> !THINNINGUTILS_ThinGeantTruthAlg_H
