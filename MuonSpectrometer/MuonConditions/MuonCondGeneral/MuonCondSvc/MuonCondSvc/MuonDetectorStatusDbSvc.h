/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MUONDETECTORSTATUSDBSVC_H
#define MUONCONDSVC_MUONDETECTORSTATUSDBSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondInterface/IMuonDetectorStatusDbTool.h"
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
#include "MuonCondData/MdtDeadTubeStatus.h"

class IIOVSvc;
template <class TYPE> class SvcFactory;

const InterfaceID InterfaceID_IMuonDetectorStatusDbSvc("MuonDetectorStatusDbSvc", 1, 0);
class IMuonDetectorStatusDBTool;


class MuonDetectorStatusDbSvc : public AthService, virtual public IInterface {
   friend class SvcFactory<MuonDetectorStatusDbSvc>;   
 public:
   MuonDetectorStatusDbSvc (const std::string& name, ISvcLocator* pSvcLocator);

  static const InterfaceID& interfaceID() { return InterfaceID_IMuonDetectorStatusDbSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);

 private:

   /** Tool handling the DB access */
  IMuonDetectorStatusDbTool * m_dbTool;

 std::string         m_tubeStatusDataLocation;
 std::string         m_dbToolType;
 std::string         m_dbToolName;

};
//}
#endif
