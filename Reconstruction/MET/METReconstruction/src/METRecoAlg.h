/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRecoAlg.h

#ifndef METRecoAlg_H
#define METRecoAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMETRecoTool;

namespace met {
  class METRecoAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    METRecoAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METRecoAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METRecoAlg();

  private:

    /// Athena configured tools
    ToolHandleArray<IMETRecoTool> m_recotools;

  }; 

}

#endif
