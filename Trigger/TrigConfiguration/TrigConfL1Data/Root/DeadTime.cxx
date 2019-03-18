/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/DeadTime.h"
#include <iostream>

using namespace std;

TrigConf::DeadTime::DeadTime() :
   L1DataBaseclass(),
   m_simple(0),
   m_complex1Level(0), 
   m_complex1Rate(0),
   m_complex2Level(0),
   m_complex2Rate(0)
{}

void
TrigConf::DeadTime::print(const std::string& indent, unsigned int detail) const {
   if(detail>=1) {
      cout << indent << "DeadTime "; printNameIdV("");
      if(detail>=2) {
         cout << indent << "        simple DT:         " << m_simple << endl;
         cout << indent << "        complex1 DT size: " << m_complex1Level << " rate: " << m_complex1Rate << endl;
         cout << indent << "        complex2 DT size: " << m_complex2Level << " rate: " << m_complex2Rate << endl;
      }
   }
}

void
TrigConf::DeadTime::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth) const {
   indent(xmlfile,indentLevel, indentWidth) 
      << "<Deadtime complex1_level=\"" << m_complex1Level
      << "\" complex1_rate=\"" << m_complex1Rate
      << "\" complex2_level=\"" << m_complex2Level
      << "\" complex2_rate=\"" << m_complex2Rate 
      << "\" name=\"" << name()
      << "\" simple=\"" << m_simple << "\"/>"
      << endl;
}
