///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
#define THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H 1

/**
 @class ThinNegativeEnergyCaloClustersAlg
*/


// STL includes
#include <string>

#include "xAODCaloEvent/CaloClusterContainer.h"

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ThinningHandleKey.h"

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
    StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream for which thinning is being done." };
    
    /// Should the thinning run?
    BooleanProperty m_doThinning
    { this, "ThinNegativeEnergyCaloClusters", true, "Should the thinning of negative energy calo clusters be run?" };
   
    /// Names of the containers to thin
    SG::ThinningHandleKey<xAOD::CaloClusterContainer> m_caloClustersKey
    { this, "CaloClustersKey", "CaloCalTopoClusters", "StoreGate key for the CaloClustersContainer to be thinned" };
 
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nCaloClustersProcessed;
    unsigned long m_nCaloClustersThinned;

};


#endif //> !THINNINGUTILS_ThinNegativeEnergyCaloClustersAlg_H
