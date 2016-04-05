/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_TESTCOOLRECREAD_H
#define ATHENADBTESTREC_TESTCOOLRECREAD_H

#include <string>
#include <vector>
#include <fstream>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "AthenaDBTestRec/TestCoolRecFolder.h"

class IOVTime;

class TestCoolRecRead : public AthAlgorithm
{
 public:
  TestCoolRecRead(const std::string& name, ISvcLocator* pSvcLocator);
  ~TestCoolRecRead();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int readAuxFiles();
  // parameters
  std::vector<std::string> m_folders;
  std::vector<int> m_ftypes;
  std::vector<std::string> m_auxfiles;
  bool par_checkmatch;
  int par_delay;
  int par_dumpchan;
  std::string par_dumpfile;

  StoreGateSvc* p_detstore;
  std::vector<TestCoolRecFolder> m_folderlist;
  int m_nbadaux;
  std::ofstream* m_dumpf;
};

#endif // ATHENADBTESTREC_TESTCOOLRECREAD_H
