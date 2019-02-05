/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Implementation file for the data object class
//The object is a map of channel number<->vector<string>
//Each string is the name of the guilty DCS parameter

#include "SCT_ConditionsData/SCT_DCSStatCondData.h"

#include <iostream>
#include <algorithm>
using namespace std;

//////////////////////////////////
//constructor
SCT_DCSStatCondData::SCT_DCSStatCondData():
  m_bad_channels{}
{
}
//////////////////////////////////
//add map entries
void SCT_DCSStatCondData::fill(const CondAttrListCollection::ChanNum& chanNum, const string param) {
  if (m_bad_channels.find(chanNum)!=m_bad_channels.end()) {
    // chan num has an entry already
    //get the parameter list for this chan num

    vector<string> par{(*m_bad_channels.find(chanNum)).second};

    vector<string>::iterator par_itr{find(par.begin(), par.end(), param)};
    if (par_itr==par.end()) {
      // if this parameter (hv, chanstat etc) doesn't exist in the list add it to the param vector
      par.push_back(param); 
      //don't insert! not a new map entry, just update the vector 
      (*m_bad_channels.find(chanNum)).second = par;
    }
  } else { 
    // no entry yet for this chan num, so start fresh
    vector<string> par;
    par.push_back(param);
    pair<map<CondAttrListCollection::ChanNum, vector<string> >::iterator, bool> successfulInsert{m_bad_channels.insert(make_pair(chanNum, par))};
    if (not successfulInsert.second) cout << "WARNING: SCT_ConditionsData map insert failed" << endl;
  }
}
//////////////////////////////////
//remove entries in map vector
void SCT_DCSStatCondData::remove(const CondAttrListCollection::ChanNum& chanNum, const string param) {
  map<CondAttrListCollection::ChanNum, vector<string> >::iterator itr{m_bad_channels.find(chanNum)};
  if (itr!=m_bad_channels.end()) {
    vector<string>::iterator vec_itr{find((*itr).second.begin(), (*itr).second.end(), param)};
    if (vec_itr!=(*itr).second.end()) {
      if ((*itr).second.size()>1) (*itr).second.erase(vec_itr);
      else m_bad_channels.erase(itr);
    }
  }
}
//////////////////////////////////
//output map vector
int SCT_DCSStatCondData::output(const CondAttrListCollection::ChanNum& chanNum, vector<string>& usersVector) const {
  DCSConditions::const_iterator pPair{m_bad_channels.find(chanNum)};
  if (pPair!=m_bad_channels.end()) {
    const vector<string>& v{pPair->second};
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
