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
  const std::string getTmpFilename(const char* filename);
  TStageFileInfo::Status getStatusOf(const char* filename, bool update=true);

  void setBaseTmpdir(const char* baseTmpdir);
  void setCpCommand(const char* cpcommand);
  void addCpArg(const char* cparg);
  void setPipeLength(const int& pipeLength);
  void setInfilePrefix(const char* infilePrefix);
  void setOutfilePrefix(const char* outfilePrefix);
  void setLogfileDir(const char* logfileDir);
  void addPrefixFix(const char* in, const char* out);

  void verbose(bool v=true) { m_verbose=v; }
  void verboseWait(bool v=true) { m_verbwait=v; }
  void tryInfRestage(bool t=true) { m_tryInfRestage=t; }
  void firstFileAlreadyStaged(bool f=true) { m_firstFileANDAlreadyStaged=f; }
  void keepLogfiles(bool k=true) { m_keepLogfiles=k; }
  void setPidInLognames(bool p=true) { m_pidInLognames=p; }
  void keepStagedfiles(bool k=true) { m_keepStagedfiles=k; }
  void checkForPreviousStage(bool c=true) { m_checkForPreviousStage=c; }
  void setInteractive(bool i=true) { m_keepLogfiles=i; m_keepStagedfiles=i; m_checkForPreviousStage=i; }
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
  void trim(std::string& input);
  void removePrefixOf(std::string& filename);
  void removeFile(std::string filename);
  void fixRootInPrefix(std::string& tmpname);

  void submitStageMonitor();
  bool m_submittedStageMonitor;

  bool fileExists(const char* fileName);

  std::list< std::string > m_toBeStagedList;
  std::map<std::string,TStageFileInfo> m_stageMap;

  static TStagerInfo s_stagerInfo;

  bool m_verbose;
  bool m_verbwait;
  bool m_tryInfRestage;
  bool m_firstFileANDAlreadyStaged;
  bool m_keepLogfiles;
  bool m_pidInLognames;
  bool m_keepStagedfiles;
  bool m_checkForPreviousStage;

  ClassDef(TStageManager,1)
} ;

#endif

