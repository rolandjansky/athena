/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AtlCopyBSEvent.cxx
 * $Author: hma $
 * $Revision: 1.3 $
 * $Date: 2008-10-01 18:26:56 $
 *
 */
 
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <ctype.h>
#include <stdlib.h>

#include "eformat/eformat.h"
#include "eformat/old/util.h"
#include "EventStorage/pickDataReader.h"
#include "EventStorage/DataWriter.h" 

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include "PersistentDataModel/Token.h"

#include "CollectionBase/TokenList.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/CollectionRowBuffer.h"

#include "FileCatalog/IFileCatalog.h"

#include "CxxUtils/checker_macros.h"

void eventLoop(DataReader*, EventStorage::DataWriter*, unsigned&, const std::vector<uint32_t>*, uint32_t, bool, bool, bool, const std::vector<long long int>* = 0);

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]) {
  using namespace eformat;

  //Interpret arguments
  if(argc<3) {
    std::cerr << "usage: " << argv[0] 
	      << " [-d --deflate] -e [--event] <eventNumbers> [-r, --run <runnumber>] [-l, --listevents] [-t --checkevents] -o, --out outputfile inputfiles...." << std::endl;
    std::cerr << "eventNumbers is a comma-separated list of events" << std::endl;
    std::cerr << "or using TAG collections: " << std::endl;
    std::cerr << "usage: " << argv[0] 
	      << " [-d --deflate] -s, --src <input collection name> <input collection type> [-x --srcconnect <input database connection string>] [-q --query <predicate string>] [-c --catalog <file catalogs>] -o, --out outputfile"  << std::endl;
    std::exit(1);
  }

  std::string fileNameOut("extractedEvents.data");
  std::vector<std::string> fileNames;
  std::vector<std::string> catalogFile;
  std::vector<std::string> collNames, collTypes;
  std::string collConnect;
  std::string collQuery;
  std::vector<uint32_t> searchEvents;
  std::vector<std::string> searchTokens;
  uint32_t searchRun=0; 
  bool searchEventSet=false;
  bool searchRunSet=false;
  bool listEvents=false;
  bool checkEvents=false;
  bool compressEvents=false;
  unsigned nFound=0;
  for (int i=1; i<argc; i++) {
    const std::string& arg1(argv[i]);
    if (arg1=="-d" || arg1=="--deflate") {
      compressEvents=true;
    } else if (arg1=="-e" || arg1=="--event") {
      //try read the list of event number
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      if (arg2.size()>0 && isdigit(arg2[0])) {
	size_t p=0;
	while (p!=std::string::npos) {
	  searchEvents.push_back(atol(arg2.c_str()+p));
	  p=arg2.find(',',p);
	  if (p!=std::string::npos) p++;
	}
      } else if (arg2=="all") {
	std::cout << "Copy all events" << std::endl;
      } else {
	std::cout << "ERROR: no numerical argument found after '" << arg1 << "'" << std::endl;
	return -1;
      }
      i++;
      searchEventSet=true;
    } else if (arg1=="-o" || arg1=="--out") {
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      //set output file name
      if (arg2.size()>0) fileNameOut=arg2;
      else {
	std::cout << "ERROR: Expected output file name after '" << arg1 << "'" <<std::endl;
	return -1;
      }
      i++;
    } else if (arg1=="-r" || arg1=="--run") {
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      //try read event number
      if (arg2.size()>0 && isdigit(arg2[0])) searchRun=atoi(arg2.c_str());
      else {
	std::cout << "ERROR: no numerical argument found after '" << arg1 << "'" << std::endl;
	return -1;
      }
      i++;
      searchRunSet=true;
    } else if (arg1=="-s" || arg1=="--src") {
      std::string arg2, arg3;
      if ((i+2) < argc) {
        arg2=argv[i+1];
        arg3=argv[i+2];
      }
      if (arg2.size()>0 && arg3.size()>0) {
        collNames.push_back(arg2);
        collTypes.push_back(arg3);
        i++; i++;
        while ((i+2) < argc) {
          arg2=argv[i+1];
          arg3=argv[i+2];
          if (arg2.empty() || arg3.empty()) break;
          if (arg2.at(0) == '-' || arg3.at(0) == '-') break;
          collNames.push_back(arg2);
          collTypes.push_back(arg3);
          i++; i++;
        }
      } else {
	std::cout << "ERROR: no input collection <name>/<type> found after '" << arg1 << "'" << std::endl;
	return -1;
      }
    } else if (arg1=="-x" || arg1=="--srcconnect") {
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      if (arg2.size()>0) collConnect=arg2;
      else {
	std::cout << "ERROR: Expected input database connection string after '" << arg1 << "'" <<std::endl;
	return -1;
      }
      i++;
    } else if (arg1=="-q" || arg1=="--query") {
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      if (arg2.size()>0) collQuery=arg2;
      else {
	std::cout << "ERROR: Expected predicate string after '" << arg1 << "'" <<std::endl;
	return -1;
      }
      i++;
    } else if (arg1=="-c" || arg1=="--catalog") {
      std::string arg2;
      if ((i+1) < argc) arg2=argv[i+1];
      if (arg2.size()>0) catalogFile.push_back(arg2);
      else {
	std::cout << "ERROR: Expected catalog file name after '" << arg1 << "'" <<std::endl;
	return -1;
      }
      i++;
    } else if (arg1=="-t" || arg1=="--checkevents") {
      checkEvents=true;
    } else if (arg1=="-l" || arg1=="--listevents") {
      listEvents=true;
    } else {
      fileNames.push_back(arg1);
    }
  }// End loop over arguments

  std::map<Guid, std::string> guidPfn;
  std::map<std::string, std::vector<uint32_t> > searchEventByPFN;
  std::map<std::string, std::vector<long long int> > offsetEventByPFN;
  // If no event search set, then we need a POOL collections as input
  if (!searchEventSet && !collNames.empty()) {
    std::string refName("StreamRAW_ref");
    std::string runAtt("RunNumber");
    std::string eventAtt("EventNumber");
    pool::CollectionService* collSvc = new pool::CollectionService();
    pool::IFileCatalog* ctlg = new pool::IFileCatalog();
    try {
      if (!catalogFile.empty()) {
        ctlg->setWriteCatalog(*catalogFile.begin());
        for (std::vector<std::string>::const_iterator iter = catalogFile.begin(); iter != catalogFile.end(); ++iter) {
          ctlg->addReadCatalog(*iter);
        }
      } else {
        ctlg->setWriteCatalog("");
      }
      ctlg->connect();
    } catch(std::exception& e) {
      std::cout << e.what()  << std::endl;
      return -1;
    }
    ctlg->start();
    for (std::vector<std::string>::const_iterator collIter = collNames.begin(), typeIter = collTypes.begin(), collEnd = collNames.end(); collIter != collEnd; ++collIter, ++typeIter) {
      try {
        // Open the collection and execute the query
        pool::ICollection* srcColl = collSvc->handle(*collIter, *typeIter, collConnect, true);
        pool::ICollectionQuery* srcQuery = srcColl->newQuery();
        srcQuery->setCondition(collQuery);
        srcQuery->addToOutputList(refName+","+runAtt+","+eventAtt);
        pool::ICollectionCursor& cursor = srcQuery->execute();
        // iterate over query results
        while (cursor.next()) { 
          coral::AttributeList attList = cursor.currentRow().attributeList();
          const unsigned int* evNum = static_cast<unsigned int*>(attList[eventAtt].addressOfData());
          searchEvents.push_back(*evNum);
          const pool::TokenList& tokens = cursor.currentRow().tokenList();
          // Fill maps with guid, pfn, offset based on what is in StreamRAW token
          try {
            const Token* token = &tokens[refName];
            Guid guid = token->dbID();
            searchTokens.push_back(token->toString());
            long long int pos = token->oid().second;
            if (!token->contID().empty()) { 
               unsigned long long int cntID; 
               sscanf(token->contID().c_str(), "%08llX", &cntID); 
               pos += (long long int)(cntID<<32); 
            } 
            if (guidPfn.count(guid) == 0) {   // first instance of this guid
              std::string pfn;
              std::string type;
              ctlg->getFirstPFN(guid.toString(), pfn, type);
              // fill map of guid to pfn
              guidPfn.insert(std::pair<Guid, std::string>(guid, pfn));
              // add pfn to list
              fileNames.push_back(pfn);
              std::vector<uint32_t> evts;
              std::vector<long long int> offs;
              evts.push_back(*evNum);
              offs.push_back(pos);
              // fill map of pfn to event number list
              searchEventByPFN.insert(std::pair<std::string, std::vector<uint32_t> >(pfn, evts));
              // fill map of pfn to offset list
              offsetEventByPFN.insert(std::pair<std::string, std::vector<long long int> >(pfn, offs));
            } else {   // guid already in map guid to pfn map
              // get pfn for this guid
              const std::string pfn = guidPfn[guid];
              // append to event number list
              searchEventByPFN[pfn].push_back(*evNum);
              // append to offset list
              offsetEventByPFN[pfn].push_back(pos);
            }
          } catch (std::exception& e) {
            std::cout << "Unable to find StreamRAW for event = " << *evNum << std::endl;
            std::cout << e.what()  << std::endl;
            return -1;
          }
        }
      } catch (std::exception& e) {
        std::cout << "Reading collections encountered " << e.what() << std::endl;
        return -1;
      }
    }
    ctlg->commit();
  }
  if ( (!searchEventSet || fileNames.empty()) && collNames.empty()) {
    std::cout << "ERROR: Insufficient input specified!" << std::endl;
    return -1;
  }

  std::sort(searchEvents.begin(),searchEvents.end());
  std::cout << "Events to copy: ";
  for (std::vector<uint32_t>::const_iterator itEvt1=searchEvents.begin(), itEvt2=searchEvents.end(); itEvt1!=itEvt2; ++itEvt1) {
    std::cout << *itEvt1 << " ";
  }
  std::cout << std::endl;

  EventStorage::DataWriter* pDW=NULL;
  //start loop over files
  for (std::vector<std::string>::const_iterator it = fileNames.begin(), it_e = fileNames.end(); it != it_e; ++it) {
    const std::string& fName=*it;
    std::cout << "Checking file " << fName << std::endl;
    DataReader *pDR = pickDataReader(fName);
    if (!pDR) {
      std::cout << "Problem opening or reading this file!\n";
      return -1;
    }

    if (!pDW) { //Create data writer when reading the first file
      //Create DataWriter
      //Copy run_parameters_pattern from first file:
      EventStorage::run_parameters_record runPara = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      runPara.run_number=pDR->runNumber();  
      runPara.max_events=pDR->maxEvents(); 
      runPara.rec_enable=pDR-> recEnable();    
      runPara.trigger_type=pDR-> triggerType();  
      std::bitset<64> word1;
      std::bitset<64> word2;
      for (unsigned int i=0; i<64; ++i) {
         word1[i] = pDR->detectorMask()[i];
         word2[i] = pDR->detectorMask()[i+64];
      }
      runPara.detector_mask_LS=word1.to_ulong();
      runPara.detector_mask_MS=word2.to_ulong();
      runPara.beam_type=pDR->beamType();
      runPara.beam_energy=pDR->beamEnergy();

      std::string shortFileNameOut=fileNameOut;
      const std::string project(pDR->projectTag());
      const std::string streamName("");
      const std::string streamType("");
      const std::string stream(pDR->stream());
      const uint32_t lbnbr(pDR->lumiblockNumber());

      std::string dirNameOut=".";
      size_t p = fileNameOut.rfind('/');
      if (p != std::string::npos) {
	dirNameOut = fileNameOut.substr(0, p);
	shortFileNameOut = fileNameOut.substr(p + 1);
      }

      EventStorage::freeMetaDataStrings metaStrings;
      if (compressEvents) {
        pDW= new EventStorage::DataWriter(dirNameOut, shortFileNameOut, runPara, project, streamType,
	        streamName, stream, lbnbr, "AtlCopyBSEvent", metaStrings, EventStorage::ZLIB);
      } else {
        pDW= new EventStorage::DataWriter(dirNameOut, shortFileNameOut, runPara, project, streamType,
	        streamName, stream, lbnbr, "AtlCopyBSEvent", metaStrings);
      }
      pDW->setMaxFileMB(10000); //Max 10 metric GByte files
      if (!pDW->good() ) { 
	std::cout << "ERROR  Unable to initialize file "<< std::endl;
	delete pDW;
	delete pDR;
	return -1;
      }    
      std::cout << "Created DataWriter for file " << shortFileNameOut << " in directory " << dirNameOut << std::endl;
    }
    if (!pDR->good() || pDR->endOfFile()) {
      std::cerr << "No events in file "<< fName << std::endl;
      continue;
    }
    // Check if using collection or not
    if (searchEventByPFN.count(fName) > 0) {
      // sort events and offsets maps by key/pfn
      std::sort(searchEventByPFN[fName].begin(),searchEventByPFN[fName].end());
      std::sort(offsetEventByPFN[fName].begin(),offsetEventByPFN[fName].end());
      eventLoop(pDR, pDW, nFound, &searchEventByPFN[fName], searchRun, searchRunSet, listEvents, checkEvents, &offsetEventByPFN[fName]);
    } else {
      eventLoop(pDR, pDW, nFound, &searchEvents, searchRun, searchRunSet, listEvents, checkEvents);
    }
    delete pDR;
    if (nFound >= searchEvents.size() && nFound) break;
  }
  delete pDW;
  if (!nFound && searchEvents.size() > 0) {
    std::cout << "No events found!"  << std::endl;
    //return -1;  // Some use cases don't expect to find the events, just issue message
    return 0;
  } else if (nFound) {
    std::cout << "Wrote " << nFound << " events to file " << fileNameOut  << std::endl;
  } else {
    std::cout << "Copied all events to file " << fileNameOut  << std::endl;
  }
  return 0;
}

void eventLoop(DataReader* pDR, EventStorage::DataWriter* pDW, unsigned& nFound, const std::vector<uint32_t>* pSearchEvents, uint32_t searchRun, bool searchRunSet, bool listEvents, bool checkEvents, const std::vector<long long int>* pOffsetEvents) {
  using namespace eformat;
  // the event loop
  uint32_t eventCounter=0;
  std::vector<long long int>::const_iterator offIt, offEnd;
  if (pOffsetEvents != 0) {
    offIt = pOffsetEvents->begin();
    offEnd = pOffsetEvents->end();
  }
  while (pDR->good()) {
    unsigned int eventSize;    
    char *buf;
    DRError ecode;
    if (pOffsetEvents != 0) {
      if (offIt == offEnd) break;
      ecode = pDR->getData(eventSize, &buf, *offIt);
      ++offIt;
    } else {
      ecode = pDR->getData(eventSize,&buf);
    }
    if (DROK != ecode) {
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
        delete [] buf;
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
      if (checkEvents) fe.check_tree();
      
      uint32_t eventNo=fe.global_id();
      uint32_t runNo=fe.run_no();
      if (listEvents)
        std::cout << "Index=" << eventCounter <<" Run=" << runNo << " Event=" << eventNo 
                << " LB=" <<  fe.lumi_block() <<std::endl;
      
      if ((!searchRunSet || (runNo==searchRun)) && std::binary_search(pSearchEvents->begin(),pSearchEvents->end(),eventNo)) {
        nFound++;
        std::cout << std::endl;
        std::cout << "File:" <<  pDR->fileName() << std::endl;
        std::cout << "Event index: " << eventCounter << std::endl;
        std::cout << "Run:         " << runNo << std::endl;   
        std::cout << "Event ID:    " << eventNo << std::endl;
        std::cout << "LumiBlock:   "  << fe.lumi_block() << std::endl;
      
        //Write event to file
        uint32_t size = fe.fragment_size_word(); 
        pDW->putData(sizeof(uint32_t)*size, reinterpret_cast<void *>(fragment));
      } else if (pSearchEvents->size() == 0) {
        //Write event to file
        uint32_t size = fe.fragment_size_word(); 
        pDW->putData(sizeof(uint32_t)*size, reinterpret_cast<void *>(fragment));
      }
    } catch (eformat::Issue& ex) {
      std::cerr << "Uncaught eformat issue: " << ex.what() << std::endl;
    } catch (ers::Issue& ex) {
      std::cerr << "Uncaught ERS issue: " << ex.what() << std::endl;
    } catch (std::exception& ex) {
      std::cerr << "Uncaught std exception: " << ex.what() << std::endl;
    } catch (...) {
      std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
    }
    // end event processing 
    delete [] fragment;
  }
}
