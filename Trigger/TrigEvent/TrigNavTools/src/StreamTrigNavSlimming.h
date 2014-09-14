/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLT_SLIM_NAVIGATION_H
#define HLT_SLIM_NAVIGATION_H

#include "AthenaKernel/IThinningSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include "TrigNavigationSlimming.h"

class StoreGateSvc;

namespace HLT {

  class HLTResult;

  class StreamTrigNavSlimming : public AthAlgorithm {

    public:

    StreamTrigNavSlimming(const std::string& name, ISvcLocator* pSvcLocator);
    
    virtual ~StreamTrigNavSlimming();
    
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();
    //    virtual StatusCode slimNavigation();

    private:
    ServiceHandle<IThinningSvc> m_thinningSvc;
    ToolHandle<HLT::TrigNavigationSlimmingTool> m_slimmingTool;
    std::string m_resultKey;

  }; // end StreamTrigNavSlimming class

} // end HLT namespace

#endif // #if HLT_SLIM_NAVIGATION_H
