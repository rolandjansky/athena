/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TSTAGEMANAGER
#define TSTAGEMANAGER

#include <TROOT.h>
#include "FileStager/TStageFileInfo.h"
#include "FileStager/TStagerInfo.h"

#include "TObject.h"

#ifndef __CINT__
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>
//#include <sys/signal.h>
#endif

#include <set>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std ;

class TStagerInfo;

typedef char* pchar;

class TStageManager : public TObject 
{
 public:

  // access to the single instance of this class (created statically on first request)
  static TStageManager& instance();

  void addToList(const char* filename);
  void releaseFile(const char* fname);
  void releaseAll();
  const char* getFile(const char* fname);
  const string getTmpFilename(const char* filename);
  TStageFileInfo::Status getStatusOf(const char* filename, bool update=true);

  void setBaseTmpdir(const char* baseTmpdir);
  void setCpCommand(const char* cpcommand);
  void addCpArg(const char* cparg);
  void setPipeLength(const int& pipeLength);
  void setInfilePrefix(const char* infilePrefix);
  void setOutfilePrefix(const char* outfilePrefix);
  void setLogfileDir(const char* logfileDir);
  void addPrefixFix(const char* in, const char* out);

  void verbose(bool v=true) { _verbose=v; }
  void verboseWait(bool v=true) { _verbwait=v; }
  void tryInfRestage(bool t=true) { _tryInfRestage=t; }
  void firstFileAlreadyStaged(bool f=true) { _firstFileANDAlreadyStaged=f; }
  void keepLogfiles(bool k=true) { _keepLogfiles=k; }
  void setPidInLognames(bool p=true) { _pidInLognames=p; }
  void keepStagedfiles(bool k=true) { _keepStagedfiles=k; }
  void checkForPreviousStage(bool c=true) { _checkForPreviousStage=c; }
  void setInteractive(bool i=true) { _keepLogfiles=i; _keepStagedfiles=i; _checkForPreviousStage=i; }
  void print();
  void printProblemFiles();

  int  getNstaging();
  int  getNtobestaged();
  int  getNstaged();
  int  getNtotal();
  int  getSizeStaged();
  int  getNproblem();

 private:

  // default constructor (called by instance())
  TStageManager();
  virtual ~TStageManager();

  // Private copy constructor : not implemented!
  TStageManager( TStageManager&);
  // Private assignment : not implemented!
  TStageManager& operator= (TStageManager&);

  void stageNext(bool forceStage=false);
  void updateStatus();
  void trim(string& input);
  void removePrefixOf(string& filename);
  void removeFile(string filename);
  void fixRootInPrefix(string& tmpname);

  void submitStageMonitor();
  bool _submittedStageMonitor;

  bool fileExists(const char* fileName);

  list< string > _toBeStagedList;
  map<string,TStageFileInfo> _stageMap;

  static TStagerInfo _stagerInfo;

  bool _verbose;
  bool _verbwait;
  bool _tryInfRestage;
  bool _firstFileANDAlreadyStaged;
  bool _keepLogfiles;
  bool _pidInLognames;
  bool _keepStagedfiles;
  bool _checkForPreviousStage;

  ClassDef(TStageManager,1)
} ;

#endif

