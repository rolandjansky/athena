/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TStageManager.h"
#include "FileStager/TCopyFile.h"
#include <libgen.h>
#include <assert.h>


ClassImp(TStageManager)


TStagerInfo TStageManager::_stagerInfo;


TStageManager::TStageManager() 
 : _submittedStageMonitor(false)
 , _verbose(false)
 , _verbwait(false)
 , _tryInfRestage(false)
 , _firstFileANDAlreadyStaged(false)
 , _keepLogfiles(false)
 , _pidInLognames(true)
 , _keepStagedfiles(false)
 , _checkForPreviousStage(false)
{
  _stageMap.clear();
  _toBeStagedList.clear();
}


TStageManager::~TStageManager() 
{
  print();

  // terminating session, so cleanup
  //_keepStagedfiles = false;
  releaseAll();

  // remove temporary stage directory
  if (_stagerInfo.tmpdir.compare(_stagerInfo.baseTmpdir)!=0)
    rmdir(_stagerInfo.tmpdir.c_str());

  // TCopyFile destructor can no longer call stagemanager after this has been destroyed.
  TCopyFile::SetOriginalTFile();
}


TStageManager&
TStageManager::instance() 
{
  // created on first call, deleted after main()
  static TStageManager _instance;
  return _instance;
}


void 
TStageManager::addToList(const char* filename)
{
  string input(filename);
  trim(input);

  if (_verbose) { cout << "TStageManager::addToList() : <" << input << ">" << endl; }

  if (_firstFileANDAlreadyStaged) {
    _stageMap[input] = TStageFileInfo();
    _stageMap[input].status = TStageFileInfo::STAGED;
    string inFile(input);
    removePrefixOf(inFile);
    _stageMap[input].inFile = inFile;
    _stageMap[input].outFile = getTmpFilename(input.c_str());
    _stageMap[input].stout = _stageMap[input].outFile + "_stage.out";
    _stageMap[input].sterr = _stageMap[input].outFile + "_stage.err";
    if (!_stagerInfo.logfileDir.empty()) {
      _stageMap[input].stout = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[input].stout.c_str());
      _stageMap[input].sterr = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[input].sterr.c_str());
    }
    _firstFileANDAlreadyStaged = false; // next file
  }

  list<string>::iterator itrF = find(_toBeStagedList.begin(),_toBeStagedList.end(),input);
  if (_stageMap.find(input)==_stageMap.end()) {
    if (itrF==_toBeStagedList.end())  _toBeStagedList.push_back(input);
  } else if ( _stageMap.find(input)->second.status != TStageFileInfo::STAGED && 
              _stageMap.find(input)->second.status != TStageFileInfo::STAGING ) {
    // not interested. cleanup
    releaseFile(input.c_str()) ;
    _stageMap.erase(_stageMap.find(input));
    if (itrF==_toBeStagedList.end())  _toBeStagedList.push_back(input);
  }

  stageNext();
}


void 
TStageManager::releaseFile(const char* fname)
{
  string tmpname(fname);
  trim(tmpname);
  fixRootInPrefix(tmpname);
  const char* filename = tmpname.c_str();

  // first update status
  updateStatus();

  list<string>::iterator itrF = find(_toBeStagedList.begin(),_toBeStagedList.end(),filename);
  if(itrF!=_toBeStagedList.end()) {
    _toBeStagedList.erase(itrF);
    return;
  }

  if(_stageMap.find(filename)==_stageMap.end()) return;

  if (_stageMap[filename].status==TStageFileInfo::RELEASED ||
      _stageMap[filename].status==TStageFileInfo::KILLEDSTAGING ||
      _stageMap[filename].status==TStageFileInfo::ERRORRELEASED ||
      _stageMap[filename].status==TStageFileInfo::KILLERROR) { 
    return;
  }

  if (_verbose) cout << "TStageManager::releaseFile() : " << filename << endl;
    
  if (_stageMap[filename].status==TStageFileInfo::STAGING) {
    // kill process first
    int  killReturn = kill(_stageMap[filename].pid, SIGKILL);  

    if( killReturn == ESRCH) {     
      // pid does not exist
      if (_verbose) cout << "Group does not exist!" << endl;
      // assume file is done staging
      // fill statistics first
      stat64(_stageMap[filename].outFile.c_str(),&(_stageMap[filename].statFile));
      _stageMap[filename].status=TStageFileInfo::STAGED;
    } else if( killReturn == EPERM) { 
      // No permission to send kill signal
      // This should never happen
      if (_verbose) cout << "No permission to send kill signal!" << endl;
      _stageMap[filename].status=TStageFileInfo::KILLERROR;
    } else {
      if (_verbose) cout << "Kill signal sent. All Ok!" << endl;
      _stageMap[filename].status=TStageFileInfo::KILLEDSTAGING;
    }
  }

  if (_stageMap[filename].status==TStageFileInfo::ERRORSTAGING) {
    _stageMap[filename].status=TStageFileInfo::ERRORRELEASED;
  }

  if (_stageMap[filename].status==TStageFileInfo::STAGED) {
    if (!_keepStagedfiles) {
      removeFile(_stageMap[filename].outFile);
      _stageMap[filename].status=TStageFileInfo::RELEASED;
    }
  }

  if (!_keepLogfiles) {
    removeFile(_stageMap[filename].stout);
    removeFile(_stageMap[filename].sterr);
  }
}


void TStageManager::releaseAll()
{
  _toBeStagedList.clear();

  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); itr=_stageMap.begin()) {
    string first = (itr->first);
    releaseFile(first.c_str());
    _stageMap.erase(itr);
  }
}


const char* 
TStageManager::getFile(const char* fname)
{
  string tmpname(fname);
  trim(tmpname);
  fixRootInPrefix(tmpname);
  const char* filename = tmpname.c_str();

  string name(filename);
  trim(name);
  removePrefixOf(name);

  if (_verbose) cout << "TStageManager::getFile() : " << filename << endl;

  // first update status
  updateStatus();

  // file not found -> better start staging immediately.
  list<string>::iterator itrF = find(_toBeStagedList.begin(),_toBeStagedList.end(),filename);
  if (_stageMap.find(filename)==_stageMap.end() && 
      (itrF==_toBeStagedList.end()) ) {
    if (_verbose) cout << "TStageManager::getFile() : " << filename << " unknown. Start immediate staging." << endl;
    _toBeStagedList.push_front(filename);
  }

  // prioritize file needs to be staged
  itrF = find(_toBeStagedList.begin(),_toBeStagedList.end(),filename);
  if (itrF!=_toBeStagedList.end() && _stageMap.find(filename)==_stageMap.end()) {
    // move file to front
    _toBeStagedList.erase(itrF);
    _toBeStagedList.push_front(filename);

    if (_verbose) cout << "TStageManager::getFile() : " << filename << ". Forced staging." << endl;
    stageNext(true); // forced stage right away
  }

  if (_stageMap.find(filename)!=_stageMap.end()) {
    if (_verbose) cout << "TStageManager::getFile() : Checking staging status of " << filename << endl;

    // file still staging
    if (_stageMap[filename].status==TStageFileInfo::STAGING) {

      // check status    
      pid_t pID = _stageMap[filename].pid;
      int childExitStatus;
      
      // wait till staging is done
      if (_verbose || _verbwait) { cout << "TStageManager::getFile()   : Waiting till <"
		   	                << filename
			                << "> is staged."
			                << endl; }

      waitpid( pID, &childExitStatus, 0);
      
      if( !WIFEXITED(childExitStatus) ) {
	if (_verbose) { cout << "waitpid() exited with status= " 
	                     << WEXITSTATUS(childExitStatus)
	                     << endl; }
	//_stageMap[filename].status = TStageFileInfo::ERRORSTAGING;
      } else if( WIFSIGNALED(childExitStatus) ) {
	if (_verbose) { cout << "waitpid() exited with signal: " 
	                     << WTERMSIG(childExitStatus)
	                     << endl; }
	//_stageMap[filename].status = TStageFileInfo::ERRORSTAGING;
      } else {
	// child exited okay
	if (_verbose) { cout << "waitpid() okay." << endl; }
	//_stageMap[filename].status = TStageFileInfo::STAGED;
      }
    }

    // check if file _really_ exists. Sometimes staging returns empty files.
    bool fexists = fileExists(_stageMap[filename].outFile.c_str());
    if (fexists) _stageMap[filename].status = TStageFileInfo::STAGED;
    else {
      if (_verbose) cout << "TStageManager::getFile() : ERROR : staging of file <" << filename << "> failed. Note: This could be an empty file or time-out." << endl;
    }

    // file is staged
    if (_stageMap[filename].status == TStageFileInfo::STAGED) {
      if (_verbose) cout << "TStageManager::getFile() : <" << filename << "> finished staging" << endl;

      stat64(_stageMap[filename].outFile.c_str(),&(_stageMap[filename].statFile));
      // start next stage
      stageNext();
      return _stageMap[filename].outFile.c_str();

    // file has already been run over ... 
    } else if (_stageMap[filename].status == TStageFileInfo::RELEASED) {
      if (_verbose) cout << "TStageManager::getFile() : WARNING : <" << filename << "> already run over!" << endl;
      /// Cleanup and let's try again.
      if (_verbose) cout << "TStageManager::getFile() : restaging <" << filename << "> immediately." << endl;
      releaseFile(fname);
      _stageMap.erase(_stageMap.find(filename));
      return getFile(fname);

    // that's funny ... staged but no file!    
    } else { 

      if (_tryInfRestage) { // false by default -- restaging done by wrapper script
	//Something went wrong. Cleanup and let's try again.
	if (_verbose) cout << "TStageManager::getFile() : ERROR : restaging <" << filename << "> immediately." << endl;
	releaseFile(fname);
	_stageMap.erase(_stageMap.find(filename));
	return getFile(fname);
      }

      cout << "TStageManager::getFile() : ERROR : staging of <" << filename << "> failed (after N retries). Giving up." << endl;      
    }
  }

  // This should not be reached
  if (_verbose) cout << "TStageManager::getFile() : ERROR <" << filename << "> not recognized." << endl;

  stageNext();

  return name.c_str(); // Let TFile handle error
}


const string
TStageManager::getTmpFilename(const char* filename)
{
  if (_stageMap.find(filename)!=_stageMap.end())
    if (!_stageMap[filename].outFile.empty()) 
      return _stageMap[filename].outFile.c_str();
  
  string infile(filename); trim(infile);
  string tmpfile(filename); trim(tmpfile);
  //removePrefixOf(infile);
  string::size_type pos = tmpfile.find_last_of("/");
  tmpfile = _stagerInfo.tmpdir + "/tcf_" + /*string(Form("tcf%d_",getpid())) +*/ tmpfile.substr(pos+1,tmpfile.size()-pos-1);

  // double check!!
  string::size_type posi = infile.find_last_of("/");
  string first  = infile.substr(posi+1,infile.size()-posi-1);
  posi = tmpfile.find(first);
  tmpfile.erase( posi + first.size() );

  if (_verbose) cout << "TStageManager::getTmpFilename() : <" << tmpfile << "> <" 
                     << tmpfile.c_str() << ">"
                     << endl;

  return tmpfile;
}


TStageFileInfo::Status
TStageManager::getStatusOf(const char* filename, bool update) 
{
  if (update) updateStatus();

  list<string>::iterator itrF = find(_toBeStagedList.begin(),_toBeStagedList.end(),filename);
  if(itrF!=_toBeStagedList.end()) {
    return TStageFileInfo::TOBESTAGED;
  }

  if(_stageMap.find(filename)!=_stageMap.end()) 
    return _stageMap[filename].status;

  return TStageFileInfo::UNKNOWN;
}


void
TStageManager::print()
{
  updateStatus();
  TStageFileInfo::Status status;
  string filename;
  Long64_t sumsize(0);
  Long64_t sumfiles(0);

  int nstaging = getNstaging();
  int ntobestaged = getNtobestaged();
  int nproblem = getNproblem();
  int ntotal = getNtotal();

  list<string>::iterator itrb = _toBeStagedList.begin();
  for (; itrb!=_toBeStagedList.end(); ++itrb) {
    filename = *itrb;
    status = getStatusOf(filename.c_str(),false);
    if (_verbose) cout << "Status <" << filename << "> : " << status << endl;
  }
  
  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    filename = (itr->first);
    status = getStatusOf(filename.c_str(),false);
    if (_verbose) cout << "Status <" << filename << "> : " << status << endl;
    if (status == TStageFileInfo::RELEASED || status == TStageFileInfo::STAGED) {
      if (_verbose) { cout << (itr->second).statFile.st_atime << " " 
                           << (itr->second).statFile.st_mtime << " " 
                           << (itr->second).statFile.st_ctime << " " 
                           << (itr->second).statFile.st_size  << " " 
                           << endl; }
      sumsize += (itr->second).statFile.st_size ;
      sumfiles++;
    }
  }

  cout << "TStageManager::print() : " 
       << "staged: " << sumfiles << " (" << sumsize/(1024*1024) << " mb)"
       << " , staging: " << nstaging
       << " , to be staged: " << ntobestaged 
       << " , problems: " << nproblem
       << " , total: " << ntotal << " files."
       << endl;
}


void TStageManager::printProblemFiles()
{
  updateStatus();
  TStageFileInfo::Status status;
  string filename;

  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    filename = (itr->first);
    status = getStatusOf(filename.c_str(),false);

    if (status != TStageFileInfo::RELEASED && status != TStageFileInfo::STAGED && status != TStageFileInfo::STAGING) {
      cout << filename << endl;
    }
  }  
}


int TStageManager::getNtobestaged()
{
  int nentries( static_cast<int>(_toBeStagedList.size()) );
  if (_verbose) { cout << "TStageManager::getNtobestaged() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getNstaging()
{
  int nentries(0);
  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    if ((itr->second).status == TStageFileInfo::STAGING) nentries++;
  }
  if (_verbose) { cout << "TStageManager::getNstaging() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getNstaged()
{
  int nentries(0);
  TStageFileInfo::Status status;

  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    status = (itr->second).status;
    if ( status == TStageFileInfo::RELEASED || status == TStageFileInfo::STAGED ) nentries++;
  }

  if (_verbose) { cout << "TStageManager::getNstaged() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getSizeStaged()
{
  TStageFileInfo::Status status;
  string filename;
  Long64_t sumsize(0);

  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    filename = (itr->first);
    status = getStatusOf(filename.c_str(),false);
    if (status == TStageFileInfo::RELEASED || status == TStageFileInfo::STAGED) {
      sumsize += (itr->second).statFile.st_size ;
    }
  }
  int mbsize = static_cast<int>( sumsize/(1024*1024) );
  return mbsize;
}


int TStageManager::getNtotal()
{
  return ( this->getNtobestaged() + static_cast<int>( _stageMap.size() ) ) ;
}


int TStageManager::getNproblem()
{
  return ( getNtotal() - getNtobestaged() - getNstaging() - getNstaged() ) ;
}

void 
TStageManager::stageNext(bool forceStage) 
{
  if (_verbose) { cout << "TStageManager::stageNext()" << endl; }

  // update status
  updateStatus();

  // copy command given?
  assert(!_stagerInfo.cpcommand.empty());

  if (!_submittedStageMonitor) {
    submitStageMonitor();
    _submittedStageMonitor=true;
  }

  if (( (getNstaging()<_stagerInfo.pipeLength) || forceStage) && (!_toBeStagedList.empty()) ) {

    string cf = *(_toBeStagedList.begin());
    _toBeStagedList.erase(_toBeStagedList.begin());

    _stageMap[cf] = TStageFileInfo();
    _stageMap[cf].status = TStageFileInfo::STAGING;
    string inFile(cf); trim(inFile);
    removePrefixOf(inFile);
    _stageMap[cf].inFile = inFile;
    _stageMap[cf].outFile = getTmpFilename(cf.c_str());

    bool needToStageFile(true);

    // check if file already happens to exist from previous stage. If so, no need to stage.
    if (_checkForPreviousStage) {
      if (_verbose) cout << "TStageManager::stageNext() : checking for previous stage of <" << cf << ">." << endl;
      if ( fileExists(_stageMap[cf].outFile.c_str()) ) {
        if (_verbose) cout << "TStageManager::stageNext() : found previous stage of <" << cf << ">. Skip staging." << endl;
        _stageMap[cf].status = TStageFileInfo::STAGED;
        stat64(_stageMap[cf].outFile.c_str(),&(_stageMap[cf].statFile));
        needToStageFile = false ;
      }
    }

    // needToStageFile
    if (needToStageFile) {
      // useful output
      if (forceStage) 
        if (_verbose) cout << "TStageManager::stageNext() : forcing stage of <" << cf << ">." << endl;
      if (_verbose || _verbwait) { cout << "TStageManager::stageNext() : Now staging  <" << cf << ">." << endl; }
      if (_verbose) { cout << "TStageManager::stageNext() : outFile = <" << _stageMap[cf].outFile << ">." << endl; }
      if (_verbose) cout << "about to fork" << endl ;

      // do fork
      if( (_stageMap[cf].pid=fork()) == 0 ) {
        // Code only executed by child process

        if (_verbose) cout << "this is child process " << getpid() << " " 
	  		   << _stageMap[cf].pid
			   << endl ;

        if (_verbose) { cout << "child process : outFile = <" << _stageMap[cf].outFile << ">." << endl; }
        if (_verbose) { cout << "child process : tmpFile = <" << getTmpFilename(cf.c_str()) << ">." << endl; }

        _stageMap[cf].stout = _stageMap[cf].outFile + ( _pidInLognames ? Form("_stage%d.out",getpid()) : "_stage.out" );
        _stageMap[cf].sterr = _stageMap[cf].outFile + ( _pidInLognames ? Form("_stage%d.err",getpid()) : "_stage.err" );

        if (!_stagerInfo.logfileDir.empty()) {
          _stageMap[cf].stout = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[cf].stout.c_str());
          _stageMap[cf].sterr = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[cf].sterr.c_str());
        }

        if (!_verbose) freopen(_stageMap[cf].stout.c_str(),"w",stdout);
        if (!_verbose) freopen(_stageMap[cf].sterr.c_str(),"w",stderr);

        int nargs = 4 + int(_stagerInfo.cparg.size());
        const int argsc = 14; //(nargs); //= 4 + int(_stagerInfo.cparg.size());
        pchar args[argsc]; // = {"-r", "-t", "-l", (char *) 0 };

        for (int i=0; i<nargs-1; ++i) { 
          args[i] = new char[1024]; 
          //for (int j=0; j<1024; ++j) { args[i][j] = (char)0; }
        }

        strcpy(args[0],_stagerInfo.cpcommand.c_str());
        for (int i=1; i<=int(_stagerInfo.cparg.size()); ++i) 
	  strcpy(args[i],(_stagerInfo.cparg[i-1]).c_str());

        strcpy(args[nargs-3],_stageMap[cf].inFile.c_str());
    
        string outTmpfile = _stagerInfo.outfilePrefix + _stageMap[cf].outFile;
        strcpy(args[nargs-2],outTmpfile.c_str());
        strcpy(args[nargs-2],outTmpfile.c_str());

        if (_verbose) cout << "child processs <" << outTmpfile.c_str() << "> <" << args[nargs-2] << ">" << endl;

        args[nargs-1] = (char *) 0;
      
        if (_verbose)  {
          cout << "child processs is executing execv " 
	       << _stagerInfo.cpcommand.c_str() 
	       << " with args " ;
          for (int i=0; i<nargs-1; ++i) { if (args[i]) { cout << args[i] << " "; } }
          cout << endl ;
        }

        execvp(_stagerInfo.cpcommand.c_str(), args);      

        //_stageMap[cf].status = TStageFileInfo::STAGING;

        for (int i=0; i<nargs-1; ++i) { if (args[i]) delete[] args[i]; }
      } else {
        // Code only executed by parent process
        if (_verbose) { 
          cout << "this is parent process, pid of child = " 
	       << _stageMap[cf].pid 
	       << endl ; 
        }
        _stageMap[cf].stout = _stageMap[cf].outFile + Form("_stage%d.out",_stageMap[cf].pid);
        _stageMap[cf].sterr = _stageMap[cf].outFile + Form("_stage%d.err",_stageMap[cf].pid);

        if (!_stagerInfo.logfileDir.empty()) {
          _stageMap[cf].stout = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[cf].stout.c_str());
          _stageMap[cf].sterr = _stagerInfo.logfileDir + "/" + basename((char*)_stageMap[cf].sterr.c_str());
        }
      } // end of fork
    } // need to stage file
  } // end of stager loop

  // reupdate status
  //updateStatus();
}


void
TStageManager::updateStatus()
{
  if (_verbose) { cout << "TStageManager::updateStatus()" << endl; }

  map<string,TStageFileInfo>::iterator itr = _stageMap.begin();
  for (; itr!=_stageMap.end(); ++itr) {
    if ((itr->second).status == TStageFileInfo::STAGING) {

      pid_t pID = (itr->second).pid;

      int childExitStatus;
      waitpid( pID, &childExitStatus, WNOHANG);

      if( WIFEXITED(childExitStatus) ) {
	// done staging
	//(itr->second).status = TStageFileInfo::STAGED;
        if (_verbose) { cout << "waitpid() still staging. Returned with: Status= " 
	                     << WEXITSTATUS(childExitStatus)
                             << " and Exit= " 
                             << WIFEXITED(childExitStatus)
	                     << endl; }
      } else if( WIFSIGNALED(childExitStatus) ) {
	if (_verbose) { cout << "waitpid() still staging. Returned with: Signal= " 
	                     << WTERMSIG(childExitStatus)
                             << " and Exit= " 
                             << WIFSIGNALED(childExitStatus)
	                     << endl; }
	//(itr->second).status = TStageFileInfo::ERRORSTAGING; 
      } 
      // else file still staging
    }
  }
}


void 
TStageManager::submitStageMonitor()
{
  if (_verbose) { cout << "TStageManager::submitStageMonitor()" << endl; }
  if (_stagerInfo.stagemonitorcmd.empty()) return; 

  std::string whichstr = Form("which %s 2>/dev/null 1>/dev/null",_stagerInfo.stagemonitorcmd.c_str());
  int whichint = system( whichstr.c_str() ); // check for stagemonitor
  if (whichint!=0) {
    cout << "TStageManager::submitStageMonitor() : ERROR : <" << _stagerInfo.stagemonitorcmd 
         << "> not found in $PATH. Exit." 
         << endl;
    exit(1);
  }

  // pass parent pid to stagemonitor
  int ppid = getpid();

  int cpid(0);
  if( (cpid=fork()) == 0 ) {
    // Code only executed by child process
    
    if (_verbose) cout << "this is child process " << getpid() << endl ;
    
    pid_t pgid = setsid(); //setpgid(getpid(), getpid());
    if( pgid < 0) {
      cout << "Failed to set process group ID" << endl;
      _exit(0); // If exec fails then exit forked process.
    }

    const int nargs = 7;
    pchar args[nargs];    
    for (int i=0; i<nargs-1; ++i) { args[i] = new char[1024]; }
    
    strcpy(args[0],_stagerInfo.stagemonitorcmd.c_str());
    sprintf(args[1],"%d",ppid);
    strcpy(args[2],_stagerInfo.tmpdir.c_str());
    strcpy(args[3],_stagerInfo.baseTmpdir.c_str());
    if (_keepLogfiles) strcpy(args[4],"1");
    else strcpy(args[4],"0");
    if (_keepStagedfiles) strcpy(args[5],"1");
    else strcpy(args[5],"0");
    args[6]=(char *) 0;

    if (_verbose)  {
      cout << "child processs is executing execv " 
	   << _stagerInfo.stagemonitorcmd
	   << " with args " ;
      for (int i=0; i<nargs-1; ++i) { if (args[i]) { cout << args[i] << " "; } }
      cout << endl ;
    }
    
    execvp(_stagerInfo.stagemonitorcmd.c_str(), args);
    for (int i=0; i<nargs-1; ++i) { if (args[i]) delete[] args[i]; }

  } else {
    // Code only executed by parent process
    if (_verbose)
      cout << "this is parent process, pid of child = " << cpid << endl ; 
  }

}


void 
TStageManager::trim(string& input)
{
  // trim leading and trailing whitespace
  string::size_type position = input.find_first_not_of(" \t\n");
  if ( position == std::string::npos ) return; // skip, it's all whitespace
  input.erase(0, position);
  position= input.find_last_not_of(" \t\n");
  if ( position != std::string::npos)
    input.erase( position+1 ); 
}


void
TStageManager::removePrefixOf(string& filename)
{
  string::size_type ipsize(_stagerInfo.infilePrefix.size());
  string::size_type opsize(_stagerInfo.outfilePrefix.size());

  if (strncmp(filename.c_str(),_stagerInfo.infilePrefix.c_str(),ipsize)==0)
    filename = filename.substr(ipsize,filename.size()-ipsize);
  else if (strncmp(filename.c_str(),_stagerInfo.outfilePrefix.c_str(),opsize)==0)
    filename = filename.substr(opsize,filename.size()-opsize);  
}


void 
TStageManager::removeFile(string filename)
{
  if (filename.empty()) return;

  if (_verbose) cout << "TStageManager::removeFile() : " << filename << endl;

  if (remove(filename.c_str())==-1) {
    if (_verbose)
      cerr << "TStageManager::removeFile() Could not remove file: <" 
	   << filename
	   << ">."
	   << endl;    
  }
}


void TStageManager::setBaseTmpdir(const char* baseTmpdir) {
  string basedir = baseTmpdir;

  string::size_type position;
  while( (position=basedir.find_last_of("/")) == basedir.size() ) 
    basedir.replace(position,1,""); // remove any last slashes

  _stagerInfo.baseTmpdir = basedir;
  _stagerInfo.setTmpdir();
}


void TStageManager::setCpCommand(const char* cpcommand) {
  _stagerInfo.cpcommand = cpcommand;
  _stagerInfo.cparg.clear();
}


void TStageManager::addCpArg(const char* cpargm) {
  _stagerInfo.cparg.push_back(cpargm);
}

void TStageManager::setPipeLength(const int& pipeLength) {
  _stagerInfo.pipeLength = pipeLength;
}

void TStageManager::setInfilePrefix(const char* infilePrefix) {
  _stagerInfo.infilePrefix = infilePrefix;
}

void TStageManager::setOutfilePrefix(const char* outfilePrefix) {
  _stagerInfo.outfilePrefix = outfilePrefix;
}

void TStageManager::setLogfileDir(const char* logfileDir) {
  _stagerInfo.logfileDir = logfileDir;
}

void TStageManager::addPrefixFix(const char* in, const char* out) {
  if (_stagerInfo.inPrefixMap.find(in)==_stagerInfo.inPrefixMap.end())
    _stagerInfo.inPrefixMap[in] = out;
}

void TStageManager::fixRootInPrefix(string& tmpname)
{
  map<string,string>::iterator itr = _stagerInfo.inPrefixMap.begin();
  for (; itr!=_stagerInfo.inPrefixMap.end(); ++itr) {
    if(tmpname.find(itr->first)!=tmpname.npos) {
      tmpname.replace(tmpname.find(itr->first),(itr->first).size(),itr->second);
      break;
    }
  }
}

bool TStageManager::fileExists(const char* fileName)
{
  struct stat64 info;
  int ret = -1;
 
  //get the file attributes
  ret = stat64(fileName, &info);
  if(ret == 0) {
    //stat() is able to get the file attributes,
    //so the file obviously exists
    // if filesize==0 assume the copying failed.
    if (info.st_size == 0) return false;
    else return true;
  } else {
    //stat() is not able to get the file attributes,
    //so the file obviously does not exist or
    //more capabilities is required
    return false;
  }
}

