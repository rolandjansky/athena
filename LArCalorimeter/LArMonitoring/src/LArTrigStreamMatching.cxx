/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTrigStreamMatching.h"

std::vector<unsigned> LArMon::trigStreamMatching(const std::vector<std::string>& streamsFromJobO, const std::vector<xAOD::EventInfo::StreamTag>& streamInEvent) {

  //The return value. Relying on RVO not to copy it when returning to caller
  std::vector<unsigned> foundStreams;
  const unsigned nStreamsInJobO=streamsFromJobO.size();
  for (unsigned i=0;i<nStreamsInJobO;++i) {
    bool found=false;
    for (const xAOD::EventInfo::StreamTag& streamTag : streamInEvent) {
      if (streamsFromJobO[i]==streamTag.name()) {
	foundStreams.push_back(i);
	found=true;
	break; //inner loop
      }//end-if
    }//end inner loop (over streamInEvent)
    //Not-found case:
    if (!found) foundStreams.push_back(nStreamsInJobO);
  }//end outer loop (over streamInJobO)
  
  return foundStreams;
}

