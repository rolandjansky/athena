/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class      DumpFrags
   @brief      Dump eformat fragments from FullEventFragment
*/

#ifndef BYTESTREAMCNVSVC_DUMPFRAGS
#define BYTESTREAMCNVSVC_DUMPFRAGS


#include <iostream> 
#include "ByteStreamData/RawEvent.h"

class DumpFrags
{
public:
  DumpFrags() {} 

  /// dump fragments from FullEventFragment
  static void dump(const RawEvent* re)
  {
    std::cout << " ---FullEventFragment run number = " 
	      << re->run_no() << " Size = " << re->fragment_size_word() << std::endl;

    std::cout<<" Global id  "<<re->global_id()<<std::endl;
    std::cout<<" Run type  "<<re->run_type()<<std::endl;
    std::cout<<" Lumi block   "<<re->lumi_block()<<std::endl;
    std::cout<<" level 1 id   "<<re->lvl1_id()<<std::endl;
    std::cout<<" Bunch crossing id   "<<re->bc_id()<<std::endl;
    std::cout<<" Bunch crossing time in seconds  "<<re->bc_time_seconds()<<std::endl;
    std::cout<<" Bunch crossing time nanoseconds offset "<<re->bc_time_nanoseconds()<<std::endl;
    std::cout<<" Level 1 trigger type   "<<re->lvl1_trigger_type()<<std::endl;

    const OFFLINE_FRAGMENTS_NAMESPACE::DataType * buffer       ;
    re->lvl1_trigger_info(buffer)                              ;
    uint32_t n = re->nlvl1_trigger_info()               ;
    std::cout << " Lvl1 trigger info , size =   " <<n <<std::endl; 

    for(uint32_t i=0;i<n;++i) {
      std::cout << "   "<< *buffer;
      ++buffer; 
    }

    std::cout << std::endl;

    re->lvl2_trigger_info(buffer)                              ;
    n = re->nlvl2_trigger_info()               ;
    std::cout << " Lvl2 trigger info, size =   " <<n <<std::endl; 

    for(uint32_t i=0;i<n;++i) {
      std::cout << "   "<< *buffer;
      ++buffer; 
    }
    std::cout << std::endl;

    re->event_filter_info(buffer)                              ;
    n = re->nevent_filter_info()               ;
    std::cout << " event filter trigger info, size =   " <<n <<std::endl; 

    for(uint32_t i=0;i<n;++i) {
      std::cout << "   "<< *buffer;
      ++buffer; 
    }
    std::cout << std::endl;


    // get all the ROBFragments
    const size_t MAX_ROBFRAGMENTS = 2048;
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType robF[MAX_ROBFRAGMENTS];
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rePointer;
    re->start(rePointer);
    size_t robcount = re->children(robF,MAX_ROBFRAGMENTS);
    if (robcount == MAX_ROBFRAGMENTS)
      {
	std::cout << "ERROR : ROB buffer overflow" << std::endl;
      }
  
    // loop over all ROBs
    for (size_t irob=0; irob<robcount; ++irob)
      {
	  // add to the map
	  OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(robF[irob]);
	  
	  std::cout << "        ROBFragment, src ID ="
		    << std::hex << rob.source_id()
		    << " size_word =" << rob.fragment_size_word() << std::endl; 
	  
	  // dump ROD
	  std::cout << "        RODFragment, src ID ="
		    << std::hex << rob.rod_source_id()
		    << " size_word =" << rob.rod_fragment_size_word() << std::endl;
	  std::cout << "        RODFragment, run number ="
		    << std::dec << rob.rod_run_no() << std::endl;       

          if (rob.rod_fragment_size_word() >= rob.rod_header_size_word() + rob.rod_trailer_size_word()) {
	    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodPointer;
	    rob.rod_data(rodPointer);
	    for(size_t i=0; i<rob.rod_ndata(); ++i)
	    {
	      if (i%10 == 0) std::cout << std::endl << "           ";
	      std::cout << std::hex<< *rodPointer << " " ; 
	      ++rodPointer; 
	    }
	    std::cout << std::endl << "          Number of words in ROD = " 
		      << rob.rod_ndata() << std::endl;
          } else {
	    std::cout << std::endl << "WARNING : ROD has unexpected data size " 
		      << rob.rod_ndata() << " and RODFragment size_word is too small!" << std::endl;
          }
      } 
  }
};
  
#endif 

