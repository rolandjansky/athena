/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TStagerInfo.h"
#include <stdlib.h>

ClassImp(TStagerInfo)


TStagerInfo::TStagerInfo()
    : pipeLength(1)
    , pid(getpid())
    , baseTmpdir("/tmp")
    , tmpdir("/tmp")
    , cpcommand("cp")
    , stagemonitorcmd("StageMonitor.exe") 
{
  setDefaultTmpdir();

  inPrefixMap["lfn/"] = "lfn:/";
  inPrefixMap["srm//"] = "srm://";
  inPrefixMap["dcap//"] = "dcap://";
  inPrefixMap["sfn/"] = "sfn:/";
  inPrefixMap["gsiftp/"] = "gsiftp:/";
  inPrefixMap["rfio/"] = "rfio:/";
  inPrefixMap["http/"] = "http:/";
  inPrefixMap["file/"] = "file:/";
  inPrefixMap["ftp/"] = "ftp:/";
  //inPrefixMap["root//"] = "root://";
  inPrefixMap["root//castoratlas"] = "root://castoratlas/";

}


TStagerInfo::~TStagerInfo() {}


void TStagerInfo::setTmpdir()
{
  // MB: make baseTmpdir the default so actual tmpdir is easily known from outside ...
  tmpdir = baseTmpdir;

  char pidchar[25];
  sprintf(pidchar,"%d",pid);
  string testtmpdir = baseTmpdir+"/"+getenv("USER")+"_pID"+pidchar;
  int errnum = mkdir(testtmpdir.c_str(),0700);

  // test the tmpdir ...
  if ( !(errnum==0 || errnum==2) ) {
    // no permissions to write in base directory, switching to /tmp
    cerr << "TStagerInfo::setTmpdir() : No permission to write in temporary directory <"
         << baseTmpdir
         << ">. Switching back to default <$TMPDIR>, or else </tmp>."
         << endl;
    setDefaultTmpdir();
  } else {
    // writing works, leave nothing behind
    rmdir(testtmpdir.c_str());
  }
}


void TStagerInfo::setDefaultTmpdir()
{
  const char* tmp1 = getenv("TMPDIR");
  const char* tmp2 = getenv("WORKDIR");
  const char* tmp3 = getenv("OSG_WN_TMP");
  const char* tmp4 = getenv("EDG_WL_SCRATCH");

  if (tmp4!=0) {
    baseTmpdir = tmp4;
    tmpdir = tmp4;
  } else if (tmp3!=0) {
    baseTmpdir = tmp3;
    tmpdir = tmp3;
  } else if (tmp2!=0) {
    baseTmpdir = tmp2;
    tmpdir = tmp2;
  } else if (tmp1!=0) {
    baseTmpdir = tmp1;
    tmpdir = tmp1;
  } else { 
    baseTmpdir = "/tmp";
    tmpdir = "/tmp";
  }
}
