/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TSTAGEFILEINFO
#define TSTAGEFILEINFO

#include "TROOT.h"
#include "TObject.h"

#ifndef __CINT__
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#endif
#include <string>
#include <map>

using namespace std ;

class TStageFileInfo  : public TObject 
{
 public:
  enum Status { UNKNOWN, TOBESTAGED, STAGING, STAGED, 
		RELEASED, ERRORSTAGING, KILLEDSTAGING, 
		ERRORRELEASED, KILLERROR };

  TStageFileInfo() : pid(-999) {}; 
  virtual ~TStageFileInfo() {};

  int  pid;
  string inFile;
  string outFile;
  Status status;
#ifndef __CINT__
  struct stat64 statFile;
#endif
  string stout;
  string sterr;

 private:

  ClassDef(TStageFileInfo,1)
} ;


#endif
