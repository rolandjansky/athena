/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_H
#define TRIGMON_EVENT_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of trigger data for single event.
    Class holds monitoring data in vectors by value.
    Some of functions are implemented via partial specialization
    of member function templates to save on repeating function
    declarations.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonAlg.h"
#include "TrigMonitoringEvent/TrigMonChain.h"
#include "TrigMonitoringEvent/TrigMonL1Item.h"
#include "TrigMonitoringEvent/TrigMonROB.h"
#include "TrigMonitoringEvent/TrigMonRoi.h"
#include "TrigMonitoringEvent/TrigMonSeq.h"
#include "TrigMonitoringEvent/TrigMonTE.h"
#include "TrigMonitoringEvent/TrigMonVar.h"

class TrigMonEvent
{
 public:
    
  TrigMonEvent();
  ~TrigMonEvent() {}
  
  static std::string getType() { return "TrigMonEvent"; }

  template<class T> typename std::vector<T>::const_iterator begin() const;
  template<class T> typename std::vector<T>::const_iterator end()   const;
  
  template<class T> unsigned int size() const;
  
  template<class T> const T& at(unsigned int i) const;

  template<class T> std::vector<T>& getVec();
  template<class T> const std::vector<T>& getVec() const;

  template<class T> bool add(const T &obj);
    
  void clear();
  
  void setEventID(uint32_t event, uint32_t lumi,
		  uint32_t bxid,  uint32_t run, 
		  uint32_t sec,   uint32_t nsec);

  void setTimer(float timer);
  void addWord(uint32_t word) { m_word.push_back(word); }

  std::vector<uint16_t>&  getL1Item() { return m_l1; }
  std::vector<uint32_t>&  getChain()  { return m_hlt; }
  std::vector<uint32_t>&  getWord()   { return m_word; }
  std::vector<uint32_t>&  getVarKey() { return m_var_key; }
  std::vector<float>&     getVarVal() { return m_var_val; }

  uint32_t getEvent()    const { return m_event; }
  uint32_t getLumi()     const;
  uint32_t getBunchId()  const;
  uint32_t getRun()      const { return m_run;   }  
  uint32_t getSec()      const { return m_sec;   }
  uint32_t getNanoSec()  const { return m_nsec;  }
  float    getTimer()    const;

  void addVar(uint32_t  key, float var);
  bool getVar(uint32_t  key, float &var) const;
  bool getKey(uint32_t &key, float  var) const;

  const std::vector<TrigMonL1Item> getL1Items() const;
  const std::vector<TrigMonChain>  getChains()  const;
  const std::vector<TrigMonVar>    getVar()     const;

  const std::vector<uint16_t>&  getL1Item() const { return m_l1; }
  const std::vector<uint32_t>&  getChain()  const { return m_hlt; }
  const std::vector<uint32_t>&  getWord()   const { return m_word; }

  const std::vector<uint32_t>&  getVarKey() const { return m_var_key; }
  const std::vector<float>&     getVarVal() const { return m_var_val; }
  
 private:
  
  // These variables are copied from EventID
  uint32_t                     m_event;     // Event number
  uint32_t                     m_lumi;      // Lumi block (low 16 bits) and bunch crossing id (high 16 bits)
  uint32_t                     m_bxid;      // Actually stores total event processing time
  uint32_t                     m_run;       // Run number
  uint32_t                     m_sec;       // Event second
  uint32_t                     m_nsec;      // Event ns
  std::vector<uint32_t>        m_word;      // Additional payload

  // Monitoring data - not all vectors are filled
  std::vector<TrigMonROB>      m_rob;       // ROB data requests
  std::vector<TrigMonRoi>      m_roi;       // RoI descriptors
  std::vector<TrigMonSeq>      m_seq;       // Sequence and algorithm execution timers
  std::vector<TrigMonTE>       m_te;        // Trigger elements summary

  // L1 and HLT decisions
  std::vector<uint16_t>        m_l1;        // Encoded TrigMonL1Item
  std::vector<uint32_t>        m_hlt;       // Encoded TrigMonChain

  // Variables stored as int, float pairs
  std::vector<uint32_t>        m_var_key;   // Variable key
  std::vector<float>           m_var_val;   // Variable value
};

unsigned getTrigMonEventVersion();
void setTrigMonEventVersion(unsigned version);

// Include template code: specialization of member templates
#include "TrigMonitoringEvent/TrigMonEvent.icc"

CLASS_DEF( TrigMonEvent , 243892660 , 1 )

#endif
