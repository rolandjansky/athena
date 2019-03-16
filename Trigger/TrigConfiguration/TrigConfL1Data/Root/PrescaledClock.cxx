/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/PrescaledClock.h"
#include <iostream>

using namespace std;

TrigConf::PrescaledClock::PrescaledClock() :
   L1DataBaseclass(),
   m_clock1(0),
   m_clock2(0)
{}

void TrigConf::PrescaledClock::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "PrescaledClock: "; printNameIdV();
      if(detail>=2) {
         cout << indent << "        clock1:  " << m_clock1 << endl;
         cout << indent << "        clock2:  " << m_clock2 << endl;  
      }
   }
}  

void TrigConf::PrescaledClock::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile,indentLevel, indentWidth) 
      << "<PrescaledClock clock1=\""<<m_clock1<<"\" clock2=\""<<m_clock2<<"\" name=\""<< name() <<"\"/>"
      << endl;
}
