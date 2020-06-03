/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  Parameter.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/18/12.

#include "L1TopoInterfaces/Parameter.h"

using namespace std;
using namespace TCS;

Parameter::Parameter( const string & name,
                      TCS::parType_t value) :
   m_name(name),
   m_value(value),
   m_defaultValue(value),
   m_isExtended(false)
{}


Parameter::Parameter( const string & name,
                      TCS::parType_t value,
                      TCS::parType_t selection) :
   m_name(name),
   m_value(value),
   m_selection(selection),
   m_defaultValue(value),
   m_isExtended(true)
{}


namespace TCS {


std::ostream &
operator<<(std::ostream &o, const TCS::Parameter & p) {
  
  if(!p.isExtended()) {
    o << p.name() << ": " << p.value();
  } 
  
  if(p.isExtended()) {
    o << p.name() << ": " << p.value() << ": " << p.position() << ": " << p.selection();
  }
    
  if(!p.isDefault()) {
    o << " (default value: " << p.defaultValue() << ")";
  }
  
  return o;
}


}
