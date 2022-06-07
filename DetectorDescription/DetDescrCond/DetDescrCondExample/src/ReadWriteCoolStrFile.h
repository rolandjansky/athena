/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
#define DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
// ReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
// of string objects stored in COOL via CoolStrFileSvc
// Richard Hawkings, started 14/11/05

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CxxUtils/checker_macros.h"

class ICoolStrFileSvc;

class ATLAS_NOT_THREAD_SAFE ReadWriteCoolStrFile: public AthAlgorithm
{
 public:
  ReadWriteCoolStrFile(const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadWriteCoolStrFile();
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  ICoolStrFileSvc* p_coolsvc;
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

  // methods
  void writeData ATLAS_NOT_THREAD_SAFE();
  void readData ATLAS_NOT_THREAD_SAFE();
};

#endif // DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
