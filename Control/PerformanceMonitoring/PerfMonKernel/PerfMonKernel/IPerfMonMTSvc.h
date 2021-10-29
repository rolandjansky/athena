/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERMONKERNEL_IPERFMONMTSV_H
#define PERMONKERNEL_IPERFMONMTSV_H

/// STL includes
#include <string>

/// Framework include
#include "GaudiKernel/IService.h"

//class IPerfMonMTSvc : virtual public IMonitorSvc
class IPerfMonMTSvc : virtual public IService
{

  public:

    /// Framework - Service InterfaceID
    static const InterfaceID& interfaceID();

    /// Start Auditing
    virtual void startAud( const std::string& stepName,
                           const std::string& compName = "PerfMonMTSlice" ) = 0;

    /// Stop Auditing
    virtual void stopAud( const std::string& stepName,
                          const std::string& compName = "PerfMonMTSlice" ) = 0;


}; // class IPerfMonMTSvc

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline const InterfaceID& IPerfMonMTSvc::interfaceID()
{
  static const InterfaceID IID_IPerfMonMTSvc("IPerfMonMTSvc", 1, 0);
  return IID_IPerfMonMTSvc;
}

#endif // PERMONKERNEL_IPERFMONMTSV_H
