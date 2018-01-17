/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file findEvent.cxx
 * $Author: tmaeno $
 * $Revision: 1.3 $
 * $Date: 2008/02/22 13:33:08 $
 *
 */
 
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include <ctype.h>
#include <stdlib.h>

#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include "eformat/eformat.h"
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#include "eformat/old/util.h"
#include "EventStorage/pickDataReader.h"
#include <time.h>

int main (int argc, char *argv[])
{
  using namespace eformat;

  setenv("TZ", "Europe/Zurich", 1);
  tzset();
  
  //Interpret arguments
  //Format
  // findEvent -e [--event] <eventNumber> [-r, --run <runnumber>] [-l, --listevents] [-c, --checkevents] files ... 
  if(argc<2) {
    std::cerr << "usage: " << argv[0] << " -e [--event] <eventNumber> [-r, --run <runnumber>] [-c, --checkevents] [-l, --listevents] files ..." 
	      << std::endl;
    std::exit(1);
  }

  std::vector<std::string> fileNames;
  uint32_t searchEvent=std::numeric_limits<uint32_t>::max();
  uint32_t searchRun=std::numeric_limits<uint32_t>::max(); 
  uint32_t eventCounter=0;
  bool listevents=false;
  bool checkevents=false;
  bool found=false;
  for (int i=1;i<argc;i++) {
    const std::string& arg1(argv[i]);
    std::string arg2;
    if ((i+1) < argc) 
      arg2=argv[i+1];
    if (arg1=="-e" || arg1=="--event") {
      //try read event number
      if (arg2.size() && isdigit(arg2[0])) 
	searchEvent=atoi(arg2.c_str());
      else {
	std::cout << "ERROR: no numerical argument found after '" << arg1 << "'" << std::endl;
	return -1;
      }
      i++;
    }
    else if (arg1=="-r" || arg1=="--run") {
      //try read event number
      if (arg2.size() && isdigit(arg2[0])) 
	searchRun=atoi(arg2.c_str());
      else {
	std::cout << "ERROR: no numerical argument found after '" << arg1 << "'" << std::endl;
	return -1;
      }
      i++;
    }
    else if (arg1=="-l" || arg1=="--listevents") {
      listevents=true;
    }
    else if (arg1=="-c" || arg1=="--checkevents") {
      checkevents=true;
    }
    else
      fileNames.push_back(arg1);
  }// End loop over arguments

  if (!fileNames.size()) {
    std::cout << "ERROR: No file names set" << std::endl;
    return -1;
  }

  if (searchEvent==std::numeric_limits<uint32_t>::max()) 
    listevents=true;


  //start loop over files
  std::vector<std::string>::const_iterator it=fileNames.begin();
  std::vector<std::string>::const_iterator it_e=fileNames.end();
  for (;!found && it!=it_e;it++) {
    const std::string& fName=*it;
    eventCounter=0;

    std::cout << "Checking file " << fName << std::endl;
    DataReader *pDR = pickDataReader(fName);

    if(!pDR) {
      std::cout << "Problem opening or reading this file!\n";
      return -1;
    }

    if(!pDR->good()) {
      std::cerr << "Problem reading file "<< fName << std::endl;
      return -1;
    }

    if (listevents) {
      const std::vector<std::string> fmds=pDR->freeMetaDataStrings();
      std::cout << "File Metadata: " << std::endl;
      std::cout << "         GUID: " << pDR->GUID() << std::endl;
      std::cout << "   Start time: " << pDR->fileStartTime() << std::endl;
      std::cout << "   Start date: " << pDR->fileStartDate() << std::endl;
      std::cout << "  Project Tag: " << pDR->projectTag() << std::endl;
      std::cout << "   Stream Tag: " << pDR->stream() << std::endl;
      std::cout << "   Lumi Block: " << pDR->lumiblockNumber() << std::endl;
      std::cout << " Free Strings: ";
      if (fmds.size()==0) {
        std::cout << "None" << std::endl;
      } else {
        std::cout << fmds[0] << std::endl;
        for (std::size_t i_fmds=1; i_fmds<fmds.size(); ++i_fmds) {
          std::cout << "               " << fmds[i_fmds] << std::endl;
        }
      }
      std::cout << std::endl;
    }

    // the event loop
    while(!found && pDR->good()) {
      unsigned int eventSize;    
      char *buf;
	
      DRError ecode = pDR->getData(eventSize,&buf);
      if(DROK != ecode) {
	std::cout << "Can't read from file!" << std::endl;
	break;
      }

      ++eventCounter;
      uint32_t* fragment = reinterpret_cast<uint32_t*>(buf); 
    
      // make a fragment with eformat 3.0 and check it's validity
      try {
	if ((eformat::HeaderMarker)(fragment[0])!=FULL_EVENT) {
	  std::cout << "Event doesn't start with full event fragment (found " 
		    << std::ios::hex << fragment[0] << ") ignored." <<std::endl;
	  delete[] buf;
	  continue;
	}
	const uint32_t formatVersion = eformat::helper::Version(fragment[3]).major_version();
	//convert to new version if necessary
	if (formatVersion != eformat::MAJOR_DEFAULT_VERSION) {
	  // 100 for increase of data-size due to header conversion
	  uint32_t newEventSize = eventSize + 1000;
	  uint32_t* newFragment = new uint32_t[newEventSize];
	  eformat::old::convert(fragment,newFragment,newEventSize);
	  // delete old fragment
	  delete [] fragment;
	  // set new pointer
	  fragment = newFragment;
	}
	FullEventFragment<const uint32_t*> fe(fragment);
      
	if (checkevents) fe.check_tree();
	
	const uint32_t eventNo=fe.global_id();
	const uint32_t runNo=fe.run_no();
	const time_t sec=fe.bc_time_seconds();
	if (listevents) {
	  std::cout << "Index=" << eventCounter <<" Run=" << runNo << " Event=" << eventNo 
		    << " LB=" <<  fe.lumi_block() << " Size=" << fe. fragment_size_word()*4./1024 <<"kB " 
		    << ctime(&sec);// << std::endl;
	}
	//std::cout << "seconds: " << fe.bc_time_seconds() << " nanosecond: " << fe.bc_time_nanoseconds() << std::endl;

	if (eventNo==searchEvent && (searchRun==std::numeric_limits<uint32_t>::max() || (runNo==searchRun))) {
	  found=true;
	  std::cout << std::endl;
	  std::cout << "File:" <<  pDR->fileName() << std::endl;
	  std::cout << "Event index: " << eventCounter << std::endl;
	  std::cout << "Run:         " << runNo << std::endl;   
	  std::cout << "Event ID:    " << eventNo << std::endl;
	  std::cout << "LumiBlock:   " << fe.lumi_block() << std::endl;
	  std::cout << "Timestamp:   " << ctime(&sec);// << std::endl;
	}
      } 
      catch (eformat::Issue& ex) {
	std::cerr << "Uncaught eformat issue: " << ex.what() << std::endl;
      }
      catch (ers::Issue& ex) {
	std::cerr << "Uncaught ERS issue: " << ex.what() << std::endl;
      }
      catch (std::exception& ex) {
	std::cerr << "Uncaught std exception: " << ex.what() << std::endl;
      }
      catch (...) {
	std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
      }
      
      // end event processing 
      delete [] fragment;
    }

    //std::cout << std::endl;
    //std::cout << "File end time " << pDR->fileEndTime() << std::endl;
    //std::cout << "File end date " << pDR->fileEndDate() << std::endl;

    delete pDR;
  }
  if (!found) {
    if (searchRun!=std::numeric_limits<uint32_t>::max()) 
      std::cout << "Run " << searchRun << " ";
    if (searchEvent!=std::numeric_limits<uint32_t>::max()) 
      std::cout << "Event " << searchEvent << " not found in given files" 
		<< std::endl;
    return -1;
  }
  else
    return 0;
}









