/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTUtils.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TrigConf;

map<unsigned int,string> HLTTriggerElement::m_IdToLabel;

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
   CheckAndStoreHash();
}

HLTTriggerElement::HLTTriggerElement(  unsigned int, const string& name ) :
   TrigConfData(name),
   m_hashId(HLTUtils::string2hash(name) ), // don't allow for external setting of trigger element id's
   m_level(0)
{
   CheckAndStoreHash();
}

HLTTriggerElement::HLTTriggerElement( const std::pair<unsigned int,string>& p ) :
   TrigConfData(p.second),
   m_hashId( HLTUtils::string2hash(p.second) ), // don't yet allow for external setting of trigger element id's
   m_level(0)
{
   CheckAndStoreHash();
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


void
HLTTriggerElement::CheckAndStoreHash() const {
   map<unsigned int, string>::const_iterator it = m_IdToLabel.find( hashId() );
   if (it == m_IdToLabel.end()) {
      m_IdToLabel.insert( pair<unsigned int, string>(hashId(),name()) );
      return;
   }


   if( it->second==name() ) // TE was build before (identical (hash,name) pair was found)
      return;

   // will abort only when hash conflict occured
   // hash ID already exists in the table
   // either the hash is not unique, or the same TE is inserted twice
   // in either case we should abort
   throw runtime_error("TrigConf::HLTTriggerElement: hash conflict between '" + it->second + "' and '" + name() + "', both result in " + boost::lexical_cast<string,unsigned int>(id()) );
}


bool
HLTTriggerElement::getLabel(unsigned int id, string & label) {
   map<unsigned int, string>::const_iterator it = m_IdToLabel.find(id);
   if(it != m_IdToLabel.end()) { label = it->second; return true; }
   label = "";
   return false;
}


bool
HLTTriggerElement::getId(const char * label, unsigned int & id) {
   map<unsigned int, string>::const_iterator it = m_IdToLabel.begin();
   for(it = m_IdToLabel.begin(); it != m_IdToLabel.end(); it++)
      if(it->second == label) { id = it->first; return true; }
   return false;
}
