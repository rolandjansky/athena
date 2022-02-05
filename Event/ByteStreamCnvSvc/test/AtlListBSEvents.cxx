/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file findEvent.cxx
 * $Author: ssnyder $
 * $Revision: 754823 $
 * $Date: 2016-06-14 20:32:30 +0200 (Tue, 14 Jun 2016) $
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
# pragma GCC diagnostic ignored "-Wparentheses"
#endif
#include "eformat/eformat.h"
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#include "eformat/old/util.h"
#include "eformat/index.h"
#include "EventStorage/pickDataReader.h"
#include <time.h>

#include "CxxUtils/checker_macros.h"

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[])
{
  using namespace eformat;

  //Interpret arguments
  //Format
  // findEvent -e [--event] <eventNumber> [-r, --run <runnumber>] [-l, --listevents] [-c, --checkevents] files ... 
  if(argc<2) {
    std::cerr << "usage: " << argv[0] << " [-s, --showsize] [-c, --checkevents] [-l, --listevents] [-m, --maxevents] files ..." 
	      << std::endl;
    std::exit(1);
  }

  std::vector<std::pair<eformat::SubDetectorGroup,std::string> > namesPerSubdet;
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(PIXEL,"  Pixel"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(SCT,"    SCT"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(TRT,"    TRT"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(LAR,"    LAr"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(TILECAL,"   Tile"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(MUON,"   Muon"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(TDAQ,"   TDAQ"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(FORWARD,"Forward"));
  namesPerSubdet.push_back(std::make_pair<eformat::SubDetectorGroup,std::string>(OTHER_DETECTORS,"  others"));


  std::map<SubDetectorGroup,unsigned> totalSizePerSubdet;
  std::map<SubDetectorGroup,unsigned>::iterator sizeit;
  
  for(sizeit=totalSizePerSubdet.begin();sizeit!=totalSizePerSubdet.end();++sizeit)
    sizeit->second=0;

  unsigned totalSize=0;

  std::vector<std::string> fileNames;
  unsigned eventCounter=0;
  unsigned maxEvents=std::numeric_limits<unsigned>::max();
  bool listevents=false;
  bool checkevents=false;
  bool showSizes=false;
  for (int i=1;i<argc;i++) {
    const std::string& arg1(argv[i]);
    std::string arg2;
    if ((i+1) < argc) 
      arg2=argv[i+1];

    if (arg1=="-l" || arg1=="--listevents") {
      listevents=true;
    }
    else if (arg1=="-c" || arg1=="--checkevents") {
      checkevents=true;
    }
    else if (arg1=="-s" || arg1=="--showsize") {
      showSizes=true;
    }
    else if (arg1=="-m" || arg1=="--maxevents") {
       if (arg2.size() && isdigit(arg2[0])) 
	 maxEvents=atoi(arg2.c_str());
       else {
	 std::cout << "ERROR: no numerical argument found after '" << arg1 << "'" << std::endl;
	 return -1;
       }
       i++;
       //std::cout << "Max events set to " << maxEvents << std::endl;
    }
    else
      fileNames.push_back(arg1);
  }// End loop over arguments

  if (!fileNames.size()) {
    std::cout << "ERROR: No file names set" << std::endl;
    return -1;
  }

  //start loop over files
  for (const std::string& fName : fileNames) {
    std::cout << "Checking file " << fName << std::endl;
    DataReader *pDR = pickDataReader(fName);

    if(!pDR) {
      std::cout << "Problem opening or reading this file!\n";
      return -1;
    }

    if(!pDR->good()) {
      std::cout << "No events in file "<< fName << std::endl;
    }

    //Print file summary
    const std::vector<std::string> fmds=pDR->freeMetaDataStrings();
    std::cout << "File Metadata: " << std::endl;
    std::cout << "         GUID: " << pDR->GUID() << std::endl;
    std::cout << "   Start time: " << pDR->fileStartTime() << std::endl;
    std::cout << "   Start date: " << pDR->fileStartDate() << std::endl;
    std::cout << "  Project Tag: " << pDR->projectTag() << std::endl;
    std::cout << "   Stream Tag: " << pDR->stream() << std::endl;
    std::cout << "   Lumi Block: " << pDR->lumiblockNumber() << std::endl;
    std::cout << "   Run Number: " << pDR->runNumber() << std::endl;
    std::cout << " Free Strings: ";
    if (fmds.size()==0) 
      std::cout << "None" << std::endl;
    else {
      std::cout  << fmds[0] << std::endl;
      for (std::size_t i_fmds=1;i_fmds<fmds.size();++i_fmds) 
	std::cout << "               " << fmds[i_fmds] << std::endl;
    }
    std::cout << "Start loop through events" << std::endl;
     

    // the event loop
    while(pDR->good() && eventCounter<=maxEvents) {
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
	totalSize+=fe.readable_payload_size_word()*4;
	
	const uint32_t eventNo=fe.global_id();
	const uint32_t runNo=fe.run_no();
	const time_t sec=fe.bc_time_seconds();
	if (listevents) {
	  std::cout << "Index=" << eventCounter <<" Run=" << runNo << " Event=" << eventNo 
		    << " LB=" <<  fe.lumi_block() << " Size=" << fe. fragment_size_word()*4./1024 <<"kB" 
		    << " Offset=" << pDR->getPosition() << " " << ctime(&sec);// << std::endl;
	}
	if (showSizes) {
	  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> > robIndex;
	  std::map<eformat::SubDetectorGroup, std::vector<const uint32_t*> >::const_iterator sd_it, sd_it_e;
	  eformat::helper::build_toc(fe,robIndex );
	  sd_it_e=robIndex.end();
	  for (sd_it=robIndex.begin();sd_it!=sd_it_e;++sd_it) {
	    const eformat::SubDetectorGroup sd=sd_it->first;
	    const std::vector<const uint32_t*>& robs = sd_it->second;
	    std::vector<const uint32_t*>::const_iterator rob_it=robs.begin();
	    std::vector<const uint32_t*>::const_iterator rob_it_e=robs.end();
	    for (;rob_it!=rob_it_e;++rob_it) {
	      ROBFragment<const uint32_t*> robFrag(*rob_it);
	      const unsigned robsize=robFrag.fragment_size_word()*sizeof(uint32_t);
	      totalSizePerSubdet[sd]+=robsize;
	    }//end loop over Rob-fragmets
	  }//end loop over subdets
	}//end if showSizes
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

  //Print summary:


  std::ios_base::fmtflags original_flags = std::cout.flags();
  std::cout.setf(std::ios::right | std::ios::fixed);//,std::ios::floatfield); 
  std::cout.width(10);
  std::cout.precision(2);
  if (showSizes) {
    std::cout << std::endl << "Average fragment size per subdetector:" << std::endl;
    std::vector<std::pair<SubDetectorGroup,std::string> >::const_iterator nit=namesPerSubdet.begin();
    std::vector<std::pair<SubDetectorGroup,std::string> >::const_iterator nit_e=namesPerSubdet.end();
    unsigned sum=0;
    for(;nit!=nit_e;++nit) {
      const SubDetectorGroup sd=nit->first;
      const std::string& name=nit->second;
      
      const unsigned s=totalSizePerSubdet[sd];
      sum+=s;
      double sPerEv=0;
      if (eventCounter>0) sPerEv=s/(1024.0*eventCounter); //In kB

      double fraction=0;
      if (totalSize>0) fraction=s/double(totalSize);
      std::cout << name << " :" << sPerEv << " kB/event (" << 100*fraction << "%)" << std::endl;
    }
    const unsigned overhead=totalSize-sum;
    double ohPerEv=0;
    if (eventCounter>0) ohPerEv=overhead/(double)eventCounter;
    std::cout << "Overhead: " << overhead/1024.0 <<" kB or " << ohPerEv << " Bytes/event" << std::endl;
  }

  std::cout << "Total: " << totalSize/(1024.0*eventCounter) << " kB/event" << std::endl; 
  std::cout.flags(original_flags);
  
  return 0;
}
