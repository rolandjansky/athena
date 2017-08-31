/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_RPCTRIGGERDBTOOL_H
#define MUONCONDTOOL_RPCTRIGGERDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IRPCTriggerDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

class Identifier; 
class StoreGateSvc; 

class RpcIdHelper;
class IIOVSvc;
class StatusCode;


class RPCTriggerDbTool: public AthAlgTool, public IRPCTriggerDbTool
{

public:    

  RPCTriggerDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

  virtual StatusCode initialize();
  
  //getter methods
  virtual std::string EtaTableFolderName() const {return m_etaTableFolder;}
  virtual std::string PhiTableFolderName() const {return m_phiTableFolder;}
  
  virtual const std::vector<std::string> GetStringRPCEtaCM_File() const {return m_vecetaCM_File;}
  virtual const std::vector<std::string> GetStringRPCEtaTh0() const {return m_vecetaTh0;}
  //  virtual const std::vector<std::string> GetStringRPCEtaTh1() const {return m_vecetaTh1;}
  //  virtual const std::vector<std::string> GetStringRPCEtaTh2() const {return m_vecetaTh2;}
  virtual const std::vector<std::string> GetStringRPCEtaSequence_Th() const {return  m_vecetaSequence_Th;}
  virtual const std::vector<std::string> GetStringRPCEtaInfo() const {return m_vecetaInfo;}
  
  virtual const std::vector<std::string> GetStringRPCPhiCM_File() const {return m_vecphiCM_File;}
  virtual const std::vector<std::string> GetStringRPCPhiTh0() const {return m_vecphiTh0;}
  virtual const std::vector<std::string> GetStringRPCPhiInfo() const {return m_vecphiInfo;}
  virtual const std::map<std::string, std::string>* GetRoads() const {return &m_trigroads;}
   
  //load methods  
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadRPCEtaTable(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadRPCPhiTable(IOVSVC_CALLBACK_ARGS);
   

 private: 



    
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;


  bool m_onlyDebug;
  std::string      m_DataLocation;
  
  std::string     m_etaTableFolder;
  std::string     m_phiTableFolder;
  
  std::string m_etaCM_File;
  std::vector<std::string> m_vecetaCM_File;
  std::string m_etaTh0;    
  std::vector<std::string> m_vecetaTh0;
  //std::string m_etaTh1;    
  //std::string m_etaTh2; 
  std::string m_etaSequence_Th;       
  std::vector<std::string> m_vecetaSequence_Th;
  std::string m_etaInfo;   
  std::vector<std::string> m_vecetaInfo;
  
  std::string m_phiCM_File;
  std::vector<std::string> m_vecphiCM_File;
  std::string m_phiTh0;    
  std::vector<std::string> m_vecphiTh0;
  std::string m_phiInfo;   
  std::vector<std::string> m_vecphiInfo;

  std::map<std::string, std::string> m_trigroads;  

  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;
  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;

};
 


#endif
