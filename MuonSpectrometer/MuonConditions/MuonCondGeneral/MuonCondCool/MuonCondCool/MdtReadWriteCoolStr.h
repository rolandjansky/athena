/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_MDTREADWRITECOOLSTR_H
#define MUONCONDCOOL_MDTREADWRITECOOLSTR_H
// ReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
// of string objects stored in COOL via CoolStrFileSvc


#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

class MdtIdHelper;
class StoreGateSvc;

namespace MuonCalib {
class MdtICoolStrSvc;

class MdtReadWriteCoolStr: public AthAlgorithm
{
 public:
  MdtReadWriteCoolStr(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdtReadWriteCoolStr(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

 private:
  MsgStream m_log;
  StoreGateSvc* p_detstore;
  MdtICoolStrSvc* p_coolsvc;

  bool m_done;
// properties
  bool par_read;
  bool par_extract;
  bool par_write;
  int par_chan;
  int par_tech;
  std::string par_folder;
  std::string par_rfile;
  std::string par_wfile;
  bool par_calT0;
  bool par_calRT;
  bool par_alignCorr;
  bool par_alignasciiformat;
  bool par_deadTube;
  // methods
  void writeData();
  void loadData();
  void readData();
  void loadDataRt();
  void loadDataAlign();
  void loadDataAlignAscii();
};
}
#endif // MUONCONDCOOL_MDTREADWRITECOOLSTR_H
