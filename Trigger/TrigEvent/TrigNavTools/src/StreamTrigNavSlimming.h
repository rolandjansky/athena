/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVTOOLS_STREAMTRIGNAVIGATIONSLIMMING_H
#define TRIGNAVTOOLS_STREAMTRIGNAVIGATIONSLIMMING_H

#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLT {

  class HLTResult;

  class StreamTrigNavSlimming : public AthAlgorithm {    
    public:
    StreamTrigNavSlimming(const std::string& name, ISvcLocator* pSvcLocator);    
    virtual ~StreamTrigNavSlimming();
    
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

    private:
    ToolHandle<DerivationFramework::IThinningTool> m_tool;


  }; // end StreamTrigNavSlimming class

} // end HLT namespace

#endif // #if HLT_SLIM_NAVIGATION_H
