/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Implementation file for the data object class
//The object is a map of channel number<->vector<string>
//Each string is the name of the guilty DCS parameter

#include "SCT_ConditionsData/SCT_DCSStatCondData.h"

#include <iostream>
#include <algorithm>

//////////////////////////////////
//constructor
SCT_DCSStatCondData::SCT_DCSStatCondData():
  m_bad_channels{}
{
}
//////////////////////////////////
//add map entries
void SCT_DCSStatCondData::fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param) {
  if (m_bad_channels.find(chanNum)!=m_bad_channels.end()) {
    // chan num has an entry already
    //get the parameter list for this chan num

    std::vector<std::string> par{(*m_bad_channels.find(chanNum)).second};

    std::vector<std::string>::iterator par_itr{std::find(par.begin(), par.end(), param)};
    if (par_itr==par.end()) {
      // if this parameter (hv, chanstat etc) doesn't exist in the list add it to the param std::vector
      par.push_back(param); 
      //don't insert! not a new map entry, just update the std::vector 
      (*m_bad_channels.find(chanNum)).second = par;
    }
  } else { 
    // no entry yet for this chan num, so start fresh
    std::vector<std::string> par;
    par.push_back(param);
    std::pair<std::map<CondAttrListCollection::ChanNum, std::vector<std::string> >::iterator, bool> successfulInsert{m_bad_channels.insert(make_pair(chanNum, par))};
    if (not successfulInsert.second) std::cout << "WARNING: SCT_ConditionsData map insert failed" << std::endl;
  }
}
//////////////////////////////////
//remove entries in map vector
void SCT_DCSStatCondData::remove(const CondAttrListCollection::ChanNum& chanNum, const std::string param) {
  std::map<CondAttrListCollection::ChanNum, std::vector<std::string> >::iterator itr{m_bad_channels.find(chanNum)};
  if (itr!=m_bad_channels.end()) {
    std::vector<std::string>::iterator vec_itr{std::find((*itr).second.begin(), (*itr).second.end(), param)};
    if (vec_itr!=(*itr).second.end()) {
      if ((*itr).second.size()>1) (*itr).second.erase(vec_itr);
      else m_bad_channels.erase(itr);
    }
  }
}
//////////////////////////////////
//output map vector
int SCT_DCSStatCondData::output(const CondAttrListCollection::ChanNum& chanNum, std::vector<std::string>& usersVector) const {
  DCSConditions::const_iterator pPair{m_bad_channels.find(chanNum)};
  if (pPair!=m_bad_channels.end()) {
    const std::vector<std::string>& v{pPair->second};
    usersVector.insert(usersVector.begin(), v.begin(), v.end());
  }
  return usersVector.size();
}
//////////////////////////////////
//output map size
int SCT_DCSStatCondData::output(const CondAttrListCollection::ChanNum& chanNum) const {
  DCSConditions::const_iterator pPair{m_bad_channels.find(chanNum)};
  return (pPair!=m_bad_channels.end()) ? (pPair->second.size()) : 0;
}
