/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_H
#define TRIGMON_CONFIG_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief This class holds encoded ids for monitoring objects 
   and helps to map these objects to trigger configuration.
*/

// Framework
#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"

// C/C++
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigConfChain.h"
#include "TrigMonitoringEvent/TrigConfSeq.h"
#include "TrigMonitoringEvent/TrigConfVar.h"

class TrigMonConfig
{
 public:
  
  TrigMonConfig();
  ~TrigMonConfig() {}
  
  static std::string getType() { return "TrigMonConfig"; }
  
  template<class T> typename std::vector<T>::const_iterator begin() const;
  template<class T> typename std::vector<T>::const_iterator end() const;
  
  template<class T> bool add(const T &obj);

  template<class T> std::vector<T>& getVec();

  template<class T> const std::vector<T>& getVec() const;

  template<class T> unsigned int size() const; 

  template<class T> const T& at(unsigned int i) const; 

  template<class T> typename std::vector<TrigConfChain>::const_iterator findChain(const T& key) const;
  template<class T> typename std::vector<TrigConfSeq>::const_iterator   findSeq(const T& key) const;

  template<class T> typename std::vector<TrigConfChain>::iterator findChainIter(const T& key);
  template<class T> typename std::vector<TrigConfSeq>::iterator   findSeqIter(const T& key); 
  
  void clear();
  void clearStrings();
  void processConfig ATLAS_NOT_THREAD_SAFE ();
  
  void setEventID(uint32_t event, uint32_t lumi, uint32_t run, 
		  uint32_t sec,   uint32_t nsec);

  void setTriggerKeys(uint32_t master_key, uint32_t lv1_ps_key, uint32_t hlt_ps_key);
  
  void addValue(const std::string &key, const std::string &val);
  
  std::vector<std::string>&  getVarName() { return m_var_name; }
  std::vector<uint32_t>&     getVarId()   { return m_var_id;   }
  std::vector<std::string>&  getPairKey() { return m_pair_key; }
  std::vector<std::string>&  getPairVal() { return m_pair_val; }

  uint32_t getEvent()   const { return m_event; }
  uint32_t getLumi()    const { return m_lumi; }
  uint32_t getRun()     const { return m_run; }  
  uint32_t getSec()     const { return m_sec; }
  uint32_t getNanoSec() const { return m_nsec; }
  
  uint32_t getMasterKey()      const { return m_master_key; }
  uint32_t getHLTPrescaleKey() const { return m_hlt_ps_key; }
  uint32_t getLV1PrescaleKey() const { return m_lv1_ps_key; }
  
  bool keyExists(const std::string &key) const;
  bool readValue(const std::string &key, std::string &val) const;
  
  uint32_t          getId       (const std::string &name, const std::string &key) const;
  uint16_t          getEncodedId(const std::string &name, const std::string &key) const;
  const std::string getName     (uint32_t id,             const std::string &key) const;    
  
  const std::vector<TrigConfVar> getConfVar() const;

  const std::vector<std::string>&  getVarName() const { return m_var_name; }
  const std::vector<uint32_t>&     getVarId()   const { return m_var_id; }

  const std::vector<std::string>&  getPairKey() const { return m_pair_key; }
  const std::vector<std::string>&  getPairVal() const { return m_pair_val; }

 private:
  
  // These variables are filled from EventID object
  uint32_t                      m_event;      // Event number
  uint32_t                      m_lumi;       // Lumi block
  uint32_t                      m_run;        // Run number
  uint32_t                      m_sec;        // Event second
  uint32_t                      m_nsec;       // Event ns

  // Trigger configuration keys
  uint32_t                      m_master_key; // Configuration key (SMK)
  uint32_t                      m_hlt_ps_key; // HLT prescale key
  uint32_t                      m_lv1_ps_key; // L1  prescale key
  
  std::vector<TrigConfChain>    m_chain;      // Chain configuration
  std::vector<TrigConfSeq>      m_seq;        // Sequence configuration

  std::vector<std::string>      m_var_name;   // Variable configuration: (name, id) pairs
  std::vector<uint32_t>         m_var_id;     // Variable configuration: (name, id) pairs
  
  std::vector<std::string>      m_pair_key;   // Key   for string pairs
  std::vector<std::string>      m_pair_val;   // Value for string pairs
};

// Include template code: specialization of member templates
#include "TrigMonitoringEvent/TrigMonConfig.icc"

CLASS_DEF( TrigMonConfig , 253562967 , 1 )

#endif
