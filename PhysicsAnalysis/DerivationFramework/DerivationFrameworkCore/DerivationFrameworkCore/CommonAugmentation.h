/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_COMMONAUGMENTATION_H
#define DERIVATIONFRAMEWORK_COMMONAUGMENTATION_H 1

#include <string>
#include <vector>
#include <list>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

namespace DerivationFramework {
    
    /////////////////////////////////////////////////////////////////////////////
    class CommonAugmentation : public AthAlgorithm {
        
    public:
        CommonAugmentation (const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();
	//StatusCode stop();
    private:
        // Tools etc
        ToolHandleArray<IAugmentationTool> m_augmentationTools;
        ServiceHandle<IChronoStatSvc>      m_chronoSvc;
        //ServiceHandle<StoreGateSvc>	   m_ometaStore;	
	
	// Accumulating variables
	//int m_eventCount;
	//float m_eventWeights;
    };
    
} // end of namespace
#endif 

