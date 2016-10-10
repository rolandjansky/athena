/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/FTKSetup.h"
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>  // needed for older versions of GCC
using namespace std;

FTKSetup FTKSetup::m_instance;

#ifndef FTK_STANDALONE
MsgStream* FTKSetup::m_log(0x0);
#endif


FTKSetup::FTKSetup() :
   m_BarrelOnly(0), m_SCTtrkMode(0), m_IBLMode(0), m_ITkMode(false), m_verbosity(0),
   m_tot_events(0), m_EnableFTKSim(0), m_RawMode(1),
   m_SectorsAsPatterns(0),
   m_MaxMissingPlanes(1), m_MaskLayers(0), m_RoadWarrior(1),
   m_KeepRemoved(0), m_HWModeSS(0), m_event(-1), m_MaxMissingSctPairs(0),
   m_restrictSctPairLayer(0), m_restrictSctPairModule(0),
   m_usage_ncalls(0),m_fixEndcapL0(false)
{
  //m_pid = getpid();
  //cout << "FTK Framework created (PID " << m_pid << ")" << endl;
  //gettimeofday(&m_starttime,0);
  //cout << "Start time " << m_starttime.tv_sec << " sec" << endl;
}


FTKSetup::~FTKSetup()
{
  // todo
}

/** this method print a message with the usage statistic of the 
    process, it uses the  /proc/<PID>/status file, the results
    and their results can depend by the kernel configuration */
void FTKSetup::usageStat(const char *header /* =0 */)
{
  // collect all the information from the proc file
  ostringstream pcpuname;
  pcpuname << "/proc/" << m_pid << "/stat" << ends;
  ifstream cpuinfo(pcpuname.str().c_str());
  // prepare the variables to completely parse /proc/[pid]/stat file
  int pid;
  cpuinfo >> pid;
  string command;
  cpuinfo >> command;
  char state;
  cpuinfo >> state;
  int ppid;
  cpuinfo >> ppid;
  int pgrp;
  cpuinfo >> pgrp;
  int session;
  cpuinfo >> session;
  int terminal;
  cpuinfo >> terminal;
  int tpgid;
  cpuinfo >> tpgid;
  unsigned int flags;
  cpuinfo >> flags;
  unsigned long int minflt;
  cpuinfo >> minflt;
  unsigned long int cminflt;
  cpuinfo >> cminflt;
  unsigned long int maxflt;
  cpuinfo >> maxflt;
  unsigned long int cmaxflt;
  cpuinfo >> cmaxflt;
  unsigned long int utime;
  cpuinfo >> utime;
  unsigned long int stime;
  cpuinfo >> stime;
  long int cutime;
  cpuinfo >> cutime;
  long int cstime;
  cpuinfo >> cstime;
  long int priority;
  cpuinfo >> priority;
  long int nice;
  cpuinfo >> nice;
  long int num_threads;
  cpuinfo >> num_threads;
  long int itrealvalue;
  cpuinfo >> itrealvalue;
  long long int starttime;
  cpuinfo >> starttime;
  unsigned long long int vsize;
  cpuinfo >> vsize;
  unsigned long long int rss;
  cpuinfo >> rss;
  unsigned long long int rsslim;
  cpuinfo >> rsslim;
  unsigned long int startcode;
  cpuinfo >> startcode;
  unsigned long int endcode;
  cpuinfo >> endcode;
  unsigned long int startstack;
  cpuinfo >> startstack;
  unsigned long int kstkesp;
  cpuinfo >> kstkesp;
  unsigned long int kstkeip;
  cpuinfo >> kstkeip;
  unsigned long int signal;
  cpuinfo >> signal;
  unsigned long int blocked;
  cpuinfo >> blocked;
  unsigned long int sigignore;
  cpuinfo >> sigignore;
  unsigned long int sigcatch;
  cpuinfo >> sigcatch;
  unsigned long int wchan;
  cpuinfo >> wchan;
  unsigned long int nswap;
  cpuinfo >> nswap;
  unsigned long int cnswap;
  cpuinfo >> cnswap;
  int exit_signal;
  cpuinfo >> exit_signal;
  int processor;
  cpuinfo >> processor;
  unsigned int rt_priority;
  cpuinfo >> rt_priority;
  unsigned int policy;
  cpuinfo >> policy;
  unsigned long long int delayacct;
  cpuinfo >> delayacct;
  unsigned long int guest_time;
  cpuinfo >> guest_time;
  long int cguest_time;
  cpuinfo >> cguest_time;
  cpuinfo.close();

  // get the current time
  struct timeval curtime;
  gettimeofday(&curtime,0);

  /* evaluate the time needed
     PS there is a possible issue if time is changed during the
     execution, i.e. if an ntpd server is used but is not
     a relevand problem */
  int realsec = curtime.tv_sec-m_starttime.tv_sec;
  // the microsendo as converted to be compatible with the proc units (1/100)
  int realcsec = (curtime.tv_usec-m_starttime.tv_usec)/10000;

  ++m_usage_ncalls;

  // Comment out to make Coverity happy
  // if (!cpuinfo&0) {
  //   cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Error occurred reading the file statistic, skipped" << endl;
  //   return;
  // }

  cerr << endl;
  if (header) {
    cerr << "USG[" << setw(2) << m_usage_ncalls <<  "]::" << header << endl;
  }

  const unsigned long int MBunit = 1048576;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::State " << state << endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Usr Time " << utime/100. << " sec"<< endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Sys Time " << stime/100. << " sec"<< endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Real Time " << realsec+0.01*realcsec << " sec" << endl;
  if (realsec>1)
    cerr << "USG[" << setw(2) << m_usage_ncalls << "]::CPU eff " << static_cast<float>(utime+stime)/realsec << " %" << endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Vir Mem " << vsize/MBunit << " MB" << endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::RSS Mem " << rss/MBunit << " MB" << endl;
  cerr << "USG[" << setw(2) << m_usage_ncalls << "]::Max Mem " << rsslim/MBunit << " MB" << endl;;
  cerr << endl;
}


/** Wrapper to print an error message and block the program execution if
    the error level is over a given threshold */
void FTKSetup::PrintMessage(ftk::message_level_t level, const char *msg)
{
#ifndef FTK_STANDALONE
   if(m_log) {
  if (level == ftk::debg) 
    *m_log << MSG::DEBUG;
  else if (level == ftk::warn)
    *m_log << MSG::WARNING;
  else if (level == ftk::sevr) 
    *m_log << MSG::FATAL;
  else 
    *m_log << MSG::INFO;

  *m_log << msg  << endmsg;
   } else {
      cout << "(level="<<level<<") "<<msg << endl;
   }
#else
   cout << msg << endl;
#endif
   if (level>=ftk::sevr) {     
     throw FTKException(msg);
   }
}


/** Wrapper to print an error message and block the program execution if
    the error level is over a given threshold */
void FTKSetup::PrintMessageFmt(ftk::message_level_t level, const char *msg,...)
{
  va_list va;
  va_start(va,msg);
  vprintf(msg,va);
  va_end(va);
  fflush(stdout);

  if (level>=ftk::sevr) {
    throw FTKException("Runtime error, message provided on stdout");
  }
}
