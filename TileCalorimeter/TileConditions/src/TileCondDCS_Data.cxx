/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Implementation file for the data object class
//The object is a map of channel number<->vector<string>
//Each string is the name of the guilty DCS parameter

#include "TileConditions/TileCondDCS_Data.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <utility>


using namespace std;
//constructor
TileCondDCS_Data::TileCondDCS_Data()
  : m_pModulesHV(0)
{}
//////////////////////////////////
//destructor
TileCondDCS_Data::~TileCondDCS_Data()
{ }
//////////////////////////////////

//To clean
StatusCode 
TileCondDCS_Data::initialize()
{
  m_pModulesHV = new map<CondAttrListCollection::ChanNum, float >;
  return StatusCode::SUCCESS;
}

StatusCode 
TileCondDCS_Data::finalize()
{
  return StatusCode::SUCCESS;
}

//////////

//add map entries
void TileCondDCS_Data::fill(const CondAttrListCollection::ChanNum & chanNum, const string& param)
{
  if (m_bad_channels.find(chanNum) != m_bad_channels.end()) {
    vector<string> par= (*m_bad_channels.find(chanNum)).second;
    vector<string>::iterator par_itr=find(par.begin(),par.end(),param);
    if (par_itr == par.end()){
      par.push_back(param);
      m_bad_channels.insert(make_pair(chanNum, par));
    }
  }
  else {
    vector<string> par;
    par.push_back(param);
    m_bad_channels.insert(make_pair(chanNum, par));
  }
}

//////////////////////////////////
//remove entries in map vector
void TileCondDCS_Data::remove(const CondAttrListCollection::ChanNum & chanNum, const string& param)
{
  map< CondAttrListCollection::ChanNum,vector<string> >::iterator itr=m_bad_channels.find(chanNum);
  if (itr != m_bad_channels.end()) {
    vector<string>::iterator vec_itr=find((*itr).second.begin(),(*itr).second.end(),param); 
    if (vec_itr != (*itr).second.end()){
      (*itr).second.erase(vec_itr);
    }
  }
}
//////////////////////////////////
//output map vector
int TileCondDCS_Data::output(const CondAttrListCollection::ChanNum & chanNum, vector< string > & usersVector){
  DCSConditions::const_iterator pPair(m_bad_channels.find(chanNum) );
  if ( pPair != m_bad_channels.end()){
    const vector< string > & v(pPair->second);
    usersVector.insert(usersVector.begin(), v.begin(), v.end() );
  }
  return usersVector.size();
}

//output map size
int TileCondDCS_Data::output(const CondAttrListCollection::ChanNum & chanNum){
  DCSConditions::const_iterator pPair(m_bad_channels.find(chanNum) );
  return (pPair != m_bad_channels.end() )? (pPair->second.size()) : 0;
}

