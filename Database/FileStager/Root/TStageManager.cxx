/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileStager/TStageManager.h"
#include "FileStager/TCopyFile.h"
#include <libgen.h>
#include <assert.h>


using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::map;


ClassImp(TStageManager)


TStagerInfo TStageManager::s_stagerInfo;


TStageManager::TStageManager() 
 : m_submittedStageMonitor(false)
 , m_verbose(false)
 , m_verbwait(false)
 , m_tryInfRestage(false)
 , m_firstFileANDAlreadyStaged(false)
 , m_keepLogfiles(false)
 , m_pidInLognames(true)
 , m_keepStagedfiles(false)
 , m_checkForPreviousStage(false)
{
  m_stageMap.clear();
  m_toBeStagedList.clear();
}


TStageManager::~TStageManager() 
{
  print();

  // terminating session, so cleanup
  //m_keepStagedfiles = false;
  releaseAll();

  // remove temporary stage directory
  if (s_stagerInfo.tmpdir.compare(s_stagerInfo.baseTmpdir)!=0)
    rmdir(s_stagerInfo.tmpdir.c_str());

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

  if (m_verbose) { cout << "TStageManager::addToList() : <" << input << ">" << endl; }

  if (m_firstFileANDAlreadyStaged) {
    m_stageMap[input] = TStageFileInfo();
    m_stageMap[input].status = TStageFileInfo::STAGED;
    string inFile(input);
    removePrefixOf(inFile);
    m_stageMap[input].inFile = inFile;
    m_stageMap[input].outFile = getTmpFilename(input.c_str());
    m_stageMap[input].stout = m_stageMap[input].outFile + "_stage.out";
    m_stageMap[input].sterr = m_stageMap[input].outFile + "_stage.err";
    if (!s_stagerInfo.logfileDir.empty()) {
      m_stageMap[input].stout = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[input].stout.c_str());
      m_stageMap[input].sterr = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[input].sterr.c_str());
    }
    m_firstFileANDAlreadyStaged = false; // next file
  }

  list<string>::iterator itrF = find(m_toBeStagedList.begin(),m_toBeStagedList.end(),input);
  if (m_stageMap.find(input)==m_stageMap.end()) {
    if (itrF==m_toBeStagedList.end())  m_toBeStagedList.push_back(input);
  } else if ( m_stageMap.find(input)->second.status != TStageFileInfo::STAGED && 
              m_stageMap.find(input)->second.status != TStageFileInfo::STAGING ) {
    // not interested. cleanup
    releaseFile(input.c_str()) ;
    m_stageMap.erase(m_stageMap.find(input));
    if (itrF==m_toBeStagedList.end())  m_toBeStagedList.push_back(input);
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

  list<string>::iterator itrF = find(m_toBeStagedList.begin(),m_toBeStagedList.end(),filename);
  if(itrF!=m_toBeStagedList.end()) {
    m_toBeStagedList.erase(itrF);
    return;
  }

  if(m_stageMap.find(filename)==m_stageMap.end()) return;

  if (m_stageMap[filename].status==TStageFileInfo::RELEASED ||
      m_stageMap[filename].status==TStageFileInfo::KILLEDSTAGING ||
      m_stageMap[filename].status==TStageFileInfo::ERRORRELEASED ||
      m_stageMap[filename].status==TStageFileInfo::KILLERROR) { 
    return;
  }

  if (m_verbose) cout << "TStageManager::releaseFile() : " << filename << endl;
    
  if (m_stageMap[filename].status==TStageFileInfo::STAGING) {
    // kill process first
    int  killReturn = kill(m_stageMap[filename].pid, SIGKILL);  

    if( killReturn == ESRCH) {     
      // pid does not exist
      if (m_verbose) cout << "Group does not exist!" << endl;
      // assume file is done staging
      // fill statistics first
      stat64(m_stageMap[filename].outFile.c_str(),&(m_stageMap[filename].statFile));
      m_stageMap[filename].status=TStageFileInfo::STAGED;
    } else if( killReturn == EPERM) { 
      // No permission to send kill signal
      // This should never happen
      if (m_verbose) cout << "No permission to send kill signal!" << endl;
      m_stageMap[filename].status=TStageFileInfo::KILLERROR;
    } else {
      if (m_verbose) cout << "Kill signal sent. All Ok!" << endl;
      m_stageMap[filename].status=TStageFileInfo::KILLEDSTAGING;
    }
  }

  if (m_stageMap[filename].status==TStageFileInfo::ERRORSTAGING) {
    m_stageMap[filename].status=TStageFileInfo::ERRORRELEASED;
  }

  if (m_stageMap[filename].status==TStageFileInfo::STAGED) {
    if (!m_keepStagedfiles) {
      removeFile(m_stageMap[filename].outFile);
      m_stageMap[filename].status=TStageFileInfo::RELEASED;
    }
  }

  if (!m_keepLogfiles) {
    removeFile(m_stageMap[filename].stout);
    removeFile(m_stageMap[filename].sterr);
  }
}


void TStageManager::releaseAll()
{
  m_toBeStagedList.clear();

  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); itr=m_stageMap.begin()) {
    string first = (itr->first);
    releaseFile(first.c_str());
    m_stageMap.erase(itr);
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

  if (m_verbose) cout << "TStageManager::getFile() : " << filename << endl;

  // first update status
  updateStatus();

  // file not found -> better start staging immediately.
  list<string>::iterator itrF = find(m_toBeStagedList.begin(),m_toBeStagedList.end(),filename);
  if (m_stageMap.find(filename)==m_stageMap.end() && 
      (itrF==m_toBeStagedList.end()) ) {
    if (m_verbose) cout << "TStageManager::getFile() : " << filename << " unknown. Start immediate staging." << endl;
    m_toBeStagedList.push_front(filename);
  }

  // prioritize file needs to be staged
  itrF = find(m_toBeStagedList.begin(),m_toBeStagedList.end(),filename);
  if (itrF!=m_toBeStagedList.end() && m_stageMap.find(filename)==m_stageMap.end()) {
    // move file to front
    m_toBeStagedList.erase(itrF);
    m_toBeStagedList.push_front(filename);

    if (m_verbose) cout << "TStageManager::getFile() : " << filename << ". Forced staging." << endl;
    stageNext(true); // forced stage right away
  }

  if (m_stageMap.find(filename)!=m_stageMap.end()) {
    if (m_verbose) cout << "TStageManager::getFile() : Checking staging status of " << filename << endl;

    // file still staging
    if (m_stageMap[filename].status==TStageFileInfo::STAGING) {

      // check status    
      pid_t pID = m_stageMap[filename].pid;
      int childExitStatus;
      
      // wait till staging is done
      if (m_verbose || m_verbwait) { cout << "TStageManager::getFile()   : Waiting till <"
		   	                << filename
			                << "> is staged."
			                << endl; }

      waitpid( pID, &childExitStatus, 0);
      
      if( !WIFEXITED(childExitStatus) ) {
	if (m_verbose) { cout << "waitpid() exited with status= " 
	                     << WEXITSTATUS(childExitStatus)
	                     << endl; }
	//m_stageMap[filename].status = TStageFileInfo::ERRORSTAGING;
      } else if( WIFSIGNALED(childExitStatus) ) {
	if (m_verbose) { cout << "waitpid() exited with signal: " 
	                     << WTERMSIG(childExitStatus)
	                     << endl; }
	//m_stageMap[filename].status = TStageFileInfo::ERRORSTAGING;
      } else {
	// child exited okay
	if (m_verbose) { cout << "waitpid() okay." << endl; }
	//m_stageMap[filename].status = TStageFileInfo::STAGED;
      }
    }

    // check if file _really_ exists. Sometimes staging returns empty files.
    bool fexists = fileExists(m_stageMap[filename].outFile.c_str());
    if (fexists) m_stageMap[filename].status = TStageFileInfo::STAGED;
    else {
      if (m_verbose) cout << "TStageManager::getFile() : ERROR : staging of file <" << filename << "> failed. Note: This could be an empty file or time-out." << endl;
    }

    // file is staged
    if (m_stageMap[filename].status == TStageFileInfo::STAGED) {
      if (m_verbose) cout << "TStageManager::getFile() : <" << filename << "> finished staging" << endl;

      if (stat64(m_stageMap[filename].outFile.c_str(),&(m_stageMap[filename].statFile)) < 0) {
        std::cerr << "ERROR: Cannot stat " << m_stageMap[filename].outFile << std::endl;
        std::abort();
      }
      // start next stage
      stageNext();
      return m_stageMap[filename].outFile.c_str();

    // file has already been run over ... 
    } else if (m_stageMap[filename].status == TStageFileInfo::RELEASED) {
      if (m_verbose) cout << "TStageManager::getFile() : WARNING : <" << filename << "> already run over!" << endl;
      /// Cleanup and let's try again.
      if (m_verbose) cout << "TStageManager::getFile() : restaging <" << filename << "> immediately." << endl;
      releaseFile(fname);
      m_stageMap.erase(m_stageMap.find(filename));
      return getFile(fname);

    // that's funny ... staged but no file!    
    } else { 

      if (m_tryInfRestage) { // false by default -- restaging done by wrapper script
	//Something went wrong. Cleanup and let's try again.
	if (m_verbose) cout << "TStageManager::getFile() : ERROR : restaging <" << filename << "> immediately." << endl;
	releaseFile(fname);
	m_stageMap.erase(m_stageMap.find(filename));
	return getFile(fname);
      }

      cout << "TStageManager::getFile() : ERROR : staging of <" << filename << "> failed (after N retries). Giving up." << endl;      
    }
  }

  // This should not be reached
  if (m_verbose) cout << "TStageManager::getFile() : ERROR <" << filename << "> not recognized." << endl;

  stageNext();

  return ""; // Let TFile handle error
}


const string
TStageManager::getTmpFilename(const char* filename)
{
  if (m_stageMap.find(filename)!=m_stageMap.end())
    if (!m_stageMap[filename].outFile.empty()) 
      return m_stageMap[filename].outFile.c_str();
  
  string infile(filename); trim(infile);
  string tmpfile(filename); trim(tmpfile);
  //removePrefixOf(infile);
  string::size_type pos = tmpfile.find_last_of("/");
  tmpfile = s_stagerInfo.tmpdir + "/tcf_" + /*string(Form("tcf%d_",getpid())) +*/ tmpfile.substr(pos+1,tmpfile.size()-pos-1);

  // double check!!
  string::size_type posi = infile.find_last_of("/");
  string first  = infile.substr(posi+1,infile.size()-posi-1);
  posi = tmpfile.find(first);
  tmpfile.erase( posi + first.size() );

  if (m_verbose) cout << "TStageManager::getTmpFilename() : <" << tmpfile << "> <" 
                     << tmpfile.c_str() << ">"
                     << endl;

  return tmpfile;
}


TStageFileInfo::Status
TStageManager::getStatusOf(const char* filename, bool update) 
{
  if (update) updateStatus();

  list<string>::iterator itrF = find(m_toBeStagedList.begin(),m_toBeStagedList.end(),filename);
  if(itrF!=m_toBeStagedList.end()) {
    return TStageFileInfo::TOBESTAGED;
  }

  if(m_stageMap.find(filename)!=m_stageMap.end()) 
    return m_stageMap[filename].status;

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

  list<string>::iterator itrb = m_toBeStagedList.begin();
  for (; itrb!=m_toBeStagedList.end(); ++itrb) {
    filename = *itrb;
    status = getStatusOf(filename.c_str(),false);
    if (m_verbose) cout << "Status <" << filename << "> : " << status << endl;
  }
  
  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
    filename = (itr->first);
    status = getStatusOf(filename.c_str(),false);
    if (m_verbose) cout << "Status <" << filename << "> : " << status << endl;
    if (status == TStageFileInfo::RELEASED || status == TStageFileInfo::STAGED) {
      if (m_verbose) { cout << (itr->second).statFile.st_atime << " " 
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

  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
    filename = (itr->first);
    status = getStatusOf(filename.c_str(),false);

    if (status != TStageFileInfo::RELEASED && status != TStageFileInfo::STAGED && status != TStageFileInfo::STAGING) {
      cout << filename << endl;
    }
  }  
}


int TStageManager::getNtobestaged()
{
  int nentries( static_cast<int>(m_toBeStagedList.size()) );
  if (m_verbose) { cout << "TStageManager::getNtobestaged() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getNstaging()
{
  int nentries(0);
  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
    if ((itr->second).status == TStageFileInfo::STAGING) nentries++;
  }
  if (m_verbose) { cout << "TStageManager::getNstaging() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getNstaged()
{
  int nentries(0);
  TStageFileInfo::Status status;

  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
    status = (itr->second).status;
    if ( status == TStageFileInfo::RELEASED || status == TStageFileInfo::STAGED ) nentries++;
  }

  if (m_verbose) { cout << "TStageManager::getNstaged() = " << nentries << endl; }
  return nentries;
}


int TStageManager::getSizeStaged()
{
  TStageFileInfo::Status status;
  string filename;
  Long64_t sumsize(0);

  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
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
  return ( this->getNtobestaged() + static_cast<int>( m_stageMap.size() ) ) ;
}


int TStageManager::getNproblem()
{
  return ( getNtotal() - getNtobestaged() - getNstaging() - getNstaged() ) ;
}

void 
TStageManager::stageNext(bool forceStage) 
{
  if (m_verbose) { cout << "TStageManager::stageNext()" << endl; }

  // update status
  updateStatus();

  // copy command given?
  assert(!s_stagerInfo.cpcommand.empty());

  if (!m_submittedStageMonitor) {
    submitStageMonitor();
    m_submittedStageMonitor=true;
  }

  if (( (getNstaging()<s_stagerInfo.pipeLength) || forceStage) && (!m_toBeStagedList.empty()) ) {

    string cf = *(m_toBeStagedList.begin());
    m_toBeStagedList.erase(m_toBeStagedList.begin());

    m_stageMap[cf] = TStageFileInfo();
    m_stageMap[cf].status = TStageFileInfo::STAGING;
    string inFile(cf); trim(inFile);
    removePrefixOf(inFile);
    m_stageMap[cf].inFile = inFile;
    m_stageMap[cf].outFile = getTmpFilename(cf.c_str());

    bool needToStageFile(true);

    // check if file already happens to exist from previous stage. If so, no need to stage.
    if (m_checkForPreviousStage) {
      if (m_verbose) cout << "TStageManager::stageNext() : checking for previous stage of <" << cf << ">." << endl;
      if ( fileExists(m_stageMap[cf].outFile.c_str()) ) {
        if (m_verbose) cout << "TStageManager::stageNext() : found previous stage of <" << cf << ">. Skip staging." << endl;
        m_stageMap[cf].status = TStageFileInfo::STAGED;
        if (stat64(m_stageMap[cf].outFile.c_str(),&(m_stageMap[cf].statFile)) < 0) {
          std::cerr << "ERROR: Cannot stat " << m_stageMap[cf].outFile << std::endl;
          std::abort();
        }
        needToStageFile = false ;
      }
    }

    // needToStageFile
    if (needToStageFile) {
      // useful output
      if (forceStage) 
        if (m_verbose) cout << "TStageManager::stageNext() : forcing stage of <" << cf << ">." << endl;
      if (m_verbose || m_verbwait) { cout << "TStageManager::stageNext() : Now staging  <" << cf << ">." << endl; }
      if (m_verbose) { cout << "TStageManager::stageNext() : outFile = <" << m_stageMap[cf].outFile << ">." << endl; }
      if (m_verbose) cout << "about to fork" << endl ;

      // do fork
      if( (m_stageMap[cf].pid=fork()) == 0 ) {
        // Code only executed by child process

        if (m_verbose) cout << "this is child process " << getpid() << " " 
	  		   << m_stageMap[cf].pid
			   << endl ;

        if (m_verbose) { cout << "child process : outFile = <" << m_stageMap[cf].outFile << ">." << endl; }
        if (m_verbose) { cout << "child process : tmpFile = <" << getTmpFilename(cf.c_str()) << ">." << endl; }

        m_stageMap[cf].stout = m_stageMap[cf].outFile + ( m_pidInLognames ? Form("_stage%d.out",getpid()) : "_stage.out" );
        m_stageMap[cf].sterr = m_stageMap[cf].outFile + ( m_pidInLognames ? Form("_stage%d.err",getpid()) : "_stage.err" );

        if (!s_stagerInfo.logfileDir.empty()) {
          m_stageMap[cf].stout = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[cf].stout.c_str());
          m_stageMap[cf].sterr = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[cf].sterr.c_str());
        }

        if (!m_verbose) freopen(m_stageMap[cf].stout.c_str(),"w",stdout);
        if (!m_verbose) freopen(m_stageMap[cf].sterr.c_str(),"w",stderr);

        int nargs = 4 + int(s_stagerInfo.cparg.size());
        const int argsc = 14; //(nargs); //= 4 + int(s_stagerInfo.cparg.size());
        pchar args[argsc]; // = {"-r", "-t", "-l", (char *) 0 };

        for (int i=0; i<nargs-1; ++i) { 
          args[i] = new char[1024]; 
          //for (int j=0; j<1024; ++j) { args[i][j] = (char)0; }
        }

        strcpy(args[0],s_stagerInfo.cpcommand.c_str());
        for (int i=1; i<=int(s_stagerInfo.cparg.size()); ++i) 
	  strcpy(args[i],(s_stagerInfo.cparg[i-1]).c_str());

        strcpy(args[nargs-3],m_stageMap[cf].inFile.c_str());
    
        string outTmpfile = s_stagerInfo.outfilePrefix + m_stageMap[cf].outFile;
        strcpy(args[nargs-2],outTmpfile.c_str());
        strcpy(args[nargs-2],outTmpfile.c_str());

        if (m_verbose) cout << "child processs <" << outTmpfile.c_str() << "> <" << args[nargs-2] << ">" << endl;

        args[nargs-1] = (char *) 0;
      
        if (m_verbose)  {
          cout << "child processs is executing execv " 
	       << s_stagerInfo.cpcommand.c_str() 
	       << " with args " ;
          for (int i=0; i<nargs-1; ++i) { if (args[i]) { cout << args[i] << " "; } }
          cout << endl ;
        }

        execvp(s_stagerInfo.cpcommand.c_str(), args);      

        //m_stageMap[cf].status = TStageFileInfo::STAGING;

        for (int i=0; i<nargs-1; ++i) { if (args[i]) delete[] args[i]; }
      } else {
        // Code only executed by parent process
        if (m_verbose) { 
          cout << "this is parent process, pid of child = " 
	       << m_stageMap[cf].pid 
	       << endl ; 
        }
        m_stageMap[cf].stout = m_stageMap[cf].outFile + Form("_stage%d.out",m_stageMap[cf].pid);
        m_stageMap[cf].sterr = m_stageMap[cf].outFile + Form("_stage%d.err",m_stageMap[cf].pid);

        if (!s_stagerInfo.logfileDir.empty()) {
          m_stageMap[cf].stout = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[cf].stout.c_str());
          m_stageMap[cf].sterr = s_stagerInfo.logfileDir + "/" + basename((char*)m_stageMap[cf].sterr.c_str());
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
  if (m_verbose) { cout << "TStageManager::updateStatus()" << endl; }

  map<string,TStageFileInfo>::iterator itr = m_stageMap.begin();
  for (; itr!=m_stageMap.end(); ++itr) {
    if ((itr->second).status == TStageFileInfo::STAGING) {

      pid_t pID = (itr->second).pid;

      int childExitStatus;
      waitpid( pID, &childExitStatus, WNOHANG);

      if( WIFEXITED(childExitStatus) ) {
	// done staging
	//(itr->second).status = TStageFileInfo::STAGED;
        if (m_verbose) { cout << "waitpid() still staging. Returned with: Status= " 
	                     << WEXITSTATUS(childExitStatus)
                             << " and Exit= " 
                             << WIFEXITED(childExitStatus)
	                     << endl; }
      } else if( WIFSIGNALED(childExitStatus) ) {
	if (m_verbose) { cout << "waitpid() still staging. Returned with: Signal= " 
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
  if (m_verbose) { cout << "TStageManager::submitStageMonitor()" << endl; }
  if (s_stagerInfo.stagemonitorcmd.empty()) return; 

  std::string whichstr = Form("which %s 2>/dev/null 1>/dev/null",s_stagerInfo.stagemonitorcmd.c_str());
  int whichint = system( whichstr.c_str() ); // check for stagemonitor
  if (whichint!=0) {
    cout << "TStageManager::submitStageMonitor() : ERROR : <" << s_stagerInfo.stagemonitorcmd 
         << "> not found in $PATH. Exit." 
         << endl;
    exit(1);
  }

  // pass parent pid to stagemonitor
  int ppid = getpid();

  int cpid(0);
  if( (cpid=fork()) == 0 ) {
    // Code only executed by child process
    
    if (m_verbose) cout << "this is child process " << getpid() << endl ;
    
    pid_t pgid = setsid(); //setpgid(getpid(), getpid());
    if( pgid < 0) {
      cout << "Failed to set process group ID" << endl;
      _exit(0); // If exec fails then exit forked process.
    }

    const int nargs = 7;
    pchar args[nargs];    
    for (int i=0; i<nargs-1; ++i) { args[i] = new char[1024]; }
    
    strcpy(args[0],s_stagerInfo.stagemonitorcmd.c_str());
    sprintf(args[1],"%d",ppid);
    strcpy(args[2],s_stagerInfo.tmpdir.c_str());
    strcpy(args[3],s_stagerInfo.baseTmpdir.c_str());
    if (m_keepLogfiles) strcpy(args[4],"1");
    else strcpy(args[4],"0");
    if (m_keepStagedfiles) strcpy(args[5],"1");
    else strcpy(args[5],"0");
    args[6]=(char *) 0;

    if (m_verbose)  {
      cout << "child processs is executing execv " 
	   << s_stagerInfo.stagemonitorcmd
	   << " with args " ;
      for (int i=0; i<nargs-1; ++i) { if (args[i]) { cout << args[i] << " "; } }
      cout << endl ;
    }
    
    execvp(s_stagerInfo.stagemonitorcmd.c_str(), args);
    for (int i=0; i<nargs-1; ++i) { if (args[i]) delete[] args[i]; }

  } else {
    // Code only executed by parent process
    if (m_verbose)
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
  string::size_type ipsize(s_stagerInfo.infilePrefix.size());
  string::size_type opsize(s_stagerInfo.outfilePrefix.size());

  if (strncmp(filename.c_str(),s_stagerInfo.infilePrefix.c_str(),ipsize)==0)
    filename = filename.substr(ipsize,filename.size()-ipsize);
  else if (strncmp(filename.c_str(),s_stagerInfo.outfilePrefix.c_str(),opsize)==0)
    filename = filename.substr(opsize,filename.size()-opsize);  
}


void 
TStageManager::removeFile(string filename)
{
  if (filename.empty()) return;

  if (m_verbose) cout << "TStageManager::removeFile() : " << filename << endl;

  if (remove(filename.c_str())==-1) {
    if (m_verbose)
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

  s_stagerInfo.baseTmpdir = basedir;
  s_stagerInfo.setTmpdir();
}


void TStageManager::setCpCommand(const char* cpcommand) {
  s_stagerInfo.cpcommand = cpcommand;
  s_stagerInfo.cparg.clear();
}


void TStageManager::addCpArg(const char* cpargm) {
  s_stagerInfo.cparg.push_back(cpargm);
}

void TStageManager::setPipeLength(const int& pipeLength) {
  s_stagerInfo.pipeLength = pipeLength;
}

void TStageManager::setInfilePrefix(const char* infilePrefix) {
  s_stagerInfo.infilePrefix = infilePrefix;
}

void TStageManager::setOutfilePrefix(const char* outfilePrefix) {
  s_stagerInfo.outfilePrefix = outfilePrefix;
}

void TStageManager::setLogfileDir(const char* logfileDir) {
  s_stagerInfo.logfileDir = logfileDir;
}

void TStageManager::addPrefixFix(const char* in, const char* out) {
  if (s_stagerInfo.inPrefixMap.find(in)==s_stagerInfo.inPrefixMap.end())
    s_stagerInfo.inPrefixMap[in] = out;
}

void TStageManager::fixRootInPrefix(string& tmpname)
{
  map<string,string>::iterator itr = s_stagerInfo.inPrefixMap.begin();
  for (; itr!=s_stagerInfo.inPrefixMap.end(); ++itr) {
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

