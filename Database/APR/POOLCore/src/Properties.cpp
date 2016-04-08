/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/Properties.h"

pool::Properties::Properties():m_properties(),m_flags(){
}

pool::Properties::~Properties(){
}

bool pool::Properties::setProperty(const std::string& propertyName, const std::string& propertyValue){
  bool ret = false;
  if(m_properties.find(propertyName)!=m_properties.end()){
    ret = true;
    m_properties.erase(propertyName);
  }
  m_properties.insert(std::make_pair(propertyName,propertyValue));
  return ret;
}

void pool::Properties::setFlag(const std::string& flagName){
  m_flags.insert(flagName);
}

bool pool::Properties::hasProperty(const std::string& propertyName) const {
  return (m_properties.find(propertyName)!=m_properties.end());
}

std::string pool::Properties::getProperty(const std::string& propertyName) const {
  std::string ret("");
  std::map<std::string,std::string>::const_iterator iP = m_properties.find(propertyName);
  if(iP!=m_properties.end()){
    ret = iP->second;
  }
  return ret;
}

bool pool::Properties::getFlag(const std::string& flagName) const {
  bool ret = false;
  if(m_flags.find(flagName)!=m_flags.end()){
    ret = true;
  }
  return ret;
}

bool pool::Properties::removeProperty(const std::string& propertyName){
  bool ret = false;
  if(m_properties.find(propertyName)!=m_properties.end()){
    ret = true;
    m_properties.erase(propertyName);
  }
  return ret;
}

bool pool::Properties::removeFlag(const std::string& flagName){
  bool ret = false;
  if(m_flags.find(flagName)!=m_flags.end()){
    ret = true;
    m_flags.erase(flagName);
  }
  return ret;
}



