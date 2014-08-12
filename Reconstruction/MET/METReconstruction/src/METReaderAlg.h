/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METReaderAlg.h

#ifndef METReaderAlg_H
#define METReaderAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"

namespace met {
  class METReaderAlg : public AthAlgorithm { 

  public: 

    /// Constructor with parameters:
    METReaderAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /// Destructor:
    ~METReaderAlg(); 

    /// Athena algorithm's Hooks
    StatusCode  initialize();
    StatusCode  execute();
    StatusCode  finalize();

  private: 

    /// Default constructor:
    METReaderAlg();

  }; 

}

#endif
