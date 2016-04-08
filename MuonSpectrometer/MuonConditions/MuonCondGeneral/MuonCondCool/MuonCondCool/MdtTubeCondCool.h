/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_MDTTUBECONDCOOL_H
#define MUONCONDCOOL_MDTTUBECONDCOOL_H
// ReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
// of string objects stored in COOL via CoolStrFileSvc


#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"
#include "MuonCondData/MdtDeadTubeStatus.h"
#include "MuonCondData/MdtDeadTubeStatusContainer.h" 

class MdtIdHelper;
class StoreGateSvc;
class MDTGeometry;

class CSMid;
namespace MuonCalib {
class MdtICoolStrSvc;

class MdtTubeCondCool: public AthAlgorithm
{
 public:
  MdtTubeCondCool(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdtTubeCondCool(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

 private:
  MsgStream m_log;
  StoreGateSvc* p_detstore;
  StoreGateSvc* m_storeGate;
  MdtICoolStrSvc* p_coolsvc;
  MdtDeadTubeStatusContainer* m_mdtDeadTubeStatusContainer;
  bool m_done;
// properties
  bool par_read;
  bool par_extract;
  bool par_write;
  bool par_deadTube;
  int par_chan;
  int par_tech;
  
  std::string par_folder;
  std::string par_rfile;
  std::string par_wfile;
  //std::string m_DeadTubeLocation;
  
  // methods
  void writeTube();
  void loadTube();
  void readTube();
  
};
}
#endif // MUONCONDCOOL_MDTTUBECONDCOOL_H
