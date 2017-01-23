///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
#define THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H 1

/**
 @class ThinNegativeEnergyCaloClustersAlg
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IThinningSvc.h"

class ThinNegativeEnergyCaloClustersAlg
: public ::AthAlgorithm
{
public:
    
    /// Constructor with parameters:
    ThinNegativeEnergyCaloClustersAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    /// Destructor:
    virtual ~ThinNegativeEnergyCaloClustersAlg();
    
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
    std::string m_caloClustersKey;
 
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nCaloClustersProcessed;
    unsigned long m_nCaloClustersThinned;

};


#endif //> !THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
