/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/ScoutingInfo.h"


ScoutingInfo::ScoutingInfo() {
}
ScoutingInfo::~ScoutingInfo() {
}

void ScoutingInfo::add(CLID clid, const std::string& name){
  
  std::pair < CLID, std::string > clid_Name (clid, name);
  m_clidName.insert(m_clidName.end(), clid_Name);
}

void ScoutingInfo::add(const std::pair<CLID, std::string>& clid_Name){
  
  m_clidName.insert(m_clidName.end(), clid_Name);
}

void ScoutingInfo::add(const std::vector< std::pair<CLID, std::string> >& clid_Name){

  for(std::vector< std::pair<CLID, std::string> >::const_iterator it = clid_Name.begin(); 
      it != clid_Name.end(); ++it)
    m_clidName.insert(m_clidName.end(), *it);
}

void ScoutingInfo::add(const std::set< std::pair<CLID, std::string> >& clid_Name){

  for(std::set< std::pair<CLID, std::string> >::const_iterator it = clid_Name.begin(); 
      it != clid_Name.end(); ++it)
    m_clidName.insert(m_clidName.end(), *it);
}

void ScoutingInfo::get(std::set< std::pair< CLID, std::string> >& clid_Name) const {

  for(std::set< std::pair<CLID, std::string> >::const_iterator it = m_clidName.begin(); 
      it != m_clidName.end(); ++it)
    clid_Name.insert(clid_Name.end(), *it);
}

const std::set< std::pair< CLID, std::string> >& ScoutingInfo::get() const {

  return m_clidName;
}

void ScoutingInfo::getclid(std::vector<CLID>& clid) const{
  
  for(std::set< std::pair<CLID, std::string> >::const_iterator it = m_clidName.begin(); 
      it != m_clidName.end(); ++it)
    clid.push_back((*it).first);
}

void ScoutingInfo::getName(std::vector<std::string>& name) const{

  for(std::set< std::pair<CLID, std::string> >::const_iterator it = m_clidName.begin(); 
      it != m_clidName.end(); ++it)
    name.push_back((*it).second);
}

void ScoutingInfo::clear() {
  m_clidName.clear();
}

void ScoutingInfo::merge(const ScoutingInfo& sI) {
  
  m_clidName.insert(sI.m_clidName.begin(),sI.m_clidName.end());
}
