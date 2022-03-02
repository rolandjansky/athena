/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTUtils/HLTUtils.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TrigConf;

HLTTriggerElement::HLTTriggerElement() :
   TrigConfData(""),
   m_hashId(0),
   m_level(0)
{}

HLTTriggerElement::HLTTriggerElement(  const string& name ) :
   TrigConfData(name),
   m_hashId(HLTUtils::string2hash(name)), // don't allow for external setting of trigger element id's
   m_level(0)
{
}

HLTTriggerElement::HLTTriggerElement(  unsigned int, const string& name ) :
   TrigConfData(name),
   m_hashId(HLTUtils::string2hash(name) ), // don't allow for external setting of trigger element id's
   m_level(0)
{
}

HLTTriggerElement::HLTTriggerElement( const std::pair<unsigned int,string>& p ) :
   TrigConfData(p.second),
   m_hashId( HLTUtils::string2hash(p.second) ), // don't yet allow for external setting of trigger element id's
   m_level(0)
{
}

HLTTriggerElement::~HLTTriggerElement() {}


void
HLTTriggerElement::writeXML(std::ofstream & xmlfile) {
   xmlfile << "                    <TRIGGERELEMENT te_name=\"" 
           << name() << "\"/>" << endl;
   return;
}

void
HLTTriggerElement::print(const std::string& indent, unsigned int /*detail*/) const {
   std::cout << indent << *this;
}

std::ostream &
TrigConf::operator<<(std::ostream & o, const HLTTriggerElement & te) {
   o << "(\"" << te.name() << "\", " << te.m_hashId << ")";
   return o;
}


bool
HLTTriggerElement::getLabel(unsigned int id, string & label) {
   label = HLTUtils::hash2string(id);
   if (label=="UNKNOWN HASH ID") {
      label = "";
      return false;
   }
   return true;
}
