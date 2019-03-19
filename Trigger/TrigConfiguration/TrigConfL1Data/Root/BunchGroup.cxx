/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/BunchGroup.h"
#include <iostream>

using namespace std;

TrigConf::BunchGroup::BunchGroup() : 
   L1DataBaseclass(),
   m_InternalNumber( 99 ),
   m_Partition( 0 ),
   m_Bunches()
{}
  
void TrigConf::BunchGroup::addBunch(int b) {
   m_Bunches.push_back(b);
}

void
TrigConf::BunchGroup::clear() {
   m_InternalNumber = 99;
   m_Bunches.clear();
}

void
TrigConf::BunchGroup::print(const std::string& indent, unsigned int detail) const {
   if(detail>=2) {
      cout << indent << "BunchGroup "; 
      printNameIdV(indent);
      if(detail>=3) {
         cout << indent << "\t partition # :\t\t\t" << m_Partition << endl; 
         cout << indent << "\t internal # :\t\t\t" << m_InternalNumber << endl; 
         cout << indent << "\t Bunches: " << m_Bunches.size() << "\t\t";
         cout << indent;
         for(unsigned int i=0; i < m_Bunches.size();i++) {
            cout << m_Bunches[i] << " ";
         }
         cout << endl;
      }
   }
}

void TrigConf::BunchGroup::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile,indentLevel, indentWidth) 
      << "<BunchGroup internalNumber=\"" << m_InternalNumber << "\" name=\"" << name() << "\"";
   if(m_Bunches.size()==0) {
      xmlfile << "/>" << endl;
   } else {
      xmlfile << ">" << endl;
      for(unsigned int i=0; i < m_Bunches.size();i++)
      indent(xmlfile,indentLevel+1, indentWidth) 
         << "<Bunch bunchNumber=\"" << m_Bunches[i] << "\"/>" << endl;
      indent(xmlfile,indentLevel, indentWidth) 
         << "</BunchGroup>" << endl;
   }
}


