/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelDCSCondData.h"

PixelDCSCondData::PixelDCSCondData():
  m_channelValues()
{
}

PixelDCSCondData::~PixelDCSCondData() { }

void PixelDCSCondData::setValue(const CondAttrListCollection::ChanNum& chanNum, const float value) {
  m_channelValues[chanNum] = value;
}

bool PixelDCSCondData::getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const {
  auto itr{m_channelValues.find(chanNum)};
  if (itr!=m_channelValues.end()) {
    value = itr->second;
    return true;
  }
  return false;
}

void PixelDCSCondData::clear() {
  m_channelValues.clear();
}

void PixelDCSCondData::fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param) {
  if (m_bad_channels.find(chanNum)!=m_bad_channels.end()) {
    // chan num has an entry already
    //get the parameter list for this chan num
    std::vector<std::string> par((*m_bad_channels.find(chanNum)).second);
    std::vector<std::string>::iterator par_itr(find(par.begin(), par.end(), param));
    if (par_itr==par.end()) {
      // if this parameter (hv, chanstat etc) doesn't exist in the list add it to the param vector
      par.push_back(param); 
      //don't insert! not a new map entry, just update the vector 
      (*m_bad_channels.find(chanNum)).second = par;
    }
  } 
  else { 
    // no entry yet for this chan num, so start fresh
    std::vector<std::string> par;
    par.push_back(param);
    std::pair<std::map<CondAttrListCollection::ChanNum, std::vector<std::string>>::iterator, bool> successfulInsert(m_bad_channels.insert(std::make_pair(chanNum, par)));
    if (not successfulInsert.second) {
      std::cout << "map insert failed" << std::endl;
    }
  }
}

int PixelDCSCondData::output(const CondAttrListCollection::ChanNum& chanNum) const {
  DCSConditions::const_iterator pPair(m_bad_channels.find(chanNum));
  return (pPair!=m_bad_channels.end()) ? (pPair->second.size()) : 0;
}

