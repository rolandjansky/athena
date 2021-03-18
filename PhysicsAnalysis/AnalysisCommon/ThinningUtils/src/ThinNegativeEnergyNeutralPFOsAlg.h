///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ThinningHandleKey.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

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
    StringProperty m_streamName
    { this, "StreamName", "", "Name of the stream for which thinning is being done." };
    
    /// Should the thinning run?
    BooleanProperty m_doThinning
    { this, "ThinNegativeEnergyNeutralPFOs", true, "Should the thinning of negative energy neutral PFOs be run?" };
   
    /// Names of the containers to thin
    SG::ThinningHandleKey<xAOD::PFOContainer> m_neutralPFOsKey
    { this, "NeutralPFOsKey", "JetETMissNeutralParticleFlowObjects", "StoreGate key for the PFOContainer to be thinned (if any)" };
    SG::ThinningHandleKey<xAOD::FlowElementContainer> m_neutralPFOsFEKey
    { this, "NeutralPFOsFEKey", "", "StoreGate key for the FlowElementContainer to be thinned (if any)" };
    SG::ThinningHandleKey<xAOD::FlowElementContainer> m_LCNeutralPFOsFEKey
    { this, "LCNeutralPFOsFEKey", "", "StoreGate key for the LC FlowElementContainer to be thinned (if any)" };
 
    /// Counters
    unsigned long m_nEventsProcessed;
    unsigned long m_nNeutralPFOsProcessed;
    unsigned long m_nNeutralPFOsThinned;

};


#endif //> !THINNINGUTILS_ThinNegativeEnergyNeutralPFOsAlg_H
