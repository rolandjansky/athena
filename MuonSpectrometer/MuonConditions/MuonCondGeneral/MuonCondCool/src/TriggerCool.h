/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_TRIGGERCOOL_H
#define MUONCONDCOOL_TRIGGERCOOL_H


#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"


class StoreGateSvc;

namespace MuonCalib {

class ITriggerCoolSvc;

class TriggerCool: public AthAlgorithm
{
 public:

  TriggerCool(const std::string& name, ISvcLocator* pSvcLocator);
  ~TriggerCool(void);
  void readData();
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

 private:
  MsgStream m_log;
  StoreGateSvc* p_detstore;
  ITriggerCoolSvc* p_coolsvc;

  bool m_readDone;
  bool m_writeDoneEta;
  bool m_writeDonePhi;
  bool m_read;
  bool m_writeEta;
  bool m_writePhi;
  
  
  std::string m_fileName_CM_Phi; 
  std::string m_fileName_Th0_Phi;
  std::string m_fileName_CM; 
  std::string m_fileName_Th0;
  //  std::string m_fileName_Th1;
  //  std::string m_fileName_Th2;
  std::string m_sequence;
  std::string m_infoEta;
  std::string m_infoPhi;
  int m_chan_Eta;
  int m_chan_Phi;
  std::string  m_phiFolder;
  std::string  m_etaFolder;
  void writeDataEta();
  void readDataEta();
  void writeDataPhi();
  void readDataPhi();
  

};
}
#endif // MUONCONDCOOL_TRIGGERCOOL_H
