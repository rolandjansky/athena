/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MUONALIGNMENTDBSVC_H
#define MUONCONDSVC_MUONALIGNMENTDBSVC_H


#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include "MuonCondInterface/IMuonAlignmentDbTool.h"


class IIOVSvc;
namespace MuonCalib 
{  
const InterfaceID InterfaceID_IMuonAlignmentDbSvc("MuonAlignmentDbSvc", 1, 0);
class IMuonAlignmentDBTool;


class MuonAlignmentDbSvc : public AthService, virtual public IInterface {
  friend class SvcFactory<MuonAlignmentDbSvc>; 
 public:
   MuonAlignmentDbSvc (const std::string& name, ISvcLocator* pSvcLocator);

  
  
  static const InterfaceID& interfaceID() { return InterfaceID_IMuonAlignmentDbSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
  
  
 public:
  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);

 private:
  
  

   /** Tool handling the DB access */
   IMuonAlignmentDbTool * m_dbTool;

 std::string         m_alineDataLocation;
 std::string         m_blineDataLocation;
 
 std::string         m_dbToolType;
 std::string         m_dbToolName;

 
};
}
#endif
