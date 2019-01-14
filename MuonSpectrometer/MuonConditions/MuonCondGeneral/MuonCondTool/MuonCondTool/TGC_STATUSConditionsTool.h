/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_TGC_STATUSCONDITIONSTOOL_H
#define MUONCONDTOOL_TGC_STATUSCONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/ITGC_STATUSConditionsTool.h"


class Identifier;
class StoreGateSvc;

class IIOVSvc;
class StatusCode;


class TGC_STATUSConditionsTool: public AthAlgTool, virtual public ITGC_STATUSConditionsTool {

public:   

  TGC_STATUSConditionsTool(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  
  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

 private:
  virtual StatusCode initialize();

  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadTgcDqStatus(IOVSVC_CALLBACK_ARGS);
  virtual std::string FolderName() const {return m_FolderName;}
  
  const TgcIdHelper* m_tgcIdHelper;
  
  virtual const std::vector<Identifier>& deadStationsId(){ return m_cachedDeadStationsId;}
           
  IIOVSvc* m_IOVSvc;

 
  std::vector<Identifier> m_cachedDeadStationsId;
  
  std::string      m_tgcDqFolder;
  std::string      m_tgcDqStatusDataLocation;
  std::string      m_FolderName;

  
        
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
