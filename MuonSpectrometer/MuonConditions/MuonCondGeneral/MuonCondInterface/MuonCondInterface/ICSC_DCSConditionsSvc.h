/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICSC_DCSCONDITIONSSVC_H
#define ICSC_DCSCONDITIONSSVC_H

#include <algorithm>
#include <string>
#include <vector>

#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IInterface.h"
#include "Identifier/Identifier.h"
#include "MuonCondInterface/ICSCConditionsSvc.h"

class ATLAS_NOT_THREAD_SAFE ICSC_DCSConditionsSvc : virtual public ICSCConditionsSvc {
public:
    virtual ~ICSC_DCSConditionsSvc() = default;

    virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
    static const InterfaceID& interfaceID();

private:
};

inline const InterfaceID& ICSC_DCSConditionsSvc::interfaceID() {
    static const InterfaceID IID_CSC_DCSConditionsSvc("CSC_DCSConditionsSvc", 1, 0);
    return IID_CSC_DCSConditionsSvc;
}
#endif
