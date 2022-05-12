/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMDT_DCSCONDITIONSRUN2TOOL_H
#define MUONCONDINTERFACE_IMDT_DCSCONDITIONSRUN2TOOL_H

// Includes for Gaudi
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

//**********************************************************
//*
//* Tool to retrieve the MDT DCS Info from COOL DB
//* retrieving of tables from DB virtual interface for run2
//*********************************************************

class IMDT_DCSConditionsRun2Tool : virtual public IAlgTool, virtual public IAddressProvider {
public:
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMDT_DCSConditionsRun2Tool("IMDT_DCSConditionsRun2Tool", 1, 0);
        return IID_IMDT_DCSConditionsRun2Tool;
    };

    virtual std::string HVFolderName() const = 0;
    virtual std::string LVFolderName() const = 0;

    virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

    virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode loadLV(IOVSVC_CALLBACK_ARGS) = 0;
    virtual const std::vector<std::string>& deadStations() const = 0;
    virtual const std::vector<Identifier>& deadStationsId() const = 0;
    virtual const std::vector<Identifier>& deadMultiLayersId() const = 0;
};

#endif
