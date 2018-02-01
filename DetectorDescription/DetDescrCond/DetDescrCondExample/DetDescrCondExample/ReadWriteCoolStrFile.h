/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
#define DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
// ReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
// of string objects stored in COOL via CoolStrFileSvc
// Richard Hawkings, started 14/11/05

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

class ICoolStrFileSvc;

class ReadWriteCoolStrFile: public AthAlgorithm
{
 public:
  ReadWriteCoolStrFile(const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadWriteCoolStrFile(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);

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
  void writeData();
  void readData();
};

#endif // DETDESCRCONDEXAMPLE_READWRITECOOLSTRFILE_H
