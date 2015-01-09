/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MUONDETECTORSTATUSDBSVC_H
#define MUONCONDSVC_MUONDETECTORSTATUSDBSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
//#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonCondInterface/IMuonDetectorStatusDbTool.h"
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
#include "MuonCondData/MdtDeadTubeStatus.h"


class IIOVSvc;
template <class TYPE> class SvcFactory;
//namespace MuonCalib 
//{  
const InterfaceID InterfaceID_IMuonDetectorStatusDbSvc("MuonDetectorStatusDbSvc", 1, 0);
class IMuonDetectorStatusDBTool;


class MuonDetectorStatusDbSvc : public AthService, virtual public IInterface {
   friend class SvcFactory<MuonDetectorStatusDbSvc>;   
 public:
   MuonDetectorStatusDbSvc (const std::string& name, ISvcLocator* pSvcLocator);

  
  
  static const InterfaceID& interfaceID() { return InterfaceID_IMuonDetectorStatusDbSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
  
  
 public:
  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);

  //virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS);
  //virtual StatusCode loadTubeStatus(IOVSVC_CALLBACK_ARGS);
 
 private:
  

  
  // Pointer to storegate svc
  //StoreGateSvc* m_storeGate;
  // Pointer to the detector store
  //StoreGateSvc* m_detStore;
  // Pointer to IOVSvc
  //IIOVSvc *p_IOVSvc;


  //const MdtIdHelper* m_mdtIdHelper;

 
  //MuonDetectorStatusDbTool* m_MuonDetectorStatusDbTool;
    
  // Pointer to data containers for the detector store
 
  //mutable MdtDeadTubeStatusContainer * m_tubeStatusData; 
   
//    // Pointer to MuonDetectorManager (imt addition)
//   const MuonGM::MuonDetectorManager* m_MuonDetectorManager;
  
  


   /** Tool handling the DB access */
  IMuonDetectorStatusDbTool * m_dbTool;

/** Indexed with MdtRegionHash for rt-regions. */ 
/** Properties: */


 std::string         m_tubeStatusDataLocation;
 std::string         m_dbToolType;
 std::string         m_dbToolName;

 
};
//}
#endif
