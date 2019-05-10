/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

#include "AthenaBaseComps/AthService.h"

#include "PerfMonKernel/IPerfMonMTSvc.h"


class PerfMonMTSvc : virtual public IPerfMonMTSvc, 
                     public AthService
{

  public:

    /// Standard Gaudi Service constructor  
    PerfMonMTSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /// Function declaring the interface(s) implemented by the service
    virtual StatusCode queryInterface( const InterfaceID& riid, 
                                       void** ppvInterface ) override;

    /// Standard Gaudi Service initialization 
    virtual StatusCode initialize() override;

    /// Standard Gaudi Service finalization
    virtual StatusCode finalize() override;

    /// Start Auditing 
    virtual void startAud( const std::string& stepName, 
                           const std::string& compName ) override;

    /// Stop Auditing   
    virtual void stopAud ( const std::string& stepName, 
                           const std::string& compName ) override;

}; // class PerfMonMTSvc

#endif // PERFMONCOMPS_PERFMONMTSVC_H
