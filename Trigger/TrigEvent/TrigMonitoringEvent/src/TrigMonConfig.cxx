/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <sstream>

#include "AthenaKernel/errorcheck.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"

//--------------------------------------------------------------------------------------  
TrigMonConfig::TrigMonConfig()
  :m_event(0),
   m_lumi(0),
   m_run(0),
   m_sec(0),
   m_nsec(0),
   m_master_key(0),
   m_hlt_ps_key(0),
   m_lv1_ps_key(0)
{
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::clear()
{
  //
  // Erase all configuration data
  //
  m_event = 0;
  m_lumi  = 0;
  m_run   = 0;
  m_sec   = 0;
  m_nsec  = 0;

  m_master_key = 0;
  m_hlt_ps_key = 0;
  m_lv1_ps_key = 0;

  m_chain.clear();
  m_seq.clear();
  m_var_id.clear();
  m_var_name.clear();
  m_pair_key.clear();
  m_pair_val.clear();
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::clearStrings()
{
  //
  // Clear string variables
  //
  for(unsigned int i = 0; i < m_chain.size(); ++i) m_chain[i].clearStrings();
  for(unsigned int i = 0; i < m_seq.size();   ++i) m_seq  [i].clearStrings();
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::processConfig ATLAS_NOT_THREAD_SAFE ()
{
  //
  // Sync TrigConfVar with global static vector in TrigConfVar.cxx file
  //

  // First, save global vector
  const std::vector<TrigConfVar> var_vec = Trig::GetCurrentTrigConfVarVector();
  
  // Add our own configuration
  if(m_var_name.size() == m_var_id.size()) {
    for(unsigned int i = 0; i < m_var_id.size(); ++i) { 
      Trig::ReserveVarId(m_var_name[i], m_var_id[i]);
    }
  }

  // Save global data
  for(unsigned int i = 0; i < var_vec.size(); ++i) { 
    if(std::find(m_var_name.begin(), m_var_name.end(), var_vec[i]) == m_var_name.end()) {
      add<TrigConfVar>(var_vec[i]);
    }
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::setEventID(uint32_t event,
			       uint32_t lumi,
			       uint32_t run,
			       uint32_t sec,
			       uint32_t nsec)
{
  //
  // Fill EventID data
  //
  m_event = event;
  m_lumi  = lumi;
  m_run   = run;
  m_sec   = sec;
  m_nsec  = nsec;
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::setTriggerKeys(uint32_t master_key,
				   uint32_t lv1_ps_key,
				   uint32_t hlt_ps_key)
{
  //
  // Save trigger configurations keys
  //
  m_master_key = master_key;
  m_lv1_ps_key = lv1_ps_key;
  m_hlt_ps_key = hlt_ps_key;
}

//--------------------------------------------------------------------------------------  
void TrigMonConfig::addValue(const std::string &key, const std::string &val)
{
  //
  // Save string pair 
  //
  m_pair_key.push_back(key);
  m_pair_val.push_back(val);
}

//--------------------------------------------------------------------------------------  
bool TrigMonConfig::keyExists(const std::string &key) const
{
  //
  // Find key and copy its value
  //
  if(m_pair_key.size() != m_pair_val.size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig") << "KeyExists - logic error!";
    return false;
  }

  for(unsigned int i = 0; i < m_pair_key.size(); ++i) {
    if(m_pair_key[i] == key) {
      return true;
    }
  }
  
  return false;
}

//--------------------------------------------------------------------------------------  
bool TrigMonConfig::readValue(const std::string &key, std::string &val) const
{
  //
  // Find key and copy its value
  //
  if(m_pair_key.size() != m_pair_val.size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig") << "ReadValue - logic error!";
    return false;
  }

  for(unsigned int i = 0; i < m_pair_key.size(); ++i) {
    if(m_pair_key[i] == key) {
      val = m_pair_val[i];
      return true;
    }
  }
  
  return false;
}

//--------------------------------------------------------------------------------------  
uint32_t TrigMonConfig::getId(const std::string &name, const std::string &key) const
{
  //
  // convert name to 32 bits hash id - unique for given name
  //
  
  if(key == "CHN") {
    const std::vector<TrigConfChain>::const_iterator fit = std::find(m_chain.begin(),
								    m_chain.end(), name);
    if(fit != m_chain.end()) {
      return fit -> getId();
    }
    else {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
        << "getId - failed to find chain: " << name;
      return 0;
    }
  } else if(key == "SEQ") {
    const std::vector<TrigConfSeq>::const_iterator fit = std::find(m_seq.begin(), m_seq.end(), name);
    if(fit != m_seq.end()) {
      return fit -> getId();
    }
    else {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
        << "getId - failed to find sequence: " << name;
      return 0;
    }
  } else if(key.find("ALG") != std::string::npos) {
    // iterate over all configured sequences
    for(unsigned int i = 0; i < m_seq.size(); ++i) {
      const TrigConfSeq &seq = m_seq[i];
      const std::vector<TrigConfAlg> &avec = seq.getAlg();
      
      // iterate over algorithms
      for(unsigned int j = 0; j < avec.size(); ++j) {
	      const TrigConfAlg &alg = avec[j];
	      if(alg.getName() == name) return alg.getNameId();
      }
    }
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
      << "getId - ERROR! Failed to find algorithm: " << name << ", " << key;
    return 0;
  }
  REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
    << "getId - ERROR! Failed to match: " << name << ", " << key;
  return 0;
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonConfig::getEncodedId(const std::string &name, const std::string &key) const
{
  //
  // convert name to 16 bits encoded id - unique for given configuration
  //
  
  if(key == "CHN") {
    const std::vector<TrigConfChain>::const_iterator fit = std::find(m_chain.begin(),
								     m_chain.end(), name);
    if(fit != m_chain.end()) {
      return fit -> getEncodedId();
    }
    else {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
        << "getEncoded - failed to find chain: " << name;
      return 0;
    }
  }
  REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
    << "getEncoded(" << name << ", " << key << ") - no match found";
  return 0;
}

//--------------------------------------------------------------------------------------  
const std::string TrigMonConfig::getName(uint32_t id, const std::string &key) const
{
  //
  // convert hash id to unique name
  //
  
  if(key == "CHN") {
    const std::vector<TrigConfChain>::const_iterator fit = std::find(m_chain.begin(),
								    m_chain.end(), id);
    if(fit != m_chain.end()) {
      return fit -> getName();
    }
    else {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
        << "getId - failed to find chain: " << id;
      return "";
    }
  } else if(key == "SEQ") {
    const std::vector<TrigConfSeq>::const_iterator fit = std::find(m_seq.begin(), m_seq.end(), id);
    if(fit != m_seq.end()) {
      return fit -> getName();
    }
    else {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
        << "getId - failed to find sequence: " << id;
      return "";
    }
  } else if(key.find("ALG") != std::string::npos){
    // iterate over all configured sequences
    for(unsigned int i = 0; i < m_seq.size(); ++i) {
      const TrigConfSeq &seq = m_seq[i];
      const std::vector<TrigConfAlg> &avec = seq.getAlg();
      
      // iterate over algorithms
      for(unsigned int j = 0; j < avec.size(); ++j) {
	const TrigConfAlg &alg = avec[j];

	if(alg.getNameId() == id) return alg.getName();
      }
    }
    REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
      << "getId - failed to find algorithm: " << id;
    return "";
  }

  REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR, "TrigMonConfig")
    << "getId - failed to match: " << id << ", " << key;

  return "";
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigConfVar> TrigMonConfig::getConfVar() const
{
  //
  // Build variables on a fly and return vector by value
  //
  std::vector<TrigConfVar> var;

  if(m_var_name.size() == m_var_id.size()) {
    //
    // Iterate over keys and values
    //
    var.reserve(m_var_name.size());

    for(unsigned int i = 0; i < m_var_name.size(); ++i) {
      if(m_var_id[i] != 0) {
  	    var.push_back(TrigConfVar(m_var_name[i], m_var_id[i]));
      }
    }
  }

  return var;
}
