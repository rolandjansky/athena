/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sstream>

// Local
#include "TrigMonitoringEvent/TrigConfVar.h"
#include "TrigMonMSG.h"

using namespace std;

namespace TrigVar {
  static std::vector<TrigConfVar> gVarVec;
  static unsigned int gCounter = 60000;
}

namespace MSGService
{
  static TrigMonMSG msg("TrigConfVar");
}

//--------------------------------------------------------------------------------------      
TrigConfVar::TrigConfVar()
  :m_id(0),
   m_name()
{
}

//--------------------------------------------------------------------------------------      
TrigConfVar::TrigConfVar(const std::string &name, uint32_t id)
  :m_id(id),
   m_name(name)
{
}

//--------------------------------------------------------------------------------------      
void TrigConfVar::print(std::ostream &os) const
{
  os << str(*this) << endl;
}

//--------------------------------------------------------------------------------------  
std::string str(const TrigConfVar &o)
{
  stringstream s;
  s << "TrigConfVar: " << o.name() << "=" << o.id();
  
  return s.str();  
}

//--------------------------------------------------------------------------------------      
uint16_t Trig::ReserveVarId(const std::string &name)
{
  //
  // Return matching id, if it does not exist then create new id
  //
  for(unsigned int i = 0; i < TrigVar::gVarVec.size(); i++) {
    //
    // Check name already exists
    //
    if(TrigVar::gVarVec[i].getName() == name) { 
      return TrigVar::gVarVec[i].getId();
    }
  }
  
  if(TrigVar::gCounter+1 >= 65535) {
    MSGService::msg.Log("ReserveVarId - error! Overflow of 16 bits key.", MSG::ERROR);
    return 0;
  }
  
  //
  // Add new configuration
  //
  TrigVar::gVarVec.push_back(TrigConfVar(name, TrigVar::gCounter++));

  return TrigVar::gVarVec.back().getId();
}

//--------------------------------------------------------------------------------------      
uint16_t Trig::ReserveVarId(const std::string &name, uint16_t id)
{
  //
  // Create new id using input id as suggestion.
  //
  bool matched_id = false;

  for(unsigned int i = 0; i < TrigVar::gVarVec.size(); i++) {
    
    if(TrigVar::gVarVec[i].getName() == name) { 
      //
      // Check if already stored id matches
      //
      if(TrigVar::gVarVec[i].getId() != id) {
        std::stringstream ss;
        ss << "ReserveVarId - error! Existing var with " << name << " and id=" << TrigVar::gVarVec[i].getId() << ": new id=" << id;
        MSGService::msg.Log(ss.str(), MSG::ERROR);
      }

      return TrigVar::gVarVec[i].getId();
    }
    else if(TrigVar::gVarVec[i].getId() == id) { 
      //
      // Check if already stored name matches
      //
      matched_id = true;

      if(TrigVar::gVarVec[i].getName() != name) {
        std::stringstream ss;
        ss << "ReserveVarId - error! Existing var with " << name << " and id=" << TrigVar::gVarVec[i].getId() << ": new name=" << name;
        MSGService::msg.Log(ss.str(), MSG::ERROR);
      }
      else {
        return id;
      }
    }
  }

  //
  // Add new configuration
  //
  if(matched_id) {
    if(TrigVar::gCounter+1 >= 65535) {
      MSGService::msg.Log("ReserveVarId - error! Overflow of 16 bits key.", MSG::ERROR);
      return 0;
    }
    
    TrigVar::gVarVec.push_back(TrigConfVar(name, TrigVar::gCounter++));
  }
  else {
    TrigVar::gVarVec.push_back(TrigConfVar(name, id));    
  }

  return TrigVar::gVarVec.back().getId();
}

//--------------------------------------------------------------------------------------      
bool Trig::FindVarId(const std::string &name, uint16_t &id)
{
  //
  // Find id for variable name
  //
  for(unsigned int i = 0; i < TrigVar::gVarVec.size(); i++) {
    
    if(TrigVar::gVarVec[i].getName() == name) { 
      id = TrigVar::gVarVec[i].getId();
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------      
bool Trig::FindVarName(const uint16_t id, std::string &name)
{
  //
  // Find id for variable name
  //
  for(unsigned int i = 0; TrigVar::gVarVec.size(); i++) {
    
    if(TrigVar::gVarVec[i].getId() == id) { 
      name = TrigVar::gVarVec[i].getName();
      return true;
    }
  }

  return false;
}

//--------------------------------------------------------------------------------------      
std::vector<TrigConfVar> Trig::GetCurrentTrigConfVarVector()
{
  //
  // Return copy of static vector
  //
  return TrigVar::gVarVec;
}
