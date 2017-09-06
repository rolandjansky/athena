/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <iostream>
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigMonRoi.h"
#include "TrigMonMSG.h"

namespace RoiBits
{
  const uint32_t maskId   = 0x000000ff;
  const uint32_t maskType = 0x00000f00;
  const uint32_t maskL1   = 0x000ff000;
  //const uint32_t maskFree = 0xfff00000;
  
  const uint32_t shiftId   = 0;
  const uint32_t shiftType = 8;
  const uint32_t shiftL1   = 12;
  //const uint32_t shiftFree = 20;
}

namespace MSGService
{
  static TrigMonMSG msg("TrigMonRoi");
}

using namespace std;

//--------------------------------------------------------------------------------------      
TrigMonRoi::TrigMonRoi()
  :m_word(1, 0)
{
}

//--------------------------------------------------------------------------------------      
TrigMonRoi::TrigMonRoi(uint32_t roi_word)
  :m_word(1, 0)
{
  //
  // Set RoiId to uknown state
  //
  m_word[0] = roi_word;
}

//--------------------------------------------------------------------------------------      
void TrigMonRoi::setType(Type type)
{
  //
  // Set roi type
  //
  if(m_word.size() < 2) return;

  m_word[1] |= (type << RoiBits::shiftType);
}

//--------------------------------------------------------------------------------------      
void TrigMonRoi::setRoiId(unsigned int id)
{
  //
  // Set roi type
  //
  if(m_word.size() < 2) return;
  
  if(id < 256) {
    m_word[1] |= (id << RoiBits::shiftId);
  }
  else {
    if(m_word.size()<3){
      m_word.push_back(id);
    }
    else{
      m_word[2]=id;
    }
  }
}

//--------------------------------------------------------------------------------------      
void TrigMonRoi::setNL1th(unsigned int lt)
{
  //
  // Set roi type
  //
  if(m_word.size() < 2) return;

  if(lt < 256) {
    m_word[1] |= (lt << RoiBits::shiftL1);
  }
  else {
    MSGService::msg.Log("setL1thresholds error! Value is out of range: ",MSG::ERROR);
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonRoi::setEtaPhi(float eta, float phi)
{
  //
  // Add eta and phi coordinatesat key=0 and key=1
  //
  if(std::count(m_var_key.begin(), m_var_key.end(), 0) == 0 && 
     std::count(m_var_key.begin(), m_var_key.end(), 1) == 0) {
    m_var_key.push_back(0);
    m_var_val.push_back(eta);
    
    m_var_key.push_back(1);
    m_var_val.push_back(phi);
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonRoi::setRoIArea(float eta_width, float phi_width)
{
  //
  // Add eta and phi coordinatesat key=0 and key=1
  //
  if(std::count(m_var_key.begin(), m_var_key.end(), 2) == 0) {

    // Assuming the area is an ellipse - approximate
    //float area = 3.14159*eta_width*phi_width;

    // Chainging to square area, assume supplied are full width
    float area = eta_width * phi_width;

    m_var_key.push_back(2);
    m_var_val.push_back(area);
  }
}

//--------------------------------------------------------------------------------------  
void TrigMonRoi::addVar(const TrigMonVar &var)
{
  //
  // Store variable as int and float, reserve 0-9 keys
  //
  if(var.getKey() > 9) {
    m_var_key.push_back(var.getKey());
    m_var_val.push_back(var.getData());
  } else {
    MSGService::msg.Log("Cannot add a var with key < 9 (internal use only) ",MSG::ERROR);
  }
}

//--------------------------------------------------------------------------------------      
TrigMonRoi::Type TrigMonRoi::getRoiType() const
{
  //
  // Return RoI type assigned by HLT
  //
  if(m_word.size() < 2) return kNone;

  const unsigned int val = (m_word[1] & RoiBits::maskType) >> RoiBits::shiftType;

  switch (val) {
  case 1: return kMuon;
  case 2: return kEmTau;
  case 3: return kJet;
  case 4: return kJetEt;
  case 5: return kEnergy;
  default: break;
  }
  
  return kNone;
}

//--------------------------------------------------------------------------------------      
uint32_t TrigMonRoi::getRoiId() const
{
  //
  // Return RoI id assigned by HLT
  //
  if(m_word.size() < 2) return Trig::getRoiId_Unknown();
  
  // For 2012 data
  if(m_word.size() == 3) return m_word[2];

  return (m_word[1] & RoiBits::maskId) >> RoiBits::shiftId;
}

//--------------------------------------------------------------------------------------  
float TrigMonRoi::getEta() const
{
  //
  // Search for eta value at key=0
  //

  if(m_var_key.size() == m_var_val.size()) {
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] == 0) return m_var_val[i];
    }
  }

  return -9999.0;
}

//--------------------------------------------------------------------------------------  
float TrigMonRoi::getPhi() const
{
  //
  // Search for phi value at key=1
  //

  if(m_var_key.size() == m_var_val.size()) {
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] == 1) return m_var_val[i];
    }
  }

  return -9999.0;
}

//--------------------------------------------------------------------------------------  
float TrigMonRoi::getRoIArea() const
{
  //
  // Search for phi value at key=2
  //

  if(m_var_key.size() == m_var_val.size()) {
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if(m_var_key[i] == 2) return m_var_val[i];
    }
  }

  return -9999.0;
}

//--------------------------------------------------------------------------------------      
uint8_t TrigMonRoi::getNL1th() const
{
  //
  // Return number of L1 thresholds assigned by HLT
  //
  if(m_word.size() < 2) return 0;

  return (m_word[1] & RoiBits::maskL1) >> RoiBits::shiftL1;
}

//--------------------------------------------------------------------------------------  
const std::vector<TrigMonVar> TrigMonRoi::getVar() const
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
      var.push_back(TrigMonVar(m_var_key[i], m_var_val[i]));
    }
  }

  return var;
}

float TrigMonRoi::getVarVal( const uint32_t key ) const 
{
    for(unsigned int i = 0; i < m_var_key.size(); ++i) {
      if (m_var_key.at(i) == key) return m_var_val.at(i);
    }
    return 0.;
}


//--------------------------------------------------------------------------------------      
void TrigMonRoi::print(std::ostream &os)
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigMonRoi &o)
{
  std::stringstream s;  
  s << "TrigMonRoi: id=" << o.getRoiId() << " word=" << o.getRoiWord();

  return s.str();
}
