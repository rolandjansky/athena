/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTALG_H
#define PERFMONCOMPS_PERFMONMTALG_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Forward declaration
class IPerfMonMTSvc;

class PerfMonMTAlg : public AthReentrantAlgorithm {

  public:

    /// Standard Algorithm constructor  
    PerfMonMTAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Standard initialization function
    StatusCode initialize();

    /// Standard execute function 
    StatusCode execute( const EventContext& ) const;

    /// Standard finalization function
    StatusCode finalize();

  private:

    /// Handle to PerfMonMTSvc
    ServiceHandle< IPerfMonMTSvc > m_perfMonMTSvc;

};

#endif // PERFMONCOMPS_PERFMONMTALG_H
