/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MemoryMonitoringTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class include
#include "MemoryMonitoringTool.h"

// stl includes
#include <fstream>

/** Constructor **/
ISF::MemoryMonitoringTool::MemoryMonitoringTool( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_numCalls(0),
  m_prevCallMemUsage(0),
  m_accumulatedCallMemory(0.),
  m_accumulatedIncrMemory(0.),
  m_table()
{
  declareInterface<ISF::IMonitoringTool>(this);
}

/** Destructor */
ISF::MemoryMonitoringTool::~MemoryMonitoringTool() {
  // clean up memory
  // loop over all (infoStr,memUsage) pairs and print them
  InfoUsagePairVector::const_iterator it    = m_table.begin();
  InfoUsagePairVector::const_iterator itEnd = m_table.end();
  for ( ; it != itEnd; ++it) {
    // the Info string:
    //delete (*it)->first;
    // the InfoUsagePair:
    delete (*it);
  }
  // reset summary table vector
  m_table.clear();

}


/** Athena algtool Hooks */
StatusCode  ISF::MemoryMonitoringTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  ATH_MSG_VERBOSE("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool Hook */
StatusCode  ISF::MemoryMonitoringTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_VERBOSE("finalize() successful");
  return StatusCode::SUCCESS;
}


/** return current memory usage in kBytes */
double ISF::MemoryMonitoringTool::getCurrent() const {
  return double( computeCurMemoryUsage());
}


/** dump current memory monitoring information */
void ISF::MemoryMonitoringTool::dumpCurrent( const char *infoStr,
                                             bool considerInSummary) {

  // get current memory usage
  int curMem = computeCurMemoryUsage();

  // store for different stats
  int curDiff = curMem-m_prevCallMemUsage;

  // use these stats for the summary table
  if (considerInSummary) {
    m_accumulatedCallMemory  += curMem;
    m_accumulatedIncrMemory  += curDiff;
    ++m_numCalls;
  }

  ATH_MSG_INFO( "Current Process Memory Usage " << infoStr
                << " (VmMem,diff-previous) kBytes: "
                << curMem<<" "<< curDiff );

  m_prevCallMemUsage = curMem;

  return;
}


/** store the current memory monitoring information internally */
void ISF::MemoryMonitoringTool::recordCurrent( const char *infoStr) {

  int curUsage = computeCurMemoryUsage();
  ATH_MSG_VERBOSE("recording current memory footprint of "
                  << curUsage << " kBytes");

  // create new InfoUsagePair and store it locally in vector
  InfoUsagePair *curInfoUsage = new InfoUsagePair(infoStr, curUsage);
  m_table.push_back( curInfoUsage);

  return;
}


/** dump all internally stored memory monitoring information */
void ISF::MemoryMonitoringTool::dumpSummary(const char *desc) const {

  ATH_MSG_INFO("*****************************************************"<<endmsg<<
               "*  (VmMem) MEMORY SUMMARY: (kBytes)");

  // loop over all (infoStr,memUsage) pairs and print them
  InfoUsagePairVector::const_iterator it    = m_table.begin();
  InfoUsagePairVector::const_iterator itEnd = m_table.end();
  for ( ; it != itEnd; ++it) {
    const char *curInfo  = (*it)->first;
    int         curUsage = (*it)->second;

    ATH_MSG_INFO("*  Memory Usage " << curInfo << "\t\t: " << curUsage);
  }

  if (m_numCalls) {
    if (!desc) desc = "MemoryMonitor call";
    ATH_MSG_INFO( "*  Average memory per " << desc << " : "
                  << (int)(m_accumulatedCallMemory/m_numCalls) );
    ATH_MSG_INFO( "*  Average memory increase per " << desc << " : "
                  << (int)(m_accumulatedIncrMemory/m_numCalls));
  }

  ATH_MSG_INFO("*****************************************************");

  return;
}

/** consolidate /proc to determine current memory usage */
int ISF::MemoryMonitoringTool::computeCurMemoryUsage() const {
  // this code is copied from:
  // atlasoff: Simulation/G4Sim/SimHelpers/SimHelpers/MemorySnooper.h

  int pid=getpid();
  std::ostringstream is;
  is<<pid;
  std::string spid=is.str();
  std::string filename="mem"+spid+".txt";
  std::string temp="cat /proc/"+spid+"/status | grep VmSize >"+filename;
  system(temp.c_str());
  std::ifstream in(filename.c_str());
  std::string text,mem,text1;
  in>>text>>mem>>text1;
  temp="rm -f "+filename;
  system(temp.c_str());
  float memsize=atof(mem.c_str());

  // finally return the memory size
  return int(memsize);
}
