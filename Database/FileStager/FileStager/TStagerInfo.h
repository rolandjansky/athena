/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TSTAGERINFO
#define TSTAGERINFO

#include "TObject.h"
#include <TROOT.h>

#ifndef __CINT__
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>
//#include <stdio.h>
#include <iostream>
#include <signal.h>
//#include <sys/signal.h>
#endif
  
#include <set>
#include <string>
#include <vector>
#include <map>
  
  
typedef char* pchar;

class TStagerInfo : public TObject
{
 public:
  TStagerInfo();
  virtual ~TStagerInfo();

  void setTmpdir();
  void setDefaultTmpdir();

  int pipeLength;
  int pid;
  std::string infilePrefix;
  std::string outfilePrefix;
  std::string logfileDir;
  std::string baseTmpdir;
  std::string tmpdir;
  std::string cpcommand;
  std::string stagemonitorcmd;
  std::vector<std::string> cparg;
  std::map<std::string,std::string> inPrefixMap;
 
 private:

  ClassDef(TStagerInfo,1)
} ;

#endif

