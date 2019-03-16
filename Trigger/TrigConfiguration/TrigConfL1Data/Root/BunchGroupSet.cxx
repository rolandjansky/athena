/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/BunchGroupSet.h"

#include <iostream>

using namespace std;
using namespace TrigConf;


BunchGroupSet::BunchGroupSet() : L1DataBaseclass(), m_BGpattern(3564) {}
  
BunchGroupSet::BunchGroupSet(const std::vector<BunchGroup>& bgv) :
   L1DataBaseclass(), 
   m_BunchGroups(bgv),
   m_BGpattern(3564)
{
   for(BunchGroup bg: m_BunchGroups)
      fillPattern(bg.internalNumber(), bg);
}
  
void
BunchGroupSet::addBunchGroup(const BunchGroup& bg) {
   fillPattern(bg.internalNumber(), bg);
   m_BunchGroups.push_back(bg);
}


void
BunchGroupSet::setBGName(uint32_t i, const std::string& name) {
   if(i<m_BunchGroups.size())
      m_BunchGroups[i].setName(name);        
}

void
BunchGroupSet::fillPattern(uint32_t bitpos, const BunchGroup& bg) {
   if(bitpos>=16) 
      return;
   uint16_t bit = 0x1 << bitpos;
   const std::vector<int>& b = bg.bunches();
   for(size_t i=0; i<b.size(); ++i)
      m_BGpattern[b[i]] |= bit;
}


void
BunchGroupSet::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "BunchGroupSet " << name();
      if(id()>0 || version()>0)
         cout << " (id=" << id() << "/v=" << version() << ")";
      cout  << endl;
      cout << indent << "Number of bunch groups: " << m_BunchGroups.size() << endl;
      if(detail>=2) {
         for(BunchGroup bg: m_BunchGroups)
            bg.print(indent + indent, detail);
      }
   }
}

void
BunchGroupSet::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile,indentLevel, indentWidth) 
      << "<BunchGroupSet name=\"" << name() << "\" menuPartition=\"" << m_MenuPartition << "\">" << endl;
   for(BunchGroup bg: m_BunchGroups)
      bg.writeXML(xmlfile, indentLevel+1, indentWidth);
   indent(xmlfile,indentLevel, indentWidth)
      << "</BunchGroupSet>" << endl;   
}

