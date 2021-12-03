/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  AthenaSummarySvc.cxx
 *  AthenaSummarySvc
 *
 *  Author: Charles Leggett
 *
 *  Provides summary at end of athena job
 *
 *****************************************************************************/

#include "AthenaSummarySvc.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/System.h"

#include "PerfMonKernel/IPerfMonSvc.h"
#include "PerfMonEvent/DataModel.h"
#include "AthenaKernel/ILoggedMessageSvc.h"

#include <fstream>
#include <unistd.h>
#include <exception>
#include <sstream>
#include <ctype.h>

static const std::string levelNames[MSG::NUM_LEVELS] = {"NIL",     "VERBOSE", "DEBUG", "INFO",
                                                        "WARNING", "ERROR",   "FATAL", "ALWAYS"};

using namespace std;

char* AthenaSummarySvc::s_block = nullptr;
bool  AthenaSummarySvc::s_badalloc = false;
const char* const II("\001");


//
///////////////////////////////////////////////////////////////////////////
//

class PD {
public:
  PD(){};
  PD(const string& a, const string& b) { add(a,b); }
  
  void add(const string& a, const char* b) {
    add(a,(string)b);
  }
  void add(const string& a, const string& b) {
    m_dat[a].push_back( b );
  }
  void add(const string& a, const PD& p) { 
    m_dat[a].push_back(  p.dump() );
  }
  template <typename T>
  void add(const string& a, const T t) {
    ostringstream ofs;
    m_ofs.clear();
    m_ofs.str("");

    m_ofs << II << t;

    m_dat[a].push_back( m_ofs.str() );
  }

  operator const string () const
  {
    return dump();
  }

  string dump() const {
    string x("{");
    map<string,vector<string> >::const_iterator itr;
    unsigned int iv;
    for (itr=m_dat.begin(); itr != m_dat.end(); ++itr) {
      if (x.length() > 1) { x+= ','; }
      x += "\"" + itr->first + "\":";
      vector<string> v = itr->second;
      if (v.size() > 1) { x += '['; }
      for (iv = 0; iv < v.size(); ++iv) {
	if (iv > 0) { x += ','; }
	if (v[iv][0] == '{') {
	  x += v[iv];
	} else if (v[iv].compare(0,1, II)==0) {
	  x.append( v[iv], 1,v[iv].length());
	} else {
	  x += "\"" + v[iv] + "\"";
	}
      }
      if (v.size() > 1) { x += ']'; }
    }
    x += '}';
    return x;
  }


private:

  map<string, vector<string> > m_dat;
  static ostringstream m_ofs ATLAS_THREAD_SAFE;
  
};

ostringstream PD::m_ofs("nothing");

inline void tolower(std::string &s)
{
  // cf https://en.cppreference.com/w/cpp/string/byte/tolower
  std::transform(s.begin(), s.end(), s.begin(), 
                 [](unsigned char c){ return std::tolower(c); } );
}
	 
//
///////////////////////////////////////////////////////////////////////////
//

AthenaSummarySvc::AthenaSummarySvc( const std::string& name, ISvcLocator* svc )
  : base_class( name, svc ),
    p_incSvc("IncidentSvc",name),
    m_new (std::set_new_handler( &AthenaSummarySvc::newHandler ))
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode AthenaSummarySvc::initialize() {

  ATH_MSG_DEBUG("Initializing AthenaSummarySvc");

  int pri=100;
  p_incSvc->addListener( this, "BeginInputFile", pri, true);
  p_incSvc->addListener( this, "EndInputFile", pri, true);
  p_incSvc->addListener( this, "BeginOutputFile", pri, true);
  p_incSvc->addListener( this, "FailOutputFile", pri, true);
  p_incSvc->addListener( this, "WroteToOutputFile", pri, true);
  p_incSvc->addListener( this, "EndOutputFile", pri, true);

  p_incSvc->addListener( this, "AbortEvent", pri, true);

  p_incSvc->addListener( this, "BeginEvent", pri, true);
  p_incSvc->addListener( this, "EndEvent", pri, true);
  p_incSvc->addListener( this, "BeginRun", pri, true);
  p_incSvc->addListener( this, "EndRun", pri, true);

  p_incSvc->addListener( this, "FirstInputFile", pri, true );

  vector<string>::const_iterator itr;
  for (itr=m_extraInc.value().begin(); itr != m_extraInc.value().end(); ++itr) {
    ATH_MSG_DEBUG("Tracking incident \"" << *itr << "\"");
    addListener(*itr);
  }

  p_logMsg = dynamic_cast< ILoggedMessageSvc* > ( msgSvc().get() );
  if (p_logMsg == nullptr) {
    ATH_MSG_INFO("unable dcast IMessageSvc to ILoggedMessageSvc: "
                 "not scanning for keywords in logs, or printing logged messages");
      } else {

    if (m_keywords.value().size() > 0) {
      IProperty *ip = dynamic_cast<IProperty*>( p_logMsg );
      if (ip != nullptr) {
        if (ip->setProperty(m_keywords).isFailure()) {
          ATH_MSG_ERROR("could not set keywords property of LoggedMessageSvc");
        } else {
          ATH_MSG_INFO("Scanning log for keyword \"" << m_keywords
                       << "\". CAVEAT EMPTOR - THIS IS VERY SLOW!!");
        }
      } else {
        ATH_MSG_ERROR("could not dcast LoggedMessageSvc to IProperty");
      }
    }
  }

  std::string fmt = m_summaryFormat.value();
  tolower(fmt);
  m_summaryFormat = fmt;

  // save some space for the summary output if we run out of memory
  ATH_MSG_DEBUG("allocating block of 100 pages");
  s_block = new char[ sysconf( _SC_PAGESIZE ) * 100 ];


  return StatusCode(s_block!=nullptr);

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode AthenaSummarySvc::reinitialize() {

  delete[] s_block; s_block = nullptr;
  s_block = new char[ sysconf( _SC_PAGESIZE ) * 100 ];
  return s_block ? StatusCode::SUCCESS : StatusCode::FAILURE;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode AthenaSummarySvc::finalize() {

  createSummary().ignore();

  // cleanup
  delete[] s_block; s_block = nullptr;
  std::set_new_handler( m_new );

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
AthenaSummarySvc::addListener( const std::string& inc ) {

  ATH_MSG_DEBUG("now listening to incident " << inc);

  if (m_extraIncidents.find( inc ) == m_extraIncidents.end()) {
    p_incSvc->addListener( this, inc, 100, true);
    m_extraIncidents[inc] = map<string,int>();
  } else {
    ATH_MSG_INFO("already listening to Incident " << inc);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
AthenaSummarySvc::addSummary(const std::string& dict, const std::string& info) {

  ATH_MSG_DEBUG("adding extra info: " << dict << "/" << info);

  m_extraInfo.push_back( make_pair(dict,info) );

}
  

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
AthenaSummarySvc::newHandler() {

  s_badalloc = true;

  if ( ! s_block )
    throw std::bad_alloc();   // default behavior (no print-out)

  // release our block to create working space for finalize()
  delete[] s_block; s_block = nullptr;
  
  // print onto std::cerr rather than MsgStream, as it's more innocuous
  std::cerr << "AthenaSummarySvc     FATAL out of memory: saving summary ..."
            << std::endl;

  IAthenaSummarySvc *ipa(nullptr);
  Gaudi::svcLocator()->service("AthenaSummarySvc",ipa).ignore();

  if (ipa != nullptr) {    
    std::string btrace;
    if ( System::backTrace(btrace,5,3) ) {
      ipa->addSummary("badalloc backtrace",btrace);
    }

    ipa->setStatus(99);
    ipa->createSummary().ignore();
  } else {
    std::cerr << "AthenaSummarySvc    ERROR unable to get hold of myself and print summary"
	      << std::endl;
  }

    
  // in order to abort the job, throw a bad_alloc, which should make its way
  // through to python, even if the ExceptionSvc is used, and that will shutdown
  // (finalize/terminate) the application on exit
  throw std::bad_alloc();

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
AthenaSummarySvc::handle(const Incident &inc) {

  ATH_MSG_DEBUG("handle incident: " << inc.type() << " " << inc.source());

  string fileName;

  const FileIncident *fi = dynamic_cast<const FileIncident*>( &inc );
  if (fi != nullptr) {
    // FIXME!!! waiting on AthenaPoolKernel-00-00-07
    ATH_MSG_INFO(" -> file incident: " << fi->fileName() << " [GUID: " << fi->fileGuid() << "]");
    fileName = fi->fileName();
  } else {
    fileName = inc.source();
  }

  if (inc.type() == "BeginInputFile" ) {
    m_inputFilesRead.push_back( fileName );
  } else if (inc.type() == "BeginOutputFile") {
    m_outputFiles.push_back( fileName );
  } else if (inc.type() == "FailOutputFile") {
    m_outputFilesError.push_back( fileName );
  } else if (inc.type() == "BeginEvent") {
    m_eventsRead ++;
  } else if (inc.type() == "SkipEvent") {
    m_eventsSkipped ++;
  } else if (inc.type() == "WriteEvent") {
    m_eventsWritten ++;
  } else if (inc.type() == "BeginRun") {
    m_runs ++;
  } else if (inc.type() == "EndRun") {
  }

  map<std::string, map<string,int> >::iterator itr 
    ( m_extraIncidents.find(inc.type()) );

  if (itr != m_extraIncidents.end()) {
    map<string,int>::iterator it = itr->second.find(inc.source());
    if (it != itr->second.end()) {
      m_extraIncidents[inc.type()][inc.source()]++;
    } else {
      m_extraIncidents[inc.type()][inc.source()] = 1;
    }
  }

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
AthenaSummarySvc::createSummary() {

  ATH_MSG_DEBUG("createSummary");

  std::ofstream ofs;
  ofs.open(m_summaryFile.value().c_str());
  if (!ofs) {
    ATH_MSG_ERROR("Unable to open output file \"" << m_summaryFile.value() << "\"");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Writing to \"" << m_summaryFile.value() << "\"");

  if (m_summaryFormat.value() == "ascii" || m_summaryFormat.value() == "both") {
    createASCII(ofs);
  }

  if (m_summaryFormat.value() == "python" || m_summaryFormat.value() == "both") {
    createDict(ofs);
  }

  ofs.close();

  return StatusCode::SUCCESS;


}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
AthenaSummarySvc::createASCII( std::ofstream& ofs ) {
  
  ATH_MSG_DEBUG("createASCII");

  list<string>::const_iterator itr;
  
  ofs << "Files read: " << m_inputFilesRead.size() << std::endl;
  for (itr=m_inputFilesRead.begin(); itr != m_inputFilesRead.end(); ++itr) {
    ofs << "  " << *itr << endl;
  }
  
  ofs << "Files written: " << m_outputFiles.size() << std::endl;
  for (itr=m_outputFiles.begin(); itr != m_outputFiles.end(); ++itr) {
    ofs << "  " << *itr << endl;
  }
  
  ofs << "File Write Error: " << m_outputFilesError.size() << std::endl;
  for (itr=m_outputFilesError.begin(); itr != m_outputFilesError.end(); ++itr) {
    ofs << "  " << *itr << endl;
  }
  
  ofs << "Events Read:    " << m_eventsRead << endl;
  ofs << "Events Written: " << m_eventsWritten << endl;
  ofs << "Events Skipped: " << m_eventsSkipped << endl;

  ofs << "Runs: " << m_runs << endl;
  

  ofs << "Message Count: " << endl;
  ofs << " FATAL:   " << msgSvc()->messageCount( MSG::FATAL ) << endl;
  ofs << " ERROR:   " << msgSvc()->messageCount( MSG::ERROR ) << endl;
  ofs << " WARNING: " << msgSvc()->messageCount( MSG::WARNING ) << endl;
  ofs << " INFO:    " << msgSvc()->messageCount( MSG::INFO ) << endl;

  if (p_logMsg != nullptr) {

    IntegerProperty thresh("loggingLevel",MSG::VERBOSE);
    IProperty *ip = dynamic_cast<IProperty*>( p_logMsg );
    if (ip != nullptr) {
      if (ip->getProperty(&thresh).isFailure()) {
        ATH_MSG_ERROR("could not get loggingLevel property of LoggedMessageSvc");
      }
    } else {
      ATH_MSG_ERROR("could not dcast LoggedMessageSvc to IProperty");
    }
      
	

    ofs << "Message Log: " << endl;
    vector<pair<string,string> > msgs;
    vector<pair<string,string> >::const_iterator mitr;
    for (unsigned int l=thresh.value(); l < MSG::ALWAYS; l++) {
      ofs << "  " << levelNames[l];
      msgs = p_logMsg->getMessages( MSG::Level(l) );
      ofs << "  " << msgs.size() << endl;
      for (mitr=msgs.begin(); mitr != msgs.end(); ++mitr) {
	ofs << "      " << mitr->first << " : " << mitr->second << endl;
      }
    }

    ofs << "Keyword tracked messages: " << endl;
    vector<ILoggedMessageSvc::LoggedMessage>::const_iterator ilm = p_logMsg->getKeyMessages().begin();
    for (;ilm != p_logMsg->getKeyMessages().end(); ++ilm) {
      ofs << "  " << levelNames[ilm->level] << "  " << ilm->source << "  " << ilm->message
	  << endl;
    }
    
  }

  if (m_extraInfo.size() > 0) {
    ofs << "Extra Summary Info:" << endl;
    vector<pair<string,string> >::const_iterator itr (m_extraInfo.begin() );
    for (; itr != m_extraInfo.end(); ++itr) {
      ofs << " " << itr->first << " : " << itr->second << endl;
    }
  }
  
  if (m_extraIncidents.size() > 0) {
    ofs << "Extra Incident Counts:" << endl;
    map<string, map<string,int> >::const_iterator itr(m_extraIncidents.begin());
    for (; itr != m_extraIncidents.end(); ++itr) {
      ofs << " " << itr->first;
      if (itr->second.begin() == itr->second.end()) {
	ofs << " : 0" << endl;
      } else {
	for (map<string,int>::const_iterator it=itr->second.begin();
	     it != itr->second.end(); ++it) {
	  ofs << " ::  " << it->first << ":" << it->second;
	}
	ofs << endl;
      }
    }
  }

  // Get Perfmon data
  IPerfMonSvc *ipms;
  if (service("PerfMonSvc",ipms,false).isFailure()) {
    ATH_MSG_DEBUG("unable to get the PerfMonSvc: not printing perf summaries");
  } else {

    ofs << "Monitored Components:" << endl;
    vector<string>::const_iterator itr;

    vector<string> cmp_i = ipms->components("ini");
    vector<string> cmp_e = ipms->components("exe");
    vector<string> cmp_f = ipms->components("fin");

    for (itr=cmp_i.begin(); itr!=cmp_i.end(); ++itr) {
      ofs << "  " << *itr << endl;
    }
  }

  if (s_badalloc) {
    ofs << "std::bad_alloc caught: out of memory condition detected" 
	<< endl;
  }

  ofs << "Exit Status: " << m_status << endl;


}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
AthenaSummarySvc::createDict( std::ofstream& ofd) {

  ATH_MSG_DEBUG("createDict");

  list<string>::const_iterator itr;

  PD p;

  PD files;
  string f;
  for (itr=m_inputFilesRead.begin(); itr != m_inputFilesRead.end(); ++itr) {
    if (f.length() > 0) { f += ","; }
    f += *itr;
  }
  files.add("read",f);

  f.clear();
  for (itr=m_outputFiles.begin(); itr != m_outputFiles.end(); ++itr) {
    if (f.length() > 0) { f += ","; }
    f += *itr;    
  }
  files.add("write",f);
  
  f.clear();
  for (itr=m_outputFilesError.begin(); itr != m_outputFilesError.end(); ++itr) {
    if (f.length() > 0) { f += ","; }
    f += *itr;    
  }
  files.add("write error",f);
  
  p.add("files",files);

  PD events;
  events.add("read",m_eventsRead);
  events.add("write",m_eventsWritten);
  events.add("skip",m_eventsSkipped);

  p.add("events",events);

  p.add("runs",m_runs);

  PD msg;
  msg.add("FATAL",msgSvc()->messageCount( MSG::FATAL ));
  msg.add("ERROR",msgSvc()->messageCount( MSG::ERROR ));
  msg.add("WARNING",msgSvc()->messageCount( MSG::WARNING ));
  msg.add("INFO",msgSvc()->messageCount( MSG::INFO ));

  p.add("message count",msg);

  if (p_logMsg != nullptr) {

    IntegerProperty thresh("loggingLevel",MSG::VERBOSE);
    IProperty *ip = dynamic_cast<IProperty*>( p_logMsg );
    if (ip != nullptr) {
      if (ip->getProperty(&thresh).isFailure()) {
        ATH_MSG_ERROR("could not get loggingLevel property of LoggedMessageSvc");
      }
    } else {
      ATH_MSG_ERROR("could not dcast LoggedMessageSvc to IProperty");
    }
    
    PD mlog;
    vector<pair<string,string> > msgs;
    vector<pair<string,string> >::const_iterator mitr;
    for (unsigned int l=thresh.value(); l < MSG::ALWAYS; l++) {
      PD slog;
      msgs = p_logMsg->getMessages( MSG::Level(l) );
      for (mitr=msgs.begin(); mitr != msgs.end(); ++mitr) {
	slog.add(mitr->first, mitr->second);
      }
      mlog.add(levelNames[l],slog);      
    }

    p.add("messages",mlog);
    
  }

  if (m_extraInfo.size() > 0) {    
    PD user;
    vector<pair<string,string> >::const_iterator itr (m_extraInfo.begin() );
    for (; itr != m_extraInfo.end(); ++itr) {
      string dat = itr->second;
      while( dat.find("\n") != string::npos) {
	dat.erase(dat.find("\n"),1);	
      }
      user.add(itr->first, dat);
    }
    p.add("user data",user);
  }
  
  if (m_extraIncidents.size() > 0) {
    PD inc;
    map<string, map<string,int> >::const_iterator itr(m_extraIncidents.begin());
    for (; itr != m_extraIncidents.end(); ++itr) {
      if (itr->second.begin() == itr->second.end()) {
	inc.add(itr->first,0);
      } else {
	PD inc2;
	for (map<string,int>::const_iterator it=itr->second.begin();
	     it != itr->second.end(); ++it) {
	  inc2.add(it->first, it->second);
	}
	inc.add(itr->first, inc2);
      }
    }

//     PD inc;
//     map<string, int>::const_iterator itr(m_extraIncidents.begin());
//     for (; itr != m_extraIncidents.end(); ++itr) {
//       inc.add(itr->first, itr->second);
//     }

    p.add("extra incidents",inc);
  }

  // Get Perfmon data
  IPerfMonSvc *ipms;
  if (service("PerfMonSvc",ipms,false).isFailure()) {
    ATH_MSG_DEBUG("unable to get the PerfMonSvc: not printing perf summaries");
  } else {

    PD mon,ini,exe,fin;
    vector<string>::const_iterator itr;

    vector<string> cmp_i = ipms->components("ini");
    vector<string> cmp_e = ipms->components("exe");
    vector<string> cmp_f = ipms->components("fin");

    for (itr=cmp_i.begin(); itr!=cmp_i.end(); ++itr) {
      const PerfMon::Component* cmp = ipms->component("ini",*itr);
      PD prf;
      prf.add("cpu",cmp->cpu.user);
      prf.add("cpu",cmp->cpu.sys);
      prf.add("cpu",cmp->cpu.real);
      prf.add("mem",cmp->mem.dVmem());
      prf.add("mem",cmp->mem.dRss());
      ini.add(*itr,prf);
    }
    for (itr=cmp_e.begin(); itr!=cmp_e.end(); ++itr) {
      const PerfMon::Component* cmp = ipms->component("exe",*itr);
      PD prf;
      prf.add("cpu",cmp->cpu.user);
      prf.add("cpu",cmp->cpu.sys);
      prf.add("cpu",cmp->cpu.real);
      prf.add("mem",cmp->mem.dVmem());
      prf.add("mem",cmp->mem.dRss());
      exe.add(*itr,prf);
    }
    for (itr=cmp_f.begin(); itr!=cmp_f.end(); ++itr) {
      const PerfMon::Component* cmp = ipms->component("fin",*itr);
      PD prf;
      prf.add("cpu",cmp->cpu.user);
      prf.add("cpu",cmp->cpu.sys);
      prf.add("cpu",cmp->cpu.real);
      prf.add("mem",cmp->mem.dVmem());
      prf.add("mem",cmp->mem.dRss());
      fin.add(*itr,prf);
    }
    mon.add("ini",ini);
    mon.add("fin",fin);
    mon.add("exe",exe);

    p.add("monitored components",mon);

  }

  p.add("exit",m_status);
  p.add("bad_alloc",s_badalloc);

  ofd <<  p.dump() << endl;

}
