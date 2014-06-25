/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonSeq.h"
#include "TrigMonitoringEvent/TrigConfChain.h"

using namespace std;

//--------------------------------------------------------------------------------------      
uint16_t Trig::getEncodedId(int level,
			    int counter)
{
  // 16 bit word for encoded chain level and counter
  uint16_t word = 0x0;
  
  if(level < 1 || level > 3) {
    cerr << "Trig::getEncoded error! Bad level: " << level << endl;
    return word;
  }
  if(counter < 0 || counter >= 16384) {
    cerr << "Trig::getEncoded error! Bad counter: " << counter << endl;
    return word;
  }

  word |= level << Bits::shiftLevel;
  word |= counter;
  
  return word;
}

//--------------------------------------------------------------------------------------      
uint16_t Trig::getEncodedId(const std::string &level, int counter)
{
  if(level == "L1")  return Trig::getEncodedId(1, counter);
  if(level == "L2")  return Trig::getEncodedId(2, counter);
  if(level == "HLT") return Trig::getEncodedId(2, counter);
  if(level == "EF")  return Trig::getEncodedId(3, counter);

  return 0;
}

//--------------------------------------------------------------------------------------      
TrigConfChain::TrigConfChain()
  :m_chain_name(""),
   m_lower_name(""),
   m_chain_id(0),
   m_lower_id(0),
   m_chain_counter(0),
   m_lower_counter(0),
   m_level(0),
   m_prescale(0.0),
   m_pass_through(0.0)
{
}

//--------------------------------------------------------------------------------------      
TrigConfChain::TrigConfChain(const string &chain_name,
			     int           chain_counter,
			     unsigned int  chain_id,
			     const string &level,
			     const string &lower_chain_name,
			     int           lower_chain_counter,
			     unsigned int  lower_chain_id,
			     float         prescale,
			     float         pass_through)
  :m_chain_name(chain_name),
   m_lower_name(lower_chain_name),
   m_chain_id(chain_id),
   m_lower_id(lower_chain_id),
   m_chain_counter(0),
   m_lower_counter(0),
   m_level(0),
   m_prescale(prescale),
   m_pass_through(pass_through)
{
  // Set counters
  if(0 <= chain_counter && chain_counter < 16384) {
    m_chain_counter = static_cast<unsigned int>(chain_counter);
  }
  else {
    cerr << "TrigConfChain ctor error! Bad chain counter: " << chain_counter<< endl;
  }

  if(0 <= lower_chain_counter && lower_chain_counter < 16384) {
    m_lower_counter = static_cast<unsigned int>(lower_chain_counter);
  }

  // Set trigger level as integer
  if     (level == "L1")  m_level = 1;
  else if(level == "L2")  m_level = 2;
  else if(level == "HLT") m_level = 2;
  else if(level == "EF")  m_level = 3;
  else {
    cerr << "TrigConfChain ctor error! " << chain_name << ": bad level " << level << endl;
  }
}

//--------------------------------------------------------------------------------------      
TrigConfChain::TrigConfChain(const string &chain_name,
			     int           chain_counter,
			     unsigned int  chain_id,
			     float         prescale)
  :m_chain_name(chain_name),
   m_lower_name(""),
   m_chain_id(chain_id),
   m_lower_id(0),
   m_chain_counter(0),
   m_lower_counter(0),
   m_level(1),
   m_prescale(prescale),
   m_pass_through(0.0)
{
  // Set counters
  if(0 <= chain_counter && chain_counter < 16384) {
    m_chain_counter = static_cast<unsigned int>(chain_counter);
  }
  else {
    cerr << "TrigConfChain ctor error! Bad chain counter: " << chain_counter<< endl;
  }
}

//--------------------------------------------------------------------------------------      
void TrigConfChain::addStream(const std::string &name, float prescale)
{
  //
  // Save stream name and prescale
  //
  m_stream_name.push_back(name);
  m_stream_prescale.push_back(prescale);
}

//--------------------------------------------------------------------------------------      
void TrigConfChain::clearStrings()
{
  //
  // Clear strings in this class and all vector classes
  //
  m_chain_name.clear();
  m_lower_name.clear();
  m_stream_name.clear();
  m_stream_prescale.clear();  // prescale vector is meaningless without stream names
  m_group.clear();
  m_ebhypo_names.clear();

  for(unsigned int i = 0; i < m_signature.size(); ++i) m_signature[i].clearStrings();
}

//--------------------------------------------------------------------------------------      
uint16_t TrigConfChain::getEncodedId() const
{
  //
  // Return level id
  //
  if(m_level < 1 || m_level > 3) {
    return 0;
  }

  return Trig::getEncodedId(getLevelId(), getCounter());
}

//--------------------------------------------------------------------------------------      
uint16_t TrigConfChain::getLowerEncodedId() const
{
  //
  // Get lower chain encoded id
  //
  if(m_level == 2 || m_level == 3) {
    return Trig::getEncodedId(m_level-1, getLowerCounter());
  }
  
  return 0;
}

//--------------------------------------------------------------------------------------      
const std::string TrigConfChain::getLevel() const
{
  //
  // Get trigger level as integer
  //
  if     (m_level == 1) return "L1";
  else if(m_level == 2) {
    if(m_chain_name.substr(0, 2) == "L2") return "L2";
    else if(m_chain_name.substr(0, 3) == "HLT") return "HLT";
  }
  else if(m_level == 3) return "EF";

  return "L0";
}

//--------------------------------------------------------------------------------------      
float TrigConfChain::getSignaturePrescale(const std::string &name) const
{
  //
  // Find stream prescale
  //
  if(m_stream_prescale.size() != m_stream_name.size()) {
    cerr << "TrigConfChain::getSignaturePrescale - logic error!" << endl;
    return 0.0;
  }

  for(unsigned int i = 0; m_stream_name.size(); ++i) {
    if(m_stream_name[i] == name) return m_stream_prescale[i];
  }

  return 0.0;
}

//--------------------------------------------------------------------------------------      
bool TrigConfChain::matchOutputTE(const uint32_t te_id) const
{
  //
  // Match sequence to chain using signatures
  //
  for(std::vector<TrigConfSig>::const_iterator it = m_signature.begin(); 
      it != m_signature.end(); ++it) {
    if(it -> matchOutputTE(te_id)) {
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------      
void TrigConfChain::print(std::ostream &os) const
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------      
std::string str(const TrigConfChain &o)
{
  std::stringstream s;
  
  s << "TrigConfChain: "
    << o.getLevel() << " " << o.getName()
    << " PS=" << o.getPS() << " PT=" << o.getPT() 
    << " lower chain=" << o.getLowerName() << endl;

  s << "   signatures: ";
  for(unsigned int i = 0; i < o.getSignature().size(); ++i) {
    s << str(o.getSignature()[i]) << " ";
  }
  s << endl;
  
  s << "   streams: ";
  for(unsigned int i = 0; i < o.getStream().size(); ++i) {
    s << o.getStream()[i] << " ";
  }
  s << endl;

  s << "   groups: ";
  for(unsigned int i = 0; i < o.getGroup().size(); ++i) {
    s <<  o.getGroup()[i] << " ";
  }
  s << endl;
  
  return s.str();
}
