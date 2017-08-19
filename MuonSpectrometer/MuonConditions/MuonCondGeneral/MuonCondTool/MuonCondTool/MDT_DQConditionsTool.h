/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MDT_DQCONDITIONSTOOL_H
#define MUONCONDTOOL_MDT_DQCONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/IMDT_DQConditionsTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCondInterface/IMDT_MapConversion.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"

class Identifier;
class StoreGateSvc;

class MdtHelper;
class IIOVSvc;
class StatusCode;
class IMDT_MapConversion;

class MDT_DQConditionsTool: public AthAlgTool, virtual public IMDT_DQConditionsTool
{

public:   

  MDT_DQConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;
  
  virtual StatusCode initialize() override;

  virtual std::string DeadFolderName() const override {return m_deadFolder;}
  virtual std::string NoisyFolderName() const override {return m_noisyFolder;}

  virtual bool Simulation_Setup() const {return m_simulation_Setup;}

  virtual const std::vector<std::string>& deadStations() const { return m_cachedDeadStations;}
  virtual const std::vector<Identifier>& deadStationsId() const { return m_cachedDeadStationsId;}
  virtual const std::vector<Identifier>& deadTubesId() const override { return m_cachedDeadTubesId;}
  //  virtual const std::vector<Identifier>& deadTubes(){ return m_cachedDeadTubes;}
  virtual const std::vector<Identifier>& deadMultiLayersId() const override { return m_cachedDeadMultiLayersId;} 
  virtual const std::vector<Identifier>& deadLayersId() const override { return m_cachedDeadLayersId;} 


  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const override { return m_Chamber_with_deadTube;}

  const std::string OnlineName(Identifier OfflineId);
  const Identifier OfflineName(std::string OnlineId);

 private:


 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadDeadChamber(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadNoisyChamber(IOVSVC_CALLBACK_ARGS) override;

       
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;

  std::map<std::string, Identifier> m_Chamber_Map;

  std::map<int, Identifier> m_Chamber_Naming_standby;


  const MdtIdHelper* m_mdtIdHelper;
 
  std::string m_OnlineName;
  Identifier m_OfflineName;
  
  std::vector<Identifier> m_Chamber_with_deadTube;
  std::vector<std::string> m_cachedDeadStations;
  std::vector<Identifier> m_cachedDeadStationsId;

  std::vector<std::string> m_cachedDeadMultiLayers;
  std::vector<Identifier> m_cachedDeadMultiLayersId;

  std::vector<std::string> m_cachedDeadLayers;
  std::vector<Identifier> m_cachedDeadLayersId;

  std::vector<std::string> m_cachedDeadTubes;
  std::vector<Identifier> m_cachedDeadTubesId;




  std::vector<Identifier> m_cachedNoisyStationsId;
  std::vector<Identifier> m_cachedNoisyMultiLayersId;
  std::vector<Identifier> m_cachedNoisyLayersId;
  std::vector<Identifier> m_cachedNoisyTubesId;


  //private comparison function for Identifier sorting
  static bool compareId(Identifier x,Identifier y) { return (x > y); } 
  
  //  bool m_check_on_setPoint;
  bool m_simulation_Setup;
  //  std::string      m_DataLocation;
 
  std::string     m_deadFolder;
  std::string     m_noisyFolder;
  
  //std::vector<std::string> parlineFolder() { return m_parlineFolder; }
 
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
