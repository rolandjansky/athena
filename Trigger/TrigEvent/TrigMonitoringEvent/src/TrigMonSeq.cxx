/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <iomanip>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonSeq.h"
#include "TrigMonMSG.h"

namespace SeqBits
{
  const uint32_t maskLow16  = 0x0000ffff;  // mask low 16 bits
  const uint32_t maskIndex  = 0xfff00000;  // mask index bits  
  const uint32_t shiftIndex = 20;
}

namespace MSGService
{
  static TrigMonMSG msg("TrigMonSeq");
}

using namespace std;

// This is to work around an edm change
const static uint16_t LARGE_INDEX_SEQ_LOCATION = 321;

//--------------------------------------------------------------------------------------  
TrigMonSeq::TrigMonSeq()
  :m_encoded(0x0)
{
}

//--------------------------------------------------------------------------------------  
TrigMonSeq::TrigMonSeq(uint32_t encoded)
  :m_encoded(encoded)
{
}

//--------------------------------------------------------------------------------------  
TrigMonSeq::TrigMonSeq(const TrigConfChain &chn, const TrigConfSeq &seq)
  :m_encoded(chn.getEncodedId())
{
  //
  // Encoded index into 
  //
  const uint32_t index = seq.getIndex();
  if(index >= 4096) {
    // EDM limitation workaround with big run-2 menus
    m_var_key.push_back(LARGE_INDEX_SEQ_LOCATION);
    m_var_val.push_back((float) index); // This is not going to get so large that we'll loose info in a float
  }
  else {
    m_encoded |= (index << SeqBits::shiftIndex);
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonSeq::addVar(const TrigMonVar &var)
{
  //
  // Store variable as int and float, reserve 0-9 keys
  //
  if(var.getKey() > 9) {
    m_var_key.push_back(var.getKey());
    m_var_val.push_back(var.getData());
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonSeq::addTimer(float timer)
{
  //
  // Add sequence timer at key = 0
  //
  m_var_key.push_back(0); 
  m_var_val.push_back(timer); 
}

//--------------------------------------------------------------------------------------  
void TrigMonSeq::addState(State value)
{
  //
  // Make OR with State enum value (already at correct bit position)
  //
  m_encoded |= value;
}

//--------------------------------------------------------------------------------------  
bool TrigMonSeq::isInitial() const
{
  //
  // Check value of SEQ State enum
  //
  return (m_encoded & kInitial);
}

//--------------------------------------------------------------------------------------  
bool TrigMonSeq::isExecuted() const
{
  //
  // Check value of SEQ State enum
  //
  return (m_encoded & kStart);
}

//--------------------------------------------------------------------------------------  
bool TrigMonSeq::isAlreadyExecuted() const
{
  //
  // Check value of SEQ State enum
  //
  return (m_encoded & kAlreadyExecuted);
}

//--------------------------------------------------------------------------------------  
bool TrigMonSeq::isPrevious() const
{
  //
  // Check value of SEQ State enum
  //
  return (m_encoded & kPrevious);
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonSeq::getLevel() const
{
  return Trig::getLevelFromEncodedId(getChnEncodedId());
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonSeq::getChnCounter() const
{
  return Trig::getCounterFromEncodedId(getChnEncodedId());
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonSeq::getChnEncodedId() const
{
  //
  // Mask out low 16 bits and return encoded chain id
  //
  return (m_encoded & SeqBits::maskLow16);
}

//--------------------------------------------------------------------------------------  
uint16_t TrigMonSeq::getSeqIndex() const
{
  //
  // Mask out SEQ State enum bits and return index
  // 
  // Check it's not stored in the hack way first
  for (uint32_t i=0; i < m_var_key.size(); ++i) {
    if (m_var_key.at(i) == LARGE_INDEX_SEQ_LOCATION) return (uint16_t) m_var_val.at(i);
  }
  return (m_encoded & SeqBits::maskIndex) >> SeqBits::shiftIndex;
}

//--------------------------------------------------------------------------------------  
double TrigMonSeq::getAlgTimer() const
{
  //
  // Explicit loop over timers to get total time measures by algorithms
  //
  if(m_alg.empty()) return 0.0;

  double timer_sum = 0.0;
  for(unsigned int i = 0; i < m_alg.size(); ++i) timer_sum += m_alg[i].getTimer();
  
  return timer_sum;
}

//--------------------------------------------------------------------------------------  
float TrigMonSeq::getSeqTimer() const
{
  //
  // Get sequence timer which is stored at key=0
  //

  if(m_var_key.size() == m_var_val.size()) {
    //
    // Look for key == 0
    //
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] == 0) return m_var_val[i];
    }
  }

  return 0.0;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonVar> TrigMonSeq::getVar() const
{
  //
  // Build variables on a fly and return vector by value
  //
  std::vector<TrigMonVar> var;

  if(m_var_key.size() == m_var_val.size()) {
    //
    // Iterate over keys abd values
    //
    var.reserve(m_var_key.size());

    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] != 0) {
	var.push_back(TrigMonVar(m_var_key[i], m_var_val[i]));
      }
    }
  }

  return var;
}

//--------------------------------------------------------------------------------------  
void TrigMonSeq::print(const TrigConfSeq &confg, std::ostream &os) const
{
  //
  // Print payload
  //
  if(confg.getIndex() != getSeqIndex()) {
    os << "TrigMonSeq::Print - error configuration does not match this sequence\n";
    return;
  }

  std::stringstream st;

  st << "TrigMonSeq: " << confg.getName() 
     << " isExecuted=" << int(isExecuted())
     << " isAlreadyExecuted=" << int(isAlreadyExecuted())
     << " isInitial=" << int(isInitial())
     << " isPrevious=" << int(isPrevious()) << "\n"
     << "           t_seq-t_alg=" << getSeqTimer() << "-" << getAlgTimer() << "=" 
     << getSeqTimer() - getAlgTimer() 
     << " SEQ contains " << m_alg.size() << " algorithm(s)"
     << "\n";

  unsigned int iwidth = 0;
  for(unsigned int j = 0; j < m_alg.size(); ++j) {
    const TrigMonAlg &alg_entry = m_alg[j];
    const TrigConfAlg &alg_confg = confg.getAlg(alg_entry.getPosition());

    iwidth = std::max<unsigned int>(iwidth, alg_confg.getName().size());
  }

  for(unsigned int j = 0; j < m_alg.size(); ++j) {
    const TrigMonAlg &alg_entry = m_alg[j];
    const TrigConfAlg &alg_confg = confg.getAlg(alg_entry.getPosition());
    
    st << "  " << setw(2) << setfill(' ') << std::right << j << ": " 
       << setw(iwidth) << std::left << alg_confg.getName()
       << " isCached=" << int(alg_entry.isCached()) 
       << " timer=" << alg_entry.elapsed()
       << " RoiId=";
      
    for(unsigned int r = 0; r < alg_entry.getNRoi(); ++r) {
      st << static_cast<unsigned int>(alg_entry.getRoiId(r)) << " ";
    }
      
    if(alg_entry.isCalled()) {
      st << " t1-t0=" 
	 << alg_entry.start().getSec() << "." << alg_entry.start().getMicroSec()
	 << "-" 
	 << alg_entry.stop().getSec() << "." << alg_entry.stop().getMicroSec()
	 << "="
	 << alg_entry.elapsed();
    }
    
    st << "\n";
  }

  os << st.str();
}
