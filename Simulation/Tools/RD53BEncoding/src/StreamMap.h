/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#ifndef STREAMMAP_H
#define STREAMMAP_H

#include "Identifier/Identifier.h"
#include "./Stream.h"

struct StreamMap {
  
  typedef std::map<const Identifier,std::vector<Stream>> stream_map;
  
  StreamMap() {};
  
  bool addStream(Identifier id, int chips) {
    
    for (int chip=0; chip<chips; chip++) {
      m_streamMap[id].push_back(Stream(id, chip));
    }
    
    if (m_streamMap[id].size()!=(unsigned int)chips) {
      std::cout << "ERROR!!! StreamMap for Identifier " << id << " contains more entries than expected!" << std::endl;
      std::cout << "-------- Check how it is filled!!! --------" << std::endl;
      return false;
    }
    
    return true;
  };
  
  Stream& getStream(Identifier id, int chip) {
//     if (m_streamMap.find(id)==m_streamMap.end()) {
//       std::cout << "ERROR!!! Identifier not found in the StreamMap!" << std::endl;
//       std::cout << "-------- returning empty Stream --------" << std::endl;
//       return Stream(id, chip);
//     }
    
    return m_streamMap[id].at(chip);
  } 
  
  void reset() {
    for (auto& elements : m_streamMap) {
      for (auto& stream : elements.second) {
        stream.reset();
      }
    }
  }
  
  stream_map& getStreams() {
    return m_streamMap;
  }
    
  stream_map m_streamMap;
  
};


#endif // STREAMMAP_H

