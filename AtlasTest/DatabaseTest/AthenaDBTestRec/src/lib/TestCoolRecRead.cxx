/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TestCoolRecRead.cxx
// Athena algorithm to read configurable amount of test data from COOL
// started Richard Hawkings 24/1/06
#include <iostream>
#include <fstream>

#include "AthenaKernel/IOVRange.h"

#include "AthenaDBTestRec/TestCoolRecRead.h"

TestCoolRecRead::TestCoolRecRead(const std::string& name, 
  ISvcLocator* pSvcLocator) :
   AthAlgorithm(name,pSvcLocator),  
   m_par_checkmatch(true),m_par_delay(0),
   m_par_dumpchan(0),m_par_dumpfile("iovdump.txt"),
   p_detstore(0),m_nbadaux(0),m_dumpf(0)
{
  declareProperty("Folders",m_folders); 
  declareProperty("FTypes",m_ftypes);
  declareProperty("AuxFiles",m_auxfiles);
  declareProperty("CheckMatch",m_par_checkmatch);
  declareProperty("Delay",m_par_delay);
  declareProperty("DumpChannels",m_par_dumpchan);
  declareProperty("DumpFile",m_par_dumpfile);
}

TestCoolRecRead::~TestCoolRecRead() {}

StatusCode TestCoolRecRead::initialize() {

  // retrieve the DetStore old way as need pointer to pass to TestCoolRecFolder
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    ATH_MSG_FATAL("Detector store not found");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Number of folders for read: " << m_folders.size());
  // check consistency of folders and types
  if (m_folders.size()!=m_ftypes.size()) {
    ATH_MSG_FATAL("Number of folder types (" << m_ftypes.size() <<
		  ") does not match number of folders defined");
    return StatusCode::FAILURE;
  }

  // setup dumpfile if needed
  if (m_par_dumpchan!=0) {
    ATH_MSG_INFO("First " << m_par_dumpchan << 
      " channels will be dumped on file " << m_par_dumpfile << 
		 " for each event");
    // attempt to open output file
    m_dumpf=new std::ofstream(m_par_dumpfile.c_str());
    if (m_dumpf==0) {
      ATH_MSG_FATAL("Cannot open output file");
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
    ATH_MSG_INFO("Detector store contains " << proxylist.size() <<
		 " proxies - define folders from there");
    for (std::vector<const SG::DataProxy*>::const_iterator pitr=
	 proxylist.begin();pitr!=proxylist.end();++pitr) {
      m_folderlist.push_back(TestCoolRecFolder((*pitr)->name(),3,*pitr,
					       m_par_dumpchan,m_dumpf));
    }
  } else {
    // setup list of folders from input
    for (unsigned int i=0;i<m_folders.size();++i) {
      m_folderlist.push_back(TestCoolRecFolder(m_folders[i],
					       m_ftypes[i],0,
					       m_par_dumpchan,m_dumpf));
    }
  }
  for (std::vector<TestCoolRecFolder>::iterator itr=m_folderlist.begin();
	 itr!=m_folderlist.end();++itr) {
    ATH_MSG_INFO("Folder " << itr->name() << 
		  " ( type " << itr->type() << ")");
    if (StatusCode::SUCCESS!=(itr->regCallBack(p_detstore))) 
      ATH_MSG_ERROR("Failed to register callback for folder " << 
		    itr->name());
  }
  if (m_par_checkmatch) ATH_MSG_INFO("Conditions data matchup will be checked for consistency");
  if (m_par_delay>0) ATH_MSG_INFO("Delay for " << m_par_delay << 
				" seconds at end of each event");

  if (m_auxfiles.size()>0) m_nbadaux=readAuxFiles();
  return StatusCode::SUCCESS;
  }

StatusCode TestCoolRecRead::execute() {
  // find the run number for mismatch check
  const EventIDBase& eid = getContext().eventID();
  int run = eid.run_number();
  int event = eid.event_number();
  int lumib = eid.lumi_block();
  uint64_t nsTime = eid.time_stamp()*1000000000LL;
  IOVTime iovkey(run,lumib,nsTime);
  // loop through all folders and access data
  for (std::vector<TestCoolRecFolder>::iterator ifolder=m_folderlist.begin();
       ifolder!=m_folderlist.end();++ifolder) {
    if (ifolder->access(p_detstore,run,event,iovkey)!=
	StatusCode::SUCCESS)
      ATH_MSG_ERROR("Retrieve for class folder "+ifolder->name()+" failed");
  }
  if (m_par_delay>0) {
    ATH_MSG_INFO("Delaying for " << m_par_delay << " seconds");
    char commstr[32];
    sprintf(commstr,"sleep %d\n",m_par_delay);
    system(commstr);
  }
  return StatusCode::SUCCESS;
}

StatusCode TestCoolRecRead::finalize() {

  int nerrorfold=0;
  int nemptyfold=0;
  int nmismatchfold=0;
  // print statistics for all folders
  for (std::vector<TestCoolRecFolder>::iterator ifolder=m_folderlist.begin();
       ifolder!=m_folderlist.end();++ifolder) {
    ATH_MSG_INFO("Folder " << ifolder->name() << " nAccess=" <<
      ifolder->nAccess() <<   " nItems=" << ifolder->nItems() <<
		 " nCallBack=" << ifolder->nCallBack());
    if (ifolder->nError()>0) {
      ++nerrorfold;
      ATH_MSG_ERROR("Folder " << ifolder->name() << " had " << 
		    ifolder->nError() << " access errors");
    }
    if (ifolder->nMisMatch()>0 && m_par_checkmatch) {
      ATH_MSG_ERROR("Folder " << ifolder->name() << " had " <<
	ifolder->nMisMatch() << " data mismatches");
      ++nmismatchfold;
    }
    if (ifolder->nItems()==0) {
      ++nemptyfold;
      ATH_MSG_ERROR("Folder " << ifolder->name() << " read no data");
    }
  }
  msg(MSG::INFO) << "TestCoolRecReadSummary: "; 
  if (nerrorfold>0 || nemptyfold>0 || m_nbadaux>0 || 
      (nmismatchfold>0 && m_par_checkmatch)) {
    msg()  << "BAD (" << nerrorfold << "," << nemptyfold << "," << nmismatchfold
	  << "," << m_nbadaux << 
       ") error, empty folders, msimatched folders, bad files " << endmsg;
  } else {
    msg() <<  "All folders OK" << endmsg;
  }
  return StatusCode::SUCCESS;
}

int TestCoolRecRead::readAuxFiles() {
  // read auxilliary files to simulate (badly) some POOL file access
  int nbad=0;
  for (std::vector<std::string>::const_iterator auxfile=m_auxfiles.begin();
       auxfile!=m_auxfiles.end();++auxfile) {
    // attempt to open file
    std::ifstream readfile(auxfile->c_str(),std::ios::in | std::ios::binary);
    if (!readfile.is_open()) {
      ATH_MSG_ERROR("Could not open auxilliary file: " << *auxfile);
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
      ATH_MSG_INFO("Read " << nbytes << " bytes from auxilliary file: " << 
		   *auxfile);
    }
  }
  return nbad;
}
