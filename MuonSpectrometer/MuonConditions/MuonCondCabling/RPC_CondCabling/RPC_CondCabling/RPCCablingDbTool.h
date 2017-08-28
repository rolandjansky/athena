/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_RPCCABLINGDBTOOL_H
#define MUONCONDTOOL_RPCCABLINGDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IRPCCablingDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

class Identifier; 
class StoreGateSvc; 

class RpcIdHelper;
class IIOVSvc;
class StatusCode;


class RPCCablingDbTool: public AthAlgTool, public IRPCCablingDbTool
{

public:    

  RPCCablingDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);
  
  virtual StatusCode initialize();

  virtual std::string mapConfFolderName() const {return m_mapFolder;}
  virtual std::string mapCorrFolderName() const {return m_corrFolder;}
  virtual const std::string* GetStringRPCConfMap() const {return &m_map;}
  virtual const std::string* GetStringRPCCorrMap() const {return &m_corr;}
    
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadRPCMap(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadRPCCorr(IOVSVC_CALLBACK_ARGS);
   

 private: 



    
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;


  const RpcIdHelper* m_rpcIdHelper;
  

  std::string      m_DataLocation;
  
  std::string     m_mapFolder;
  std::string     m_corrFolder;
  
  std::string m_map;
  std::string m_corr;    
    
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;


};
 


#endif
