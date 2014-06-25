/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_CHAIN_H
#define TRIGCONF_CHAIN_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief This class provides map between monitoring object TrigMonChain 
   and chain configuration. This class is copied after HLTChain class 
   with some variables removed. It was added so we would be able to 
   write chain configuration into ROOT files and ship it along with 
   monitoring data.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigConfSig.h"

class TrigConfChain
{
 public:
  
  TrigConfChain();
  
  // Constructor for L2 and EF chains
  TrigConfChain(const std::string &chain_name,
		int                chain_counter,
		unsigned int       chain_id,
		const std::string &level,
		const std::string &lower_chain_name,
		int                lower_chain_counter,
		unsigned int       lower_chain_id,
		float              prescale,
		float              pass_through);
  
  // Constructor for L1 chains
  TrigConfChain(const std::string &chain_name,
		int               chain_counter,
		unsigned int      chain_id,
		float             prescale);

  ~TrigConfChain() {} 

  void clearStrings();

  void addSignature(const TrigConfSig  &obj)  { m_signature.push_back(obj); }
  void addGroup    (const std::string &name) { m_group.push_back(name); }  
  void addStream   (const std::string &name, float prescale);
  void addEBHypo   (const std::string &name) { m_ebhypo_names.push_back(name); }

  void clearGroup  () { m_group.clear(); }
  void clearStream () { m_stream_name.clear(); m_stream_prescale.clear(); }
  void clearEBHypo () { m_ebhypo_names.clear(); }

  void addLowerChainId(uint32_t lower_id)       { m_lower_ids.push_back(lower_id); }

  void setPrescale   (float val) { m_prescale     = val; }
  void setPassThrough(float val) { m_pass_through = val; }

  const std::string& getName()      const { return m_chain_name; }
  const std::string& getChainName() const { return getName(); }
  const std::string& getLowerName() const { return m_lower_name; }

  uint32_t getId()             const { return m_chain_id; }
  uint16_t getCounter()        const { return m_chain_counter; }
  uint16_t getEncodedId()      const;

  uint16_t getLowerCounter()   const { return m_lower_counter; }
  uint32_t getLowerId()        const { return m_lower_id; }
  uint16_t getLowerEncodedId() const;

  unsigned int      getLevelId() const { return m_level; }
  const std::string getLevel()   const;    

  float getPrescale()    const { return m_prescale; }
  float getPS()          const { return getPrescale(); }
  float getPassThrough() const { return m_pass_through; }
  float getPT()          const { return getPassThrough(); }
  
  const std::vector<TrigConfSig>& getSignature() const { return m_signature; }
  const std::vector<std::string>& getStream()    const { return m_stream_name; }
  const std::vector<std::string>& getGroup()     const { return m_group; }
  const std::vector<std::string>& getEBHypo()    const { return m_ebhypo_names; }

  // Allow access to stream and group
  std::vector<float>&       getStreamPS()              { return m_stream_prescale; }
  std::vector<std::string>& getStream()                { return m_stream_name;     }
  std::vector<std::string>& getGroup()                 { return m_group;           }
  std::vector<std::string>& getEBHypo()                { return m_ebhypo_names;    }

  const std::vector<uint32_t>& getLowerIds() const { return m_lower_ids; }

  float getSignaturePrescale(const std::string &name) const;

  bool matchOutputTE(uint32_t te_id) const;
  
  void print(std::ostream &os = std::cout) const;
  
 private:
  
  std::string                   m_chain_name;          // Chain name
  std::string                   m_lower_name;          // Lower chain name
  uint32_t                      m_chain_id;            // Hash value from chain_name
  uint32_t                      m_lower_id;            // Lower chain hash value from chain_name
  uint16_t                      m_chain_counter;       // Chain counter
  uint16_t                      m_lower_counter;       // Lower chain counter
  uint8_t                       m_level;               // Trigger level
  float                         m_prescale;            // Prescale value
  float                         m_pass_through;        // Pass_through flag

  std::vector<uint32_t>         m_lower_ids;           // Lower chain ids (exceptional case!)
  std::vector<float>            m_stream_prescale;     // Stream prescale list
  std::vector<TrigConfSig>      m_signature;           // Signature list
  std::vector<std::string>      m_stream_name;         // Stream name list
  std::vector<std::string>      m_group;               // Group name list
  std::vector<std::string>      m_ebhypo_names;        // List of L1 items from EB Hypo
};

std::string str(const TrigConfChain &);

//
// getEncoded() returns 16 bit word for chain level and counter:
// llcccccccccccccc
// 0123456789012345
//
// chain level   [l] 2  bits (0:3)      = 1 (L1), 2 (L2), 3 (EF)
// chain counter [c] 14 bits (0:16,384) 
//

//
//  Helper functions
//
namespace Trig { 
  namespace Bits {
    const uint16_t maskLevel   = 0xc000;
    const uint16_t maskCounter = 0x3fff;
    
    const uint16_t shiftLevel   = 14;
    const uint16_t shiftCounter = 0;
  }

  uint16_t getEncodedId(int level, int counter);
  uint16_t getEncodedId(const std::string &level, int counter);
  
  inline uint16_t getCounterFromEncodedId(uint16_t encoded) {
    return (encoded & Bits::maskCounter) >> Bits::shiftCounter;
  }
  inline uint16_t getLevelFromEncodedId(uint16_t encoded) {
    return (encoded & Bits::maskLevel) >> Bits::shiftLevel;
  }
}

//
// Inlined global sort functions
//
inline bool operator==(const TrigConfChain &lhs, const TrigConfChain &rhs) {
  return lhs.getName() == rhs.getName();
}
inline bool operator <(const TrigConfChain &lhs, const TrigConfChain &rhs) {
  return lhs.getName() < rhs.getName();
}

//
// Compare TrigConfChain and other types
//
inline bool operator==(const TrigConfChain &chn, const std::string &name) { 
  return chn.getName() == name; 
}
inline bool operator==(const std::string &name, const TrigConfChain &chn) { 
  return chn.getName() == name; 
}

inline bool operator <(const TrigConfChain &chn, const std::string &name) { 
  return chn.getName() < name; 
}
inline bool operator <(const std::string &name, const TrigConfChain &chn) { 
  return name < chn.getName(); 
}

inline bool operator==(const TrigConfChain &chn, unsigned int id) { 
  return chn.getId() == id;
}
inline bool operator==(unsigned int id, const TrigConfChain &chn) { 
  return id == chn.getId();
}

inline bool operator <(const TrigConfChain &chn, unsigned int id) { 
  return chn.getId() < id; 
}
inline bool operator <(unsigned int id, const TrigConfChain &chn) { 
  return id < chn.getId(); 
}

CLASS_DEF( TrigConfChain , 158937726 , 1 )

#endif
