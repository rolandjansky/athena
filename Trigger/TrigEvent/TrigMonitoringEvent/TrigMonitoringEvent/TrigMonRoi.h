/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROI_H
#define TRIGMON_ROI_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Compact copy of TrigRoIDescriptor.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonVar.h"

class TrigMonRoi
{
 public:
  
  enum Type {
    kNone   = 0,       
    kMuon   = 1,
    kEmTau  = 2,
    kJet    = 3,
    kJetEt  = 4,
    kEnergy = 5
  };

  TrigMonRoi();
  explicit TrigMonRoi(unsigned int roi_word);
  ~TrigMonRoi() {}

  void setType(Type type);
  void addWord(unsigned int word) { m_word.push_back(word); }

  void setRoiId(unsigned int id);  
  void setNL1th(unsigned int lt);
  
  void setEtaPhi (float eta, float phi);
  void setRoIArea(float eta_width, float phi_width);

  void addVar(const TrigMonVar &var);

  uint32_t getRoiWord() const { return m_word[0]; }

  Type     getRoiType() const;  
  uint32_t getRoiId()   const;
  uint8_t  getNL1th()   const;

  float getEta()     const;
  float getPhi()     const; 
  float getRoIArea() const; 

  std::vector<uint32_t>&  getWord()   { return m_word; }
  std::vector<uint16_t>&  getVarKey() { return m_var_key; }
  std::vector<float>&     getVarVal() { return m_var_val; }

  const std::vector<uint32_t>&  getWord()   const { return m_word; }
  const std::vector<uint16_t>&  getVarKey() const { return m_var_key; }
  const std::vector<float>&     getVarVal() const { return m_var_val; }
  float                         getVarVal(  const uint32_t key ) const;
  const std::vector<TrigMonVar> getVar()    const;

  void print(std::ostream &os = std::cout);

 private:

  std::vector<uint32_t>   m_word;          // Encoded RoIWord and other data
  std::vector<uint16_t>   m_var_key;       // Variable key
  std::vector<float>      m_var_val;       // Variable value
};

std::string str(const TrigMonRoi &);

namespace Trig
{
  //
  // Roi id for special two cases when RoiId is not available
  //
  inline uint8_t getRoiId_Unknown() { return 255; }
  inline uint8_t getRoiId_JetEt()   { return 254; }
  inline uint8_t getRoiId_Energy()  { return 253; }
}

//
// m_word[0] = L1 hardware RoI word
//
// m_word[1] = encoded word, if set it stores: 
//      8 bits:  0- 8 [i] RoiId   assigned by HLT
//      4 bits:  9-12 [t] RoiType assigned by HLT
//      8 bits: 13-20 [l] number of L1 thresholds activated by HLT
//     12 bits: 21-32 [f] free
//
// m_encoded = [pppppppppppp][llllllll][tttt][rrrrrrrr]
// 
// In 2012 data, we add one more variable to record full RoI Id
// m_word[2] = RoI Id
//

//
// Inlined global comparison operators
//
inline bool operator==(const TrigMonRoi &lhs, const TrigMonRoi &rhs) { 
  return lhs.getRoiWord() == rhs.getRoiWord();
}
inline bool operator <(const TrigMonRoi &lhs, const TrigMonRoi &rhs) { 
  return lhs.getRoiWord() < rhs.getRoiWord();
}

inline bool operator==(const TrigMonRoi &roi, unsigned int word) { 
  return roi.getRoiWord() == word;
}
inline bool operator==(unsigned int word, const TrigMonRoi &roi) { 
  return word == roi.getRoiWord();
}

CLASS_DEF( TrigMonRoi , 125550248 , 1 )

#endif
