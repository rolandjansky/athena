/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>

namespace athenaMP_MemHelper
{
  int getPss(pid_t mother_pid, unsigned long& pss
	     , unsigned long& swap,unsigned long& rss
	     , unsigned long& size, bool verbose=false)
  {
    std::vector<pid_t> cpids;
    char smaps_buffer[32];

    // Open pipe to pstree
    snprintf(smaps_buffer,32,"pstree -A -p %ld | tr \\- \\\\n",(long)mother_pid);
    if(verbose) std::cout << "AthenaMP getPss. pstree command: " << smaps_buffer << std::endl;
    FILE* pipe = popen(smaps_buffer, "r");
    if (pipe==0) {
      std::cerr << "AthenaMP getPss. Error: unable to open pstree pipe!" << std::endl;
      return 1;
    }

    // Collect pid-s of child processes
    // NB! These will include 3 short-lived processes related to pstree
    char buffer[256];
    std::string result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 256, pipe) != NULL) {
	result += buffer;
	int pos(0);
	while(pos<256 && buffer[pos] != '\n' && buffer[pos] != '(') {
	  pos++;
	}

	if(pos<256 && buffer[pos] == '(' && pos>1 && buffer[pos-1] != '}') {
	  pos++;
	  pid_t pt(0);
	  while(pos<256 && buffer[pos] != '\n' && buffer[pos] != ')') {
	    pt=10*pt+buffer[pos]-'0';
	    pos++;
	  }
	  cpids.push_back(pt);
	  if(verbose) std::cout << "AthenaMP getPss. Collected PID="<< pt << std::endl;
	} // if(buffer[pos] == '(' && pos>1 && buffer[pos-1] != '}') 
      } // if(fgets(buffer, 256, pipe) != NULL)
    } // while(!feof(pipe))
    pclose(pipe);
    
    // Collect sum of Size/Pss/Rss/Swap for all of the processes
    long tsize(0);
    long trss(0);
    long tpss(0);
    long tswap(0);
    std::vector<std::string> openFails;
    for(std::vector<pid_t>::const_iterator it=cpids.begin(); it!=cpids.end(); ++it) {
      snprintf(smaps_buffer,32,"/proc/%ld/smaps",(long)*it);

      FILE *file = fopen(smaps_buffer,"r");
      if(file==0) {
	openFails.push_back(std::string(smaps_buffer));
      } else {
	while(fgets(buffer,256,file)) {
	  if(sscanf(buffer,"Size: %80ld kB",&tsize)==1) size+=tsize;
	  if(sscanf(buffer,"Pss: %80ld kB", &tpss)==1)  pss+=tpss;
	  if(sscanf(buffer,"Rss: %80ld kB", &trss)==1)  rss+=trss;
	  if(sscanf(buffer,"Swap: %80ld kB",&tswap)==1) swap+=tswap;
	}
	fclose(file);
      } // if(file==0)
    } // for(std::vector<pid_t>::const_iterator it=cpids.begin(); it!=cpids.end(); ++it) 

    // If fopen failed more than 3 times, then this might indicate an error
    // 3 is the number of the short-lived processes (see above), for them fopen is expected to fail
    if(openFails.size()>3) {
      std::cerr << "AthenaMP getPss. Error: too many failures in opening smaps files!" << std::endl;
      for(const std::string& str : openFails)
	std::cerr << "--> " << str << std::endl;
      return 1;
    }
    if(verbose) {
      std::cout << "AthenaMP getPss. Warning: failed to open the following files" << std::endl;
      for(const std::string& str : openFails)
	std::cout << "--> " << str << std::endl;
    }
    return 0;
  }
}
