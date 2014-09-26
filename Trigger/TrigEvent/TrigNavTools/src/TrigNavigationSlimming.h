/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVTOOLS_TRIGNAVIGATIONSLIMMING_H
#define TRIGNAVTOOLS_TRIGNAVIGATIONSLIMMING_H


#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"


namespace HLT {

  class TrigNavigationSlimming : public AthAlgorithm {

    public:

      TrigNavigationSlimming(const std::string& name, ISvcLocator *pSvcLocator);
      ~TrigNavigationSlimming() { ; };

      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();
          
    protected:
      ToolHandle<DerivationFramework::IThinningTool> m_tool;
      
  }; // end of class TrigNavigationSlimming

} // end of namespace HLT

#endif
