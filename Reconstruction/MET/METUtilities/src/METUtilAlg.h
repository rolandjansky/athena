/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METUtilAlg.h

#ifndef METUtilAlg_H
#define METUtilAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IMETRebuilder;

namespace met {
  class METUtilAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    METUtilAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METUtilAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METUtilAlg();

  private:

    /// Athena configured tools
    ToolHandleArray<IMETRebuilder> m_rebuilders;

  }; 

}

#endif
