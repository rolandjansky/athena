/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATLFAST_FSSTOREGATECLEAN_H
#define ATLFAST_FSSTOREGATECLEAN_H

#include <string>
#include <vector>

// Gaudi
//#ifndef GAUDIKERNEL_ALGORITHM_H
//#include "GaudiKernel/Algorithm.h"
//#define GAUDIKERNEL_ALGORITHM_H
//#endif

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

class ISvcLocator;

namespace FastCaloSim {

  class FSStoregateClean : public AthAlgorithm {

  public:
    /** Standard Athena-Algorithm Constructor */
    FSStoregateClean(const std::string& name, ISvcLocator* pSvcLocator);
    ~FSStoregateClean();
    
    StatusCode          initialize();
    StatusCode          execute();
    StatusCode          finalize();

  private:
    std::vector< std::string > m_SG_keys;
  };

} // end of namespace bracket
#endif


















