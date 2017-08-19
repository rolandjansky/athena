/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_RPC_DCSCONDITIONSTOOL_H
#define MUONCONDTOOL_RPC_DCSCONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/IRPC_DCSConditionsTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"


class Identifier;
class StoreGateSvc;

class MdtHelper;
class IIOVSvc;
class StatusCode;
class IRPC_MapConversion;

class RPC_DCSConditionsTool: public AthAlgTool, virtual public IRPC_DCSConditionsTool
{

public:   

  RPC_DCSConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;

  virtual StatusCode initialize() override;

  virtual std::string  PanelDeadFolderName() const override {return m_deadPanelFolder;}
  virtual std::string  PanelOffFolderName() const override {return m_offPanelFolder;}



  virtual const std::vector<Identifier>& deadPanelId() const override { return m_cachedDeadPanelId;}
  virtual const std::vector<Identifier>& offPanelId() const override { return m_cachedOffPanelId;} 



 private:


 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadPanelOff(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadPanelDead(IOVSVC_CALLBACK_ARGS) override;


       
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;


  const RpcIdHelper* m_rpcIdHelper;
 
  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;
  
 
  std::vector<Identifier> m_cachedDeadPanelId;
  std::vector<Identifier> m_cachedOffPanelId;

 

  //private comparison function for Identifier sorting
  static bool compareId(Identifier x,Identifier y) { return (x > y); } 
  

  std::string      m_DataLocation;
 
  std::string     m_offPanelFolder;
  std::string     m_deadPanelFolder;

  std::vector<std::string> parlineFolder() { return m_parlineFolder; }
  std::stringstream m_RPCPaneloff;
  std::stringstream m_RPCPaneldead;

  IChronoStatSvc* m_chronoSvc;
  std::vector<std::string>      m_parlineFolder;



};
 


#endif
