/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MDT_DCSCONDITIONSTOOL_H
#define MUONCONDTOOL_MDT_DCSCONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCondInterface/IMDT_DCSConditionsTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCondInterface/IMDT_MapConversion.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "StoreGate/StoreGateSvc.h"

class Identifier;
class StoreGateSvc;

class MdtHelper;
class IIOVSvc;
class StatusCode;
class IMDT_MapConversion;

class MDT_DCSConditionsTool: public AthAlgTool, virtual public IMDT_DCSConditionsTool
{

public:   

  MDT_DCSConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;
  
  virtual StatusCode initialize() override;

  virtual std::string DropChamberFolderName() const override {return m_dropchamberFolder;}
  virtual std::string HVFolderName() const override {return m_hvFolder;}
  virtual std::string LVFolderName() const override {return m_lvFolder;}
  virtual std::string JTAGFolderName() const override {return m_jtagFolder;}
  virtual std::string SetPointsV0FolderName() const override {return m_setPointsV0Folder;}
  virtual std::string SetPointsV1FolderName() const override {return m_setPointsV1Folder;}

  virtual bool Simulation_Setup() const override {return m_simulation_Setup;}

  virtual const std::vector<std::string>& deadStations() const override { return m_cachedDeadStations;}
  virtual const std::vector<Identifier>& deadStationsId() const override { return m_cachedDeadStationsId;}
  virtual const std::vector<Identifier>& deadMultiLayersId() const override { return m_cachedDeadMultiLayersId;} 

  const std::string OnlineName(Identifier OfflineId);
  const Identifier OfflineName(std::string OnlineId);

 private:


 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadDropChamber(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadLV(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadJTAG(IOVSVC_CALLBACK_ARGS) override;

       
  StoreGateSvc* m_detStore;
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

  //private comparison function for Identifier sorting
  static bool compareId(Identifier x,Identifier y) { return (x > y); } 
  
  bool m_check_on_setPoint;
  bool m_simulation_Setup;
  std::string      m_DataLocation;
 
  std::string     m_dropchamberFolder;
  std::string     m_hvFolder;
  std::string     m_lvFolder;
  std::string     m_jtagFolder;
  std::string     m_setPointsV0Folder;
  std::string     m_setPointsV1Folder;
  std::vector<std::string> parlineFolder() { return m_parlineFolder; }
  std::stringstream m_MDTChamDrop;
  std::stringstream m_MDTLV;
  std::stringstream m_MDTHV;
  std::stringstream m_MDTJTAG;
  IChronoStatSvc* m_chronoSvc;
  std::vector<std::string>      m_parlineFolder;
  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;
  ToolHandle<IMDT_MapConversion> m_condMapTool;
 
  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;
};
 


#endif
