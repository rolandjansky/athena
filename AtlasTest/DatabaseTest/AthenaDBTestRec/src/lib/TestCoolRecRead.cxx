/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCoolRecRead.cxx
// Athena algorithm to read configurable amount of test data from COOL
// started Richard Hawkings 24/1/06
#include <iostream>
#include <fstream>

#include "StoreGate/StoreGateSvc.h"
//#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "AthenaKernel/IOVRange.h"

#include "AthenaDBTestRec/TestCoolRecRead.h"

TestCoolRecRead::TestCoolRecRead(const std::string& name, 
  ISvcLocator* pSvcLocator) :
   Algorithm(name,pSvcLocator),  
   par_checkmatch(true),par_delay(0),
   par_dumpchan(0),par_dumpfile("iovdump.txt"),
   p_storegate(0),p_detstore(0),m_nbadaux(0),m_dumpf(0)
{
  declareProperty("Folders",m_folders); 
  declareProperty("FTypes",m_ftypes);
  declareProperty("AuxFiles",m_auxfiles);
  declareProperty("CheckMatch",par_checkmatch);
  declareProperty("Delay",par_delay);
  declareProperty("DumpChannels",par_dumpchan);
  declareProperty("DumpFile",par_dumpfile);
}

TestCoolRecRead::~TestCoolRecRead() {}

StatusCode TestCoolRecRead::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "In initialize()" << endreq;

  if (StatusCode::SUCCESS!=service("StoreGateSvc",p_storegate)) {
    log << MSG::FATAL << "Event store not found" << endreq;
    return StatusCode::FAILURE;
  }
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    log << MSG::FATAL << "Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Number of folders for read: " << m_folders.size()
	<< endreq;
  // check consistency of folders and types
  if (m_folders.size()!=m_ftypes.size()) {
    log << MSG::FATAL << "Number of folder types (" << m_ftypes.size() <<
      ") does not match number of folders defined" << endreq;
    return StatusCode::FAILURE;
  }

  // setup dumpfile if needed
  if (par_dumpchan!=0) {
    log << MSG::INFO << "First " << par_dumpchan << 
      " channels will be dumped on file " << par_dumpfile << 
      " for each event" << std::endl;
    // attempt to open output file
    m_dumpf=new std::ofstream(par_dumpfile.c_str());
    if (m_dumpf==0) {
      log << MSG::FATAL << "Cannot open output file" << endreq;
    }
    * m_dumpf << "# Folder EventNum Channel IOVKey Start Stop (Payload)" <<
      std::endl;
  }

  if (m_folders.size()==0) {
    // if no folders defined, define the list from proxies in DetectorStore
    // For folders using <key> to define a differnt SG key from folder name
    // this will result in folder references using the key name not the folder
    // name
    std::vector<const SG::DataProxy*> proxylist=p_detstore->proxies();
    log << MSG::INFO << "Detector store contains " << proxylist.size() <<
      " proxies - define folders from there" << endreq;
    for (std::vector<const SG::DataProxy*>::const_iterator pitr=
	 proxylist.begin();pitr!=proxylist.end();++pitr) {
      m_folderlist.push_back(TestCoolRecFolder((*pitr)->name(),3,*pitr,
					       par_dumpchan,m_dumpf));
    }
  } else {
    // setup list of folders from input
    for (unsigned int i=0;i<m_folders.size();++i) {
      m_folderlist.push_back(TestCoolRecFolder(m_folders[i],
					       m_ftypes[i],0,
					       par_dumpchan,m_dumpf));
    }
  }
  for (std::vector<TestCoolRecFolder>::iterator itr=m_folderlist.begin();
	 itr!=m_folderlist.end();++itr) {
    log << MSG::INFO << "Folder " << itr->name() << 
			       " ( type " << itr->type() << ")" << endreq;
    if (StatusCode::SUCCESS!=(itr->regCallBack(p_detstore)))
      log << MSG::ERROR << "Failed to register callback for folder " << 
         itr->name() << endreq;
  }
  if (par_checkmatch) log << MSG::INFO << 
   "Conditions data matchup will be checked for consistency" << endreq;
  if (par_delay>0) log << MSG::INFO << "Delay for " << par_delay << 
		     " seconds at end of each event" << endreq;

  if (m_auxfiles.size()>0) m_nbadaux=readAuxFiles();
  return StatusCode::SUCCESS;
}

StatusCode TestCoolRecRead::execute() {
  MsgStream log(msgSvc(), name());
  // find the run number for mismatch check
  int run=0;
  int event=0;
  int lumib=0;
  uint64_t nsTime=0;
  // const xAOD::EventInfo* eventInfo(0);
  const DataHandle<EventInfo> eventInfo;
  if (StatusCode::SUCCESS==p_storegate->retrieve(eventInfo)) {
    run=eventInfo->event_ID()->run_number();
    event=eventInfo->event_ID()->event_number();
    lumib=eventInfo->event_ID()->lumi_block();
    nsTime=eventInfo->event_ID()->time_stamp()*1000000000LL;
   } else {
     log << MSG::WARNING << "Could not get Event object to find runnumber"
	   << endreq;
   }
  IOVTime iovkey(run,lumib,nsTime);
  // loop through all folders and access data
  for (std::vector<TestCoolRecFolder>::iterator ifolder=m_folderlist.begin();
       ifolder!=m_folderlist.end();++ifolder) {
    if (ifolder->access(p_detstore,run,event,iovkey)!=
	StatusCode::SUCCESS)
      log << MSG::ERROR << "Retrieve for class folder " << ifolder->name()
	    << " failed" << endreq;
  }
  if (par_delay>0) {
    log << MSG::INFO << "Delaying for " << par_delay << " seconds" << endreq;
    char commstr[32];
    sprintf(commstr,"sleep %d\n",par_delay);
    system(commstr);
  }
  return StatusCode::SUCCESS;
}

StatusCode TestCoolRecRead::finalize() {
  MsgStream log(msgSvc(), name());

  int nerrorfold=0;
  int nemptyfold=0;
  int nmismatchfold=0;
  // print statistics for all folders
  for (std::vector<TestCoolRecFolder>::iterator ifolder=m_folderlist.begin();
       ifolder!=m_folderlist.end();++ifolder) {
    log << MSG::INFO << "Folder " << ifolder->name() << " nAccess=" <<
      ifolder->nAccess() <<   " nItems=" << ifolder->nItems() <<
      " nCallBack=" << ifolder->nCallBack() << endreq;
    if (ifolder->nError()>0) {
      ++nerrorfold;
      log << MSG::ERROR << "Folder " << ifolder->name() << " had " << 
        ifolder->nError() << " access errors" << endreq;
    }
    if (ifolder->nMisMatch()>0 && par_checkmatch) {
      log << MSG::ERROR << "Folder " << ifolder->name() << " had " <<
	ifolder->nMisMatch() << " data mismatches" << endreq;
      ++nmismatchfold;
    }
    if (ifolder->nItems()==0) {
      ++nemptyfold;
      log << MSG::ERROR << "Folder " << ifolder->name() << " read no data"
	    << endreq;
    }
  }
  log << MSG::INFO << "TestCoolRecReadSummary: ";
  if (nerrorfold>0 || nemptyfold>0 || m_nbadaux>0 || 
      (nmismatchfold>0 && par_checkmatch)) {
    log << "BAD (" << nerrorfold << "," << nemptyfold << "," << nmismatchfold
	  << "," << m_nbadaux << 
       ") error, empty folders, msimatched folders, bad files " << endreq;
  } else {
    log <<  "All folders OK" << endreq;
  }
  return StatusCode::SUCCESS;
}

int TestCoolRecRead::readAuxFiles() {
  MsgStream log(msgSvc(), name());
  // read auxilliary files to simulate (badly) some POOL file access
  int nbad=0;
  for (std::vector<std::string>::const_iterator auxfile=m_auxfiles.begin();
       auxfile!=m_auxfiles.end();++auxfile) {
    // attempt to open file
    std::ifstream readfile(auxfile->c_str(),std::ios::in | std::ios::binary);
    if (!readfile.is_open()) {
      log << MSG::ERROR << "Could not open auxilliary file: " << *auxfile
	    << endreq;
      ++nbad;
    } else {
      int nbytes=0;
      int bufsize=100000;
      char* buffer=new char[bufsize];
      while (!readfile.eof()) {
	readfile.read(buffer,bufsize);
        nbytes+=bufsize;
      }
      readfile.close();
      delete[] buffer;
      log << MSG::INFO << "Read " << nbytes << 
	" bytes from auxilliary file: " << *auxfile << endreq;
    }
  }
  return nbad;
}
