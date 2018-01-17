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
  bool m_par_read;
  bool m_par_extract;
  bool m_par_write;
  int m_par_chan;
  int m_par_tech;
  std::string m_par_folder;
  std::string m_par_rfile;
  std::string m_par_wfile;
  bool m_par_calT0;
  bool m_par_calRT;
  bool m_par_alignCorr;
  bool m_par_alignasciiformat;
  bool m_par_deadTube;
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
