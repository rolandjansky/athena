/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <csignal>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <dirent.h>
//#include <sys/signal.h>

#include <string>
#include <vector>

using namespace std ;

// global dirs
string tmpdir;
string baseTmpdir;
bool keepLogfiles;
bool keepStagedfiles;
bool verbose(false);

void term(int sig)
{
  //..necessary cleanup operations before terminating
  if (verbose) std::cout << "StageMonitor : now handling signal : " << sig << std::endl;

  // get dir contents ...
  vector<string> files;
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(tmpdir.c_str())) == NULL) {
    //cout << "Error(" << errno << ") opening " << tmpdir << endl;
  } else {
    while ((dirp = readdir(dp)) != NULL) {
      string ifile = (dirp->d_name) ;
      if (ifile.find("tcf_")!=ifile.npos) files.push_back(tmpdir+"/"+ifile);
    }
    closedir(dp);
  }

  bool warningPrinted(false);

  // delete stuff
  for (unsigned int i=0; i<files.size(); i++) {
    //cout << files[i] << endl;
    if (keepLogfiles || keepStagedfiles) {
      if ((files[i].rfind(".out")==files[i].size()-4) || 
          (files[i].rfind(".err")==files[i].size()-4)) { // log files
        if (keepLogfiles) {
          if (!warningPrinted) {
            std::cout << "StageMonitor() : WARNING : Make sure to manually remove : " << std::endl;
            warningPrinted = true;
          }
          std::cout << "rm -f " << files[i] << std::endl;
          continue;
        }
      } else if (keepStagedfiles) { 
        if (!warningPrinted) {
          std::cout << "StageMonitor() : WARNING : Make sure to manually remove : " << std::endl;
          warningPrinted = true;
        }
        std::cout << "rm -f " << files[i] << std::endl;
        continue; // remainder are staged files
      }
    } else remove(files[i].c_str());
  }  
  if (tmpdir.compare(baseTmpdir)!=0)
    rmdir(tmpdir.c_str());
}


//-----------------------------------
int main(int argc, const char** argv)
//-----------------------------------
{
  if ( argc<4 ) {
    std::cout << "usage: " << argv[0] << " <pid> <tmpdir> <tmpdirbase> [<keepLogFiles>] [<keepStagedFiles>]" << std::endl ;
    return 1 ;
  }

  signal(SIGTERM, term); // register a SIGTERM handler
  
  bool doCleanup(false);
  pid_t pID = atoi(argv[1]);
  tmpdir = argv[2];
  baseTmpdir = argv[3];
  if (argc>=5) keepLogfiles = (bool)atoi(argv[4]);
  else keepLogfiles = false;
  if (argc>=6) keepStagedfiles = (bool)atoi(argv[5]);
  else keepStagedfiles = false;

  //cout << argv[0] << " : Monitoring process with id = " << pID << endl;
  int killReturn;

  while (1) {
    killReturn = kill(pID,0);
    //cout <<  argv[0] << " : " << killReturn << endl;
    if( killReturn == -1 ) {     
      // pid does not exist
      doCleanup=true;
      break;
    }
    sleep(10);
  }

  if (doCleanup) raise(SIGTERM); // will cause term() to run

  exit(0);
}

