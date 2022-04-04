/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPC_DCSCONDITIONSTOOL_H
#define MUONCONDINTERFACE_IRPC_DCSCONDITIONSTOOL_H

// Includes for Gaudi
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

//**********************************************************
//*
//* Tool to retrieve the RPC DCS Info from COOL DB
//* retrieving of tables from DB virtual interface
//*********************************************************

class IRPC_DCSConditionsTool : virtual public IAlgTool, virtual public IAddressProvider {
public:
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IRPC_DCSConditionsTool("IRPC_DCSConditionsTool", 1, 0);
        return IID_IRPC_DCSConditionsTool;
    };

    virtual std::string PanelOffFolderName() const = 0;
    virtual std::string PanelDeadFolderName() const = 0;

    virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

    virtual StatusCode loadPanelOff(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode loadPanelDead(IOVSVC_CALLBACK_ARGS) = 0;

    virtual const std::vector<Identifier>& deadPanelId() const = 0;
    virtual const std::vector<Identifier>& offPanelId() const = 0;
};

#endif
