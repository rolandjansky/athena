/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

*/

#ifndef STREAM_H
#define STREAM_H

#include "Identifier/Identifier.h"

struct Stream {
  
  Stream(Identifier id, int chip=0) : 
  m_id(id), m_chip(chip)
  {    
    m_streams    .reserve(100);
    m_tags       .reserve(100);
    m_addressing .reserve(100);
    m_tot        .reserve(100);
    m_data       .reserve(100);
    m_orphans    .reserve(100);
    m_cores      .reserve(100);
  };
  
  void reset() {
    m_streams    .clear();
    m_tags       .clear();
    m_addressing .clear();
    m_tot        .clear();
    m_data       .clear();
    m_orphans    .clear();
    m_cores      .clear();    
  }
  
  std::vector<float>& getStreams () {
    return m_streams;
  }
  
  std::vector<float>& getCores () {
    return m_cores;
  }
  
  std::vector<float>& getOrphans () {
    return m_orphans;
  }
  
  std::vector<float>& getTags () {
    return m_tags;
  }
  
  std::vector<float>& getAddressing () {
    return m_addressing;
  }
  
  std::vector<float>& getTot () {
    return m_tot;
  }
  
  std::vector<float>& getData () {
    return m_data;
  }
  
  Identifier identify() {
    return m_id;    
  }
  
  int chip() {
    return m_chip;    
  }
  
  std::vector<float> m_streams;
  std::vector<float> m_orphans;
  std::vector<float> m_cores;
  std::vector<float> m_tags;
  std::vector<float> m_addressing;
  std::vector<float> m_tot;
  std::vector<float> m_data;
  
  Identifier m_id;
  int m_chip;
};


#endif // STREAM_H
