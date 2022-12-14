/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MDT_DCSCONDITIONSRUN2TOOL_H
#define MUONCONDTOOL_MDT_DCSCONDITIONSRUN2TOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCondInterface/IMDT_DCSConditionsRun2Tool.h"
#include "MuonCondInterface/IMDT_MapConversion.h"
#include "MuonIdHelpers/MdtIdHelper.h"

class IIOVSvc;
class IMDT_MapConversion;

class MDT_DCSConditionsRun2Tool : public AthAlgTool, virtual public IMDT_DCSConditionsRun2Tool {
public:
    MDT_DCSConditionsRun2Tool(const std::string& type, const std::string& name, const IInterface* parent);

    /** required by the IAddressProvider interface */
    // virtual StatusCode updateAddress(SG::TransientAddress* tad);
    virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad, const EventContext& ctx) override;

    virtual StatusCode initialize() override;

    virtual std::string HVFolderName() const override { return m_hvFolder; }
    virtual std::string LVFolderName() const override { return m_lvFolder; }
    //  virtual std::string JTAGFolderName() const {return m_jtagFolder;}

    virtual const std::vector<std::string>& deadStations() const override { return m_cachedDeadStations; }
    virtual const std::vector<Identifier>& deadStationsId() const override { return m_cachedDeadStationsId; }
    virtual const std::vector<Identifier>& deadMultiLayersId() const override { return m_cachedDeadMultiLayersId; }

    const std::string OnlineName(Identifier OfflineId);
    const Identifier OfflineName(std::string OnlineId);

private:
    virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;

    virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) override;
    virtual StatusCode loadLV(IOVSVC_CALLBACK_ARGS) override;
    // virtual StatusCode loadJTAG(IOVSVC_CALLBACK_ARGS);

    IIOVSvc* m_IOVSvc;

    std::map<std::string, Identifier> m_Chamber_Map;

    std::map<int, Identifier> m_Chamber_Naming_standby;

    std::map<Identifier, float> m_ChamberML_V0;
    std::map<Identifier, float> m_ChamberML_V1;

    std::map<int, float> m_ChamberML_V0_chanum;
    std::map<int, float> m_ChamberML_V1_chanum;
    const MdtIdHelper* m_mdtIdHelper;

    std::string m_OnlineName;
    Identifier m_OfflineName;

    std::vector<std::string> m_cachedDropStations;
    std::vector<Identifier> m_cachedDropStationsId;

    std::vector<std::string> m_cachedDeadStations;
    std::vector<Identifier> m_cachedDeadStationsId;

    std::vector<std::string> m_cachedDeadMultiLayers;
    std::vector<Identifier> m_cachedDeadMultiLayersId;
    std::vector<Identifier> m_cachedDeadMultiLayersId_standby;

    std::vector<std::string> m_cachedDeadLVStations;
    std::vector<Identifier> m_cachedDeadLVStationsId;

    std::vector<std::string> m_cachedDeadJTAGStatus;
    std::vector<Identifier> m_cachedDeadJTAGStatusId;

    // private comparison function for Identifier sorting
    static bool compareId(Identifier x, Identifier y) { return (x > y); }

    // bool m_check_on_setPoint;
    // bool m_simulation_Setup;
    std::string m_DataLocation;

    std::string m_hvFolder;
    std::string m_lvFolder;
    // std::string     m_jtagFolder;

    std::vector<std::string> parlineFolder() { return m_parlineFolder; }
    std::stringstream m_MDTChamDrop;
    std::stringstream m_MDTLV;
    std::stringstream m_MDTHV;
    std::stringstream m_MDTJTAG;
    IChronoStatSvc* m_chronoSvc;
    std::vector<std::string> m_parlineFolder;
    std::string m_chrono1;
    std::string m_chrono2;
    std::string m_chrono3;
    std::string m_chrono4;
    ToolHandle<IMDT_MapConversion> m_condMapTool;

    MsgStream m_log;
    bool m_debug;
    bool m_verbose;
};

#endif
