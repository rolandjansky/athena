/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IRPCDETECTORSTATUSDBTOOL_H
#define MUONCONDINTERFACE_IRPCDETECTORSTATUSDBTOOL_H

// Includes for Gaudi
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

static const InterfaceID IID_IRpcDetectorStatusDbTool("IRpcDetectorStatusDbTool", 1, 0);

class IRpcDetectorStatusDbTool : virtual public IAlgTool, virtual public IAddressProvider {
public:
    virtual std::string FolderName() const = 0;
    virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode loadRpcDqStatus(IOVSVC_CALLBACK_ARGS) = 0;
    static const InterfaceID& interfaceID() { return IID_IRpcDetectorStatusDbTool; };

    virtual const std::vector<Identifier>& EffPanelId() const = 0;
    virtual const std::vector<Identifier>& EffStripId() const = 0;

    virtual const std::map<Identifier, double>& RPC_EfficiencyMap() const = 0;
    virtual const std::map<Identifier, double>& RPC_EfficiencyGapMap() const = 0;
    virtual const std::map<Identifier, double>& RPC_MeanClusterSizeMap() const = 0;
    virtual const std::map<Identifier, double>& RPC_FracClusterSize1Map() const = 0;
    virtual const std::map<Identifier, double>& RPC_FracClusterSize2Map() const = 0;
    virtual const std::map<Identifier, double>& RPC_FracClusterSize3Map() const = 0;
    virtual const std::map<Identifier, std::string>& RPC_DeadStripListMap() const = 0;
    virtual const std::map<Identifier, float>& RPC_FracDeadStripMap() const = 0;
    virtual const std::map<Identifier, int>& RPC_ProjectedTracksMap() const = 0;
    virtual const std::map<Identifier, int>& RPC_DeadStripList() const = 0;
    virtual const std::map<Identifier, std::vector<double> >& RPC_TimeMapforStrip() const = 0;
};

#endif
