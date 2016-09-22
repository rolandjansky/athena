/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_DERIVATIONKERNEL_H
#define DERIVATIONFRAMEWORK_DERIVATIONKERNEL_H 1

#include <string>
#include <vector>
#include <list>

#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

namespace DerivationFramework {
    
    /////////////////////////////////////////////////////////////////////////////
    class DerivationKernel : public AthFilterAlgorithm {
        
    public:
        DerivationKernel (const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();
    private:
        // Tools etc
        ToolHandleArray<ISkimmingTool>     m_skimmingTools;
	ToolHandleArray<IThinningTool>     m_thinningTools;
        ToolHandleArray<IAugmentationTool> m_augmentationTools;
        ServiceHandle<IChronoStatSvc>      m_chronoSvc;

        // Some counters
        int m_eventCounter;
        int m_acceptCntr;        
	bool m_runSkimmingFirst;        
    };
    
} // end of namespace
#endif 

