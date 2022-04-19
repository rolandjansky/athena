/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_ICSC_DCSCONDITIONSTOOL_H
#define MUONCONDINTERFACE_ICSC_DCSCONDITIONSTOOL_H

// Includes for Gaudi
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

//**********************************************************
//* Author Monica Verducci monica.verducci@cern.ch
//*
//* Tool to retrieve the CSC DCS Info from COOL DB
//* retrieving of tables from DB virtual interface
//*********************************************************

class ICSC_DCSConditionsTool : virtual public IAlgTool, virtual public IAddressProvider {
public:
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_ICSC_DCSConditionsTool("ICSC_DCSConditionsTool", 1, 0);
        return IID_ICSC_DCSConditionsTool;
    };

    virtual std::string HVFolderName() const = 0;
    virtual std::string ChamberFolder() const = 0;

    virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;

    virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode loadchamber(IOVSVC_CALLBACK_ARGS) = 0;
    virtual const std::vector<Identifier>& deadStationsId() const = 0;
    virtual const std::vector<Identifier>& deadWireLayersId() const = 0;

    virtual const std::vector<std::string>& deadStations() const = 0;
    virtual const std::vector<std::string>& deadWireLayers() const = 0;
};

#endif
