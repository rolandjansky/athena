///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H
#define THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H 1

/**
 @class ThinNegativeEnergyNeutralPFOsAlg
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IThinningSvc.h"

class ThinNegativeEnergyNeutralPFOsAlg
: public ::AthAlgorithm
{
public:
    
    /// Constructor with parameters:
    ThinNegativeEnergyNeutralPFOsAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor:
    virtual ~ThinNegativeEnergyNeutralPFOsAlg();
    
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
   
    /// Names of the containers to thin
    std::string m_neutralPFOsKey;
 
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nNeutralPFOsProcessed;
    unsigned long m_nNeutralPFOsThinned;

};


#endif //> !THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H
