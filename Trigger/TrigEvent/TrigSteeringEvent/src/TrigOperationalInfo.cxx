/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include <sstream>
#include <cmath>


TrigOperationalInfo::TrigOperationalInfo() {
  
}

TrigOperationalInfo::TrigOperationalInfo(const std::vector<std::string>& keys,
                                         const std::vector<float>& values)
  : m_infoName (keys),
    m_infoValue (values)
{
}

unsigned int TrigOperationalInfo::defined(const std::string& name) const {
  return count( m_infoName.begin(), m_infoName.end(), name ); 
}

float TrigOperationalInfo::get(const std::string& name) const {
  std::vector<std::string>::const_iterator it;
  it = find ( m_infoName.begin(), m_infoName.end(), name ); 
  return m_infoValue[it - m_infoName.begin()];
}

void TrigOperationalInfo::set(const std::string& name, float value) {
  // Tim Martin - April 2016. Removed code which enforces unique names. Rquired by use in Cost Monitoring. 
  m_infoName.push_back(name);
  m_infoValue.push_back(value);
}

std::pair<std::vector<std::string>, std::vector<float> > TrigOperationalInfo::infos() const {
  return std::make_pair(m_infoName, m_infoValue);
}

const std::vector<std::string>& TrigOperationalInfo::getKeys() const  {
  return m_infoName;
}

const std::vector<float>& TrigOperationalInfo::getValues() const {
  return m_infoValue;
}

void TrigOperationalInfo::updateAtLocation(unsigned int location, float value) {
  m_infoValue.at(location) = value;
}

//////////////////////////////////////////////////////////////////
// helper operators


std::string str ( const TrigOperationalInfo& d ) {
  std::stringstream ss;
  std::pair<std::vector<std::string>, std::vector<float> > quantities = d.infos();
  std::vector<std::string>::const_iterator keyIt = quantities.first.begin();
  std::vector<float>::const_iterator valueIt     = quantities.second.begin();
  for (unsigned int i = 0; i < quantities.first.size(); i++ ) {
    ss << " " << *keyIt << ": " << *valueIt;
    ++keyIt;
    ++valueIt;
  }
  return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigOperationalInfo& d ) {
  m << str(d);
  return m;
}


bool operator== ( const TrigOperationalInfo& a, const TrigOperationalInfo& b ) {
  
  std::pair<std::vector<std::string>, std::vector<float> > a_infos = a.infos();
  std::pair<std::vector<std::string>, std::vector<float> > b_infos = b.infos();
  if ( a_infos.first.size() != b_infos.first.size()) 
    return false;
  if ( ! (a_infos.first  == b_infos.first))
    return false;

  if ( ! (a_infos.second  == b_infos.second))
    return false;
  
  return true;
}
void diff( const TrigOperationalInfo& a, const TrigOperationalInfo& b, std::map<std::string, double>& variableChange ) {
  std::pair<std::vector<std::string>, std::vector<float> > a_infos = a.infos();
  std::pair<std::vector<std::string>, std::vector<float> > b_infos = b.infos();

  if ( a_infos.first.size() != b_infos.first.size()) 
    variableChange["size"] =  a_infos.first.size() - b_infos.first.size();

  std::vector<std::string>::const_iterator a_keyIt = a_infos.first.begin();
  std::vector<float>::const_iterator a_valueIt     = a_infos.second.begin();
  for ( unsigned i = 0; i < a_infos.first.size(); i++ ) {
    if ( b.defined(*a_keyIt) ) {
      float b_val = b.get(*a_keyIt);
      variableChange[*a_keyIt] = *a_valueIt - b_val;
    } else {
      variableChange[*a_keyIt+"_abs"] = *a_valueIt;
    }

    ++a_keyIt;
    ++a_valueIt;
  }
}
