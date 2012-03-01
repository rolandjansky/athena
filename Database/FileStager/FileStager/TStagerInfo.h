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
  
using namespace std ;
  
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
  string infilePrefix;
  string outfilePrefix;
  string logfileDir;
  string baseTmpdir;
  string tmpdir;
  string cpcommand;
  string stagemonitorcmd;
  vector<string> cparg;
  map<string,string> inPrefixMap;
 
 private:

  ClassDef(TStagerInfo,1)
} ;

#endif

