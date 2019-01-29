/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANDAROOTTOOLS_H_
#define PANDAROOTTOOLS_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>

class PandaRootTools {

 public:
  
  PandaRootTools();

  virtual ~PandaRootTools(){}

  TString getNextFile();

  int GetNumberOfInputFiles();
  int GetFilesRead();

  void NotifyJobFinished(uint64_t eventsProcessed);

  void Fail(uint64_t eventsProcessed);
  void Abort();

 private:    

  enum ErrorCodes {
    EC_FAIL = 220,
    EC_ABORT = 221,
    EC_NOTFINISHED = 222,
    EC_BADINPUT = 223
  };

  std::vector<TString> m_fileList; 
  unsigned int m_currentFile;

  void createJobSummary(uint64_t eventsProcessed);

  ClassDef(PandaRootTools, 1)
};

#endif
