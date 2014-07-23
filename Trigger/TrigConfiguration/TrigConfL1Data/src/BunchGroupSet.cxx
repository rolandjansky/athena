/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
BunchGroupSet::~BunchGroupSet() {}


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
   uint16_t bit = 0x1 << bitpos;
   const std::vector<int>& b = bg.bunches();
   for(size_t i=0; i<b.size(); ++i)
      m_BGpattern[b[i]] |= bit;
}


void
BunchGroupSet::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "BunchGroupSet "; printNameIdV(); 
      if(detail>=2) {
         cout << indent << "        number of bunch groups: " << m_BunchGroups.size() << endl;
         for(BunchGroup bg: m_BunchGroups)
            bg.print(indent + "  ", detail);
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

