/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMDT_DCSCONDITIONSRUN2SVC_H
#define IMDT_DCSCONDITIONSRUN2SVC_H

#include <algorithm>
#include <string>
#include <vector>

#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IInterface.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

class IMDT_DCSConditionsRun2Svc : virtual public IMDTConditionsSvc {
public:
    virtual ~IMDT_DCSConditionsRun2Svc() {}

    // map initialization method
    virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;

    static const InterfaceID& interfaceID();

private:
};

inline const InterfaceID& IMDT_DCSConditionsRun2Svc::interfaceID() {
    static const InterfaceID IID_MDT_DCSConditionsRun2Svc("MDT_DCSConditionsRun2Svc", 1, 0);
    return IID_MDT_DCSConditionsRun2Svc;
}
#endif
