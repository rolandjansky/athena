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
  bool m_par_read;
  bool m_par_extract;
  bool m_par_write;
  bool m_par_deadTube;
  int m_par_chan;
  int m_par_tech;
  
  std::string m_par_folder;
  std::string m_par_rfile;
  std::string m_par_wfile;
  //std::string m_DeadTubeLocation;
  
  // methods
  void writeTube();
  void loadTube();
  void readTube();
  
};
}
#endif // MUONCONDCOOL_MDTTUBECONDCOOL_H
