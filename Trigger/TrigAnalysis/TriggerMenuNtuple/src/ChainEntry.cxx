/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ChainEntry.cxx
*/
#include <iostream>
#include "TriggerMenuNtuple/ChainEntry.h"

using namespace std;


void ChainEntry::addChain(const std::string& chain_name, int id) {
  std::map<int, std::string>::const_iterator p;
  bool foundit=false;
  for (p=s_ChainIdMap.begin(); p!=s_ChainIdMap.end(); ++p) {
    if (p->second == chain_name) {
      foundit = true;
      break;
    }
  }
  int i = static_cast<int>(s_ChainIdMap.size());
  if (id >= 0) i = id;
  if (!foundit) s_ChainIdMap[i] = chain_name;
}

const std::string ChainEntry::chainName(int id) {
  std::map<int, std::string>::const_iterator p;
  if ( (p=s_ChainIdMap.find(id)) != s_ChainIdMap.end()) {
    return p->second;
  } else {
    return "";
  }
}

int ChainEntry::chainId(const std::string& chain_name) {
  std::map<int, std::string>::const_iterator p;
  for (p=s_ChainIdMap.begin(); p!=s_ChainIdMap.end(); ++p) {
    if (p->second == chain_name) {
      return p->first;
    }
  }
  return -1;
}

const std::map<int, std::string>& ChainEntry::chainIdMap() {
  return s_ChainIdMap;
}

std::map<int, std::string> ChainEntry::s_ChainIdMap;


ChainEntry::ChainEntry(const std::string& chain_name, int chain_counter){
  m_Name = chain_name;
  m_Counter = chain_counter;
}

ChainEntry::~ChainEntry(){
}

void ChainEntry::addRoI(int type, int index){
  bool foundit = false;
  for (unsigned i=0; i<m_RoITypes.size(); ++i) {
    if (m_RoITypes[i] == type && m_RoIIndices[i] == index) {
      foundit = true;
      break;
    }
  }
  if (!foundit) {
    m_RoITypes.push_back(type);
    m_RoIIndices.push_back(index);
  }
}

void ChainEntry::dump() const {
  cout << (*this) << endl;
}

std::ostream& operator<<(std::ostream& o, const ChainEntry& x) {
  o << "ChainEntry: " << x.getName() << endl;
  std::vector<int> types = x.getRoITypes();
  std::vector<int> indices = x.getRoIIndices();
  for (unsigned int i=0; i<types.size(); ++i) {
    o << "  RoI type=" << types[i] << " index=" << indices[i] << endl;
  }
  return o;
}

